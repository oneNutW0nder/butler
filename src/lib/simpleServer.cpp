#include "simpleServer.hpp"

#include <fmt/format.h>

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <utility>

#include "httpParser.hpp"
#include "util.hpp"

namespace server {

// Load openssl
void init_ssl() {
  SSL_library_init();
  SSL_load_error_strings();
}

/**
 * Initialize the server and return the serverRoot dir.
 *
 * @return --> The root directory for the server default: "$HOME/butler-server"
 */
std::filesystem::path init_server() {
  char* home;
  if ((home = getenv("HOME")) == nullptr) {
    std::cerr << "[!] FATAL: Please set the $HOME env variable... exiting"
              << std::endl;
    exit(11);
  }

  std::filesystem::path serverRoot(home);
  serverRoot /= SERVER_ROOT;
  // Create server root if doesn't exist
  if (!std::filesystem::exists(serverRoot)) {
    if (!std::filesystem::create_directory(serverRoot)) {
      std::cerr << "[!] FATAL: Failed to create server root at $HOME/butler... "
                   "exiting"
                << std::endl;
      exit(12);
    }
  }

  // Write default index.html file if it doesn't exist
  if (!std::filesystem::exists(serverRoot / "index.html")) {
    std::ofstream helloFile(serverRoot / "index.html", std::ofstream::out);
    if (!helloFile.is_open()) {
      std::cerr << "[!] FATAL: Failed to create default index.html at "
                   "$HOME/butler/index.html... exiting"
                << std::endl;
      exit(13);
    }

    std::string welcome =
        "<h1>Welcome!</h1><br><h1>Thank you for using the Butler Web "
        "Server!</h1>";
    helloFile.write(welcome.c_str(), welcome.size());
    helloFile.close();
  }

  // METHOD AGNOSTIC VARS
  setenv("GATEWAY_INTERFACE", "CGI/1.1", 0);
  setenv("SERVER_PROTOCOL", "HTTP/1.1", 0);
  setenv("REMOTE_HOST", "127.0.0.1", 0);
  setenv("REDIRECT_STATUS", "0", 0);

  std::cout << "[+] Server initialization finished." << std::endl;
  std::cout << "[+] Server Root at: " << serverRoot << std::endl;

  return serverRoot;
}

// Used for fatal errors with SSL
// https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
void ssl_errors(const char* str) {
  std::cerr << str << std::endl;
  ERR_print_errors_fp(stderr);
  exit(10);
}

/**
 * Reads a chunk of 1024 bytes from a BIO. Throws
 * std::runtime_error if a problem occurs
 * Adapted from: https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
 *
 * @param bio --> BIO to read from
 *
 * @return --> Data read from the BIO
 */
std::string receiveChunk(BIO* bio, const bool& https) {
  char buffer[1024];
  int len = BIO_read(bio, buffer, sizeof(buffer));
  // Check for TLS conn attempt in HTTP mode
  // This does not display anything to the user but it "frees" the connection
  // Users will see a "secure connection failed" message in the browser
  if (!https) {
    if (buffer[0] == '\026' && buffer[1] == '\003' && buffer[2] == '\001') {
      throw(httpException("Attempted HTTPS connection on HTTP only server", 400,
                          "Bad Request"));
    }
  }
  if (len < 0) {
    throw(std::runtime_error("ERROR reading from BIO"));
  } else if (len > 0) {
    return std::string(buffer, len);
  } else if (BIO_should_retry(bio)) {
    return receiveChunk(bio, https);
  } else {
    throw(std::runtime_error("ERROR reading from empty BIO"));
  }
}

/**
 * Splits request headers. Used for searching for content-length
 * when reading from the BIO
 * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
 *
 * @param text --> Request headers for splitting
 *
 * @return --> vector of split headers
 */
std::vector<std::string> split_headers(const std::string& text) {
  std::vector<std::string> lines;
  const char* start = text.c_str();
  while (const char* end = strstr(start, "\r\n")) {
    lines.emplace_back(start, end);
    start = end + 2;
  }
  return lines;
}

/**
 * Receives a full HTTP request from a BIO. It reads from the BIO
 * until it can find a "content-length" header which it will use to
 * read the full request body, otherwise it just reads in all the request
 * headers and returns them as a std::string.
 * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
 *
 * @param bio --> pointer to BIO to read from
 *
 * @return --> String representation of the HTTP request
 */
std::string receive_http_message(BIO* bio, const bool& https) {
  std::string contentLen = "Content-length";
  std::transform(contentLen.begin(), contentLen.end(), contentLen.begin(),
                 ::tolower);
  std::string headers = server::receiveChunk(bio, https);
  char* end_of_headers = strstr(&headers[0], "\r\n\r\n");
  while (end_of_headers == nullptr) {
    headers += server::receiveChunk(bio, https);
    end_of_headers = strstr(&headers[0], "\r\n\r\n");
  }
  std::string body = std::string(end_of_headers + 4, &headers[headers.size()]);
  headers.resize(end_of_headers + 2 - &headers[0]);
  size_t content_length = 0;
  for (const std::string& line : server::split_headers(headers)) {
    if (const char* colon = strchr(line.c_str(), ':')) {
      auto header_name = std::string(&line[0], colon);
      if (header_name == "content-length") {
        content_length = std::stoul(colon + 1);
      }
    }
  }
  while (body.size() < content_length) {
    body += server::receiveChunk(bio, https);
  }
  return headers + "\r\n" + body;
}

/**
 * Sends data across a BIO
 * Adapted From: https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
 *
 * @param bio --> BIO to write data to
 * @param resp --> Data to write, usually an HTTP request
 */
void sendTo(BIO* bio, const std::string& resp) {
  BIO_write(bio, resp.data(), resp.size());
  BIO_flush(bio);
}

/**
 * This function handles the weirdness that is openssl's accept BIOs.
 * When a new connection is received it adds the new BIO to the chain
 * where it was accepted. To handle the new connection, we have to pop
 * the new BIO off the chain.
 * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
 *
 * @param listenBIO --> The BIO where you are doing the listening
 *
 * @return --> returns the new connection BIO or nullptr if failed
 */
server::UniquePtr<BIO> new_connection(BIO* listenBIO) {
  if (BIO_do_accept(listenBIO) <= 0) {
    return nullptr;
  }
  return server::UniquePtr<BIO>(BIO_pop(listenBIO));
}

/**
 * Constructs a response for the server to send back to the client. It takes in
 * necessary parameters like the status code, status msg, and any content for
 * the body.
 *
 * @param code          --> HTTP status code
 * @param codeMsg       --> HTTP status code message
 * @param content       --> Content to return in response body
 * @param otherHeaders  --> And aditional headers to include in the response
 *
 * @return              --> response as a string
 */
std::string makeResponse(
    const std::string& code, const std::string& codeMsg,
    const std::string& content,
    const std::map<std::string, std::string>& otherHeaders) {
  // TODO: Add DATE header and value to all responses
  // HTTP/1.1 CODE CODE_MSG
  // Get date
  char buff[100];
  time_t now = time(nullptr);
  struct tm tm = *gmtime(&now);
  strftime(buff, sizeof(buff), "%a, %d %b %Y %H:%M:%S %Z", &tm);

  std::string resp = fmt::format("HTTP/1.1 {} {}\r\n", code, codeMsg);
  resp += fmt::format("Date: {}\r\n", buff);
  resp += "Server: Butler\r\n";
  resp += fmt::format("Content-Length: {}\r\n", content.length());
  resp += fmt::format("Content-Type: text/html\r\n");
  for (auto& i : otherHeaders) {
    resp += fmt::format("{}: {}\r\n", i.first, i.second);
  }
  resp += fmt::format("\r\n");
  resp += content;

  return resp;
}

/**
 * Takes in a request-target from a request and parses it for the
 * requested resource. The "params" returned are not processed any
 * further here. They are simply grabbed from the request-target and
 * returned
 *
 * @param reqTarget --> request-target from a request
 * @param absolute  --> Whether or not the request-target is an absolute-uri
 *
 * @return  --> a pair of <resource, params>
 */
std::pair<std::string, std::string> parseResource(std::string reqTarget,
                                                  const bool& absolute) {
  int loc;
  std::string params;

  // Decode the URL first
  urlDecode(reqTarget);

  // Handle absolute request
  if (absolute) {
    // Strip off scheme for https:// || http://
    if ((loc = reqTarget.find("https://")) != std::string::npos) {
      // strip off https:// scheme
      reqTarget.erase(0, 8);
    } else {
      // Stip off http:// scheme
      reqTarget.erase(0, 7);
    }

    // Check for "/"
    if ((loc = reqTarget.find('/')) != std::string::npos) {
      reqTarget.erase(0, loc);
    } else {
      // If no "/" is found then set default value
      reqTarget = '/';
    }
  }

  // Default resource request
  if (reqTarget == "/") {
    return {"/index.html", params};
  }

  // Return path to target if no params or fragment found
  if (reqTarget.find('?') == std::string::npos &&
      reqTarget.find('#') == std::string::npos) {
    return {reqTarget, params};
  }

  // Check for and strip fragment
  if ((loc = reqTarget.find('#')) != std::string::npos) {
    reqTarget.erase(loc);
  }

  // Check for params and save params if exist
  if ((loc = reqTarget.find('?')) != std::string::npos) {
    params = reqTarget.substr(loc + 1);
    reqTarget.erase(loc);
  }

  // If we make it here all that should remain is just the path to resource
  return {reqTarget, params};
}

/**
 * Takes in a struct of requestInfo that contains all the pertinent
 * information about a request. This function handles serving each
 * support method and returns the appropriate error response if
 * an error is encountered.
 *
 * @param reqInfo   --> requestInfo struct of information
 *
 * @return  --> returns a response to send back to the client
 */
std::string serveRequest(struct requestInfo* reqInfo) {
  // Lock this entire method because of the filesystem operations
  // std::lock_guard<std::mutex> lk(mut);

  // * PUT AND DELETE FIRST BECAUSE THEY ARE STATE ALTERING WIHTOUT 404
  // * DELETE METHOD
  if (reqInfo->method == "DELETE") {
    // Remove returns false if the delete failed and true if it was deleted
    if (!std::filesystem::remove_all(reqInfo->serverRoot +=
                                     reqInfo->resource)) {
      // Return 500 because we can't determine exactly why it failed
      throw(httpException(
          "An error has occurred. Please contact the system admin.", 500,
          "Internal Server Error"));
    }

    return makeResponse(
        "200", "OK",
        "Successfully deleted the requested resource :: " + reqInfo->resource,
        {});
  }

  // * PUT METHOD
  if (reqInfo->method == "PUT") {
    // Check for existing file
    if (!std::filesystem::exists(reqInfo->serverRoot += reqInfo->resource)) {
      // Create directory chain for lots of non existent paths
      // This will create a directory for the final resource as well but will be
      // deleted later
      if (!std::filesystem::create_directories(reqInfo->serverRoot)) {
        // Failure to create directories return 403
        throw(httpException("Failed to create file: Forbidden", 403,
                            "Forbidden"));
      }

      // Delete final directory because that should be the file targeted by PUT
      if (!std::filesystem::remove(reqInfo->serverRoot)) {
        // Failure to delete --> 403
        // Failed to create message displayed because this function is involved
        // in
        //      the file creation process.... albeit janky
        throw(httpException("Failed to create file: Forbidden", 403,
                            "Forbidden"));
      }
    }

    // Finally create the file
    std::ofstream fd(reqInfo->serverRoot, std::ios::trunc);
    fd << reqInfo->body;
    fd.close();

    // Build location absolute-URI
    std::string loc;
    if (reqInfo->https) {
      loc = "https://";
    } else {
      loc = "http://";
    }

    loc.append(DEFAULT_SERVER_NAME);
    loc.append(":" + reqInfo->port);
    loc.append(reqInfo->resource);

    // Only supporting a 201 response for both creation and modified content
    return makeResponse("201", "Created", "", {{"Location", loc}});
  }

  // * HEAD METHOD
  if (reqInfo->method == "HEAD") {
    // check for: 404 File not found!
    if (!std::filesystem::exists(reqInfo->serverRoot += reqInfo->resource)) {
      // Throw with no "err_msg" beacuse HEAD response should have no body
      throw(httpException("", 404, "Not Found"));
    }
    return makeResponse("200", "OK", "", {});
  }

  // check for: 404 File not found!
  // This applies to GET, and POST
  // reqInfo->serverRoot is the full path to the requested resource after this
  // check
  if (!std::filesystem::exists(reqInfo->serverRoot += reqInfo->resource)) {
    throw(httpException("The file you requested does not exist", 404,
                        "Not Found"));
  }

  // Check if we can open the file for reading
  std::ifstream fd(reqInfo->serverRoot, std::ios::in);
  if (!fd.is_open()) {
    // Assume forbidden if can't open... no good way to check for permission
    // failure
    throw(httpException(
        "Failed to open the requested resource: Permission denied", 403,
        "Forbidden"));
  }

  bool php = false;
  if (reqInfo->serverRoot.ends_with(".php")) {
    fd.close();
    php = true;
    // Set path to script file
    setenv("SCRIPT_FILENAME", reqInfo->serverRoot.c_str(), 1);
  }

  // * GET METHOD
  if (reqInfo->method == "GET") {
    if (php) {
      // Set GET env vars
      setenv("REQUEST_METHOD", "GET", 0);
      setenv("QUERY_STRING", reqInfo->params.c_str(), 0);

      // Write php to tmp file
      // TODO: Check error code and return 500 if exec failed
      system("php-cgi --no-header > /tmp/phpOut");
      std::ifstream tmpfile{"/tmp/phpOut"};
      std::stringstream out;
      out << tmpfile.rdbuf();

      // Unset env vars
      unsetenv("REQUEST_METHOD");
      unsetenv("QUERY_STRING");

      // Return the output of the PHP script
      return makeResponse("200", "OK", out.str(), {});
    } else {
      std::stringstream tmp;
      tmp << fd.rdbuf();
      return makeResponse("200", "OK", tmp.str(), {});
    }
  }

  // * POST METHOD
  if (reqInfo->method == "POST") {
    if (php) {
      // Set POST env vars
      setenv("REQUEST_METHOD", "POST", 0);
      setenv("CONTENT_LENGTH", std::to_string(reqInfo->body.length()).c_str(),
             0);
      setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 0);
      setenv("BODY", reqInfo->body.c_str(), 0);

      // PIPE $BODY to php
      // ! VULNERABLE
      system("echo $BODY | php-cgi > /tmp/phpOut");
      std::ifstream tmpfile{"/tmp/phpOut"};
      std::stringstream out;
      out << tmpfile.rdbuf();

      // Unset env vars
      unsetenv("REQUEST_METHOD");
      unsetenv("CONTENT_LENGTH");
      unsetenv("CONTENT_TYPE");
      unsetenv("BODY");

      return makeResponse("200", "OK", out.str(), {});
    } else {
      std::stringstream tmp;
      tmp << fd.rdbuf();
      return makeResponse(
          "200", "OK",
          tmp.str().append("<br>Sent POST to non PHP resource! " +
                           reqInfo->body),
          {});
    }
  }
  // Throw an exception here because we should never be in an invalidated state
  throw(httpException("Extreme Fatal Error", 500, "Internal Server Error"));
}

/**
 * This function handles the validation, parsing, and serving of a request.
 * This could be used to implement true threading in the future, but for now
 * we can handle ~20k requests in a little over a second.
 *
 * @param bio           --> BIO representing the current connection
 * @param serverRoot    --> the root of the server
 * @param https         --> whether or not we are in HTTPS mode
 * @param port          --> Port for the server
 */
void requestHandler(BIO* bio, std::string serverRoot, const bool& https,
                    const std::string& port) {
  try {
    std::string req = server::receive_http_message(bio, https);

    // Validate and get resource/params
    auto valid = httpParser::Validator(req);
    auto resources =
        server::parseResource(valid.GetMReqTarget(), valid.GetMAbsoluteUri());

    std::cout << resources.second << std::endl;

    // Log to log file and include response code
    // mut.lock();
    std::ofstream log("./butler.log", std::ios::app);
    log << fmt::format("{} {} {}\n", valid.GetMMethod(), valid.GetMReqTarget(),
                       valid.GetMVersion());
    log.close();
    // mut.unlock();

    std::cout << fmt::format("[+] Received request: {} {}", valid.GetMMethod(),
                             valid.GetMReqTarget())
              << std::endl;

    // Create a requestInfo struct to pass around
    server::requestInfo reqInfo;
    reqInfo.method = valid.GetMMethod();
    reqInfo.resource = resources.first;
    reqInfo.params =
        resources.second;  // ! Param different for POST and GET
                           // ! POST --> Array in ENV var "BODY"
                           // ! GET  --> key:val passed to "php-cgi"
    reqInfo.body = valid.GetMBody();
    reqInfo.serverRoot = std::move(serverRoot);
    reqInfo.https = https;
    reqInfo.port = port;

    // Send response
    auto resp = server::serveRequest(&reqInfo);

    server::sendTo(bio, resp);
  }
  // Catch custom server exceptions that contain info about error type and
  // message
  catch (server::httpException& e) {
    auto resp = server::makeResponse(std::to_string(e.GetMStatusCode()),
                                     e.GetMCodeMsg(), e.GetMErrMsg(), {});
    server::sendTo(bio, resp);
  }
  // Catch all other exceptions and respond with 500 code
  catch (...) {
    auto resp = server::makeResponse("500", "Internal Server Error",
                                     "General Error", {});
    server::sendTo(bio, resp);
  }
}
}  // namespace server