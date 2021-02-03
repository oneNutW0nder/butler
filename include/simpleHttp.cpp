#include "simpleHttp.hpp"
#include "simpleSocket.hpp"
#include <utility>
#include <fmt/core.h>

// Custom Constructor
http::Request::Request(std::string host, std::string port){

    this->m_host = std::move(host);
    this->m_port = std::move(port);

}

void http::Request::render(){

    // Ensure port is included in host header if non standard port
    if (this->m_port != "80" && this->m_port != "443"
        && this->m_port != "http" && this->m_port != "https"){
        this->m_host += ":";
        this->m_host += this->m_port;
    }

    this->m_req = fmt::format("{} {} HTTP/1.1\r\n", this->m_method, this->m_path);
    this->m_req += fmt::format("Host: {}\r\n", this->m_host);
    this->m_req += fmt::format("User-Agent: {}\r\n", this->m_userAgent);
    this->m_req += fmt::format("Connection: {}\r\n", this->m_connectionType);

    // Add other headers given by the user
    auto iter = this->m_otherHeaders.begin();
    while (iter != this->m_otherHeaders.end()){
        this->m_req += fmt::format("{}: {}\r\n", iter->first, iter->second);
        iter++;
    }

    // Add content length and body then we are good to go!
    this->m_req += fmt::format("Content-Length: {}\r\n", this->m_body.length());
    this->m_req += "\r\n";
    this->m_req += fmt::format("{}", this->m_body);


}

// Sends request
std::vector<uint8_t> http::Request::sendRequest(){

    socket::Socket sock = socket::Socket();

    sock.SetMTls(this->m_tls);
    sock.connectTo(this->m_host, this->m_port);
    sock.sendTo(this->m_req);

    sock.readFrom();
    sock.cleanup();

    return sock.GetMResp();

}

// Handles redirects
void http::Request::redirect(){

}
