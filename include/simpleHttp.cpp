#include "simpleHttp.hpp"
#include <iostream>
#include <fmt/core.h>

// Custom Constructor
http::Request::Request(std::string method, std::string path, std::string host, std::string port, std::string userAgent, std::string connectionType,std::string body, std::map<std::string, std::string> otherHeaders, bool redirects){

    this->m_method = method;
    this->m_path = path;
    this->m_host = host;
    this->m_port = port;
    this->m_userAgent = userAgent;
    this->m_connectionType = connectionType;
    this->m_body = body;
    // this->m_otherHeaders = otherHeaders;
    this->m_otherHeaders = {{"Test", "Header"},{"Another", "Header"}};
    this->m_redirects = redirects;

}

void http::Request::render(){

    // Ensure port is included in host header if non standard port
    if (this->m_port != "80" && this->m_port != "443"){
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

void http::Request::sendRequest(){

}

void http::Request::redirect(){

}

void http::Request::printRequest(){
    std::cout << this->m_req << std::endl;
}