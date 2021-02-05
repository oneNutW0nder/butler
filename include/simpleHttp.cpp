/**
 * Name:    Simon Buchheit
 * Email:   scb5436@rit.edu
 * File:    simpleHttp.cpp
 *
 * Implementation functions for the http::Request class.
 */

#include "simpleHttp.hpp"
#include "simpleSocket.hpp"
#include "util.hpp"

#include <regex>

#include <fmt/core.h>

/**
 * Builds a value HTTP/1.1 request given the values of the
 * member variables.
 *
 * Stores the resulting request in `m_req`
 */
void http::Request::render() {

    // Clear out this->m_req to ensure a clean request is built each time
    this->m_req = "";

    this->m_req = fmt::format("{} {} HTTP/1.1\r\n", this->m_method, this->m_path);
    this->m_req += fmt::format("Host: {}\r\n", this->m_host);
    this->m_req += fmt::format("User-Agent: {}\r\n", this->m_userAgent);
    this->m_req += fmt::format("Connection: {}\r\n", this->m_connectionType);

    // Add other headers given by the user
    auto iter = this->m_otherHeaders.begin();
    while (iter != this->m_otherHeaders.end()) {
        this->m_req += fmt::format("{}: {}\r\n", iter->first, iter->second);
        iter++;
    }

    // Add content length and body then we are good to go!
    this->m_req += fmt::format("Content-Length: {}\r\n", this->m_body.length());
    this->m_req += "\r\n";
    this->m_req += fmt::format("{}", this->m_body);


}

/**
 * Creates a connection using the socket::Socket class
 * which can be found in `simpleSocket.hpp/cpp`. The HTTP/1.1
 * request is sent, the response read and saved in `m_resp`.
 */
void http::Request::sendRequest() {

    socket::Socket sock = socket::Socket();
    sock.SetMTls(this->m_tls);
    sock.connectTo(this->m_host, this->m_port);
    sock.sendTo(this->m_req);
    sock.readFrom();
    this->m_resp = sock.GetMResp();

    // TODO: Figure out handling redirects...
    //       saving code below
//    sock.~socket::Socket();
//    do{

//        auto resp = sock.GetMResp();
//        std::string stringResp(resp.begin(), resp.end());
//        auto headers = parseHeaders(stringResp);
//
//        if (!headers["HTTP/1.1"].find("301") == std::string::npos
//            || !headers["HTTP/1.1"].find("302") == std::string::npos){
//            break;
//        }
//
//        std::string location = headers["Location:"];
//        auto parsedLocation = parseUrl(location);
//
//        this->m_host = parsedLocation["host"];
//        this->m_path = parsedLocation["path"];
//        this->m_port = parsedLocation["port"];
//        if(parsedLocation["scheme"] == "https")
//            this->m_tls = true;
//        else
//            this->m_tls = false;
//
//        sock.~Socket();
//
//    }while(this->m_redirects);

}

/**
 * Takes a URL and splits it into its respective parts.
 * Parts are the saved to the matching member vairables.
 *
 * @param url --> A valid URL such as `http(s)://www.rit.edu`
 *
 * Note: It is up to the user to provide a valid URL
 */
void http::Request::parseUrl(std::string &url) {

    std::string scheme;
    std::string host;
    std::string port;
    std::string path;

    int loc;

    if ((loc = url.find("https://")) != std::string::npos) {
        scheme = "https";
        this->m_tls = true;
        this->m_port = "443";
        url = ltrim(url, "https://");
    } else if ((loc = url.find("http://")) != std::string::npos) {
        scheme = "http";
        this->m_tls = false;
        this->m_port = "80";
        url = ltrim(url, "http://");
    } else {
        // This block handles receiving a path such as /absolute/path or "relative/path"
        // TODO:
    }

    // No path remaining in URL so we can setup defaults
    // Ex: url = www.rit.edu
    if ((loc = url.find("/")) == std::string::npos) {
        this->m_host = url;
        this->m_path = "/";
        url = ltrim(url, this->m_host);
    } else {
        this->m_host = url.substr(0, loc);
        this->m_path = url.substr(loc, url.length());
        url = ltrim(url, this->m_host);
        url = ltrim(url, this->m_path);
    }

    // Check for nonstandard port
    if ((loc = host.find(":")) != std::string::npos) {
        this->m_port = host.substr(loc + 1, this->m_host.length());
        url = rtrim(url, this->m_port);
    }

    // Return anything that wasn't parsed for the user to handle
    if (!url.empty()) {
        this->m_extra = url;
    }

}

/**
 * Can be used after sending a request to get a map of response headers
 * and the response body. Headers will be stored in `m_resp_headers` and
 * the body in `m_resp_body`.
 */
void http::Request::parseResp() {

    int loc_x;
    int loc_y;

    // Return an empty map if we don't find the end of the headers
    if ((loc_x = this->m_resp.find("\r\n\r\n")) == std::string::npos) {
        this->m_resp_headers = {};
        this->m_resp_body = "";
    } else {
        std::string tmp;

        while ((loc_x = this->m_resp.find("\r\n")) != std::string::npos) {
            // End the loop when we reach the \r\n\r\n
            if (loc_x == 0) {
                this->m_resp = ltrim(this->m_resp, "\r\n");
                this->m_resp_body = this->m_resp;
                break;
            }
            tmp = this->m_resp.substr(0, loc_x);
            loc_y = tmp.find(" ");
            this->m_resp_headers[tmp.substr(0, loc_y)] = tmp.substr(loc_y + 1, tmp.length() - 2);

            this->m_resp = ltrim(this->m_resp, tmp.append("\r\n"));
        }
    }


}

/**
 * Can be used on the body of a response with a given regex expression.
 *
 * @param rgx --> A regex expression that follows the EMCAScript standard
 * @return --> A set of matches for the regex
 */
std::unordered_set<std::string> http::Request::parseHtml(const std::string &rgx) {

    if (rgx.empty()) {
        return {};
    }

    std::unordered_set<std::string> unqVals = {};
    std::regex re(rgx, std::regex::ECMAScript);
    for (std::sregex_iterator it = std::sregex_iterator(this->m_resp_body.begin(), this->m_resp_body.end(), re);
         it != std::sregex_iterator(); it++) {
        unqVals.insert(it->str(0));
    }

    return unqVals;
}

