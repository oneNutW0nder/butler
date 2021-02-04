#include "simpleHttp.hpp"
#include "simpleSocket.hpp"
#include "util.hpp"
#include <fmt/core.h>


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

// Sends request
std::vector<uint8_t> http::Request::sendRequest() {

    socket::Socket sock = socket::Socket();
    sock.SetMTls(this->m_tls);
    sock.connectTo(this->m_host, this->m_port);
    sock.sendTo(this->m_req);
    sock.readFrom();
    return sock.GetMResp();

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

void http::Request::parseUrl(std::string &url) {

    std::string scheme;
    std::string host;
    std::string port;
    std::string path;
    std::string other;

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

std::map<std::string, std::string> http::Request::parseHeaders(std::string &req) {

    int loc_x;
    int loc_y;

    // Return an empty map if we don't find the end of the headers
    if ((loc_x = req.find("\r\n\r\n")) == std::string::npos) {
        return {};
    }

    std::map<std::string, std::string> headers;
    std::string tmp;

    while ((loc_x = req.find("\r\n")) != std::string::npos) {
        // End the loop when we reach the \r\n\r\n
        if (loc_x == 0) {
            break;
        }
        tmp = this->m_req.substr(0, loc_x);
        loc_y = tmp.find(" ");
        headers[tmp.substr(0, loc_y)] = tmp.substr(loc_y + 1, tmp.length() - 2);

        this->m_req = ltrim(req, tmp.append("\r\n"));
    }

    return headers;
}

