//
// Created by simon on 2/9/2021.
//
#include <iostream>
#include <algorithm>
#include <string>

#include "httpParser.hpp"
#include "util.hpp"

// parser class takes a full request and parses everything
httpParser::Validator::Validator(std::string &request){
    try{
        std::cout << request << std::endl;
        // Search for double \r\n\r\n for quick invalidation
        if (request.find("\r\n\r\n") == std::string::npos){
            gotError("400 BAD REQUEST", 10);
        }

        // Handle the request line
        int loc;
        if((loc = request.find("\r\n")) == std::string::npos){
            gotError("400 BAD REQUEST", 11);
        }

        // Validate request line and remove it if its valid
        validateRequestLine(request.substr(0, loc));
        request.erase(0, loc+2);

        // Remaining request should just be headers
        validateHeaders(request);


        // TODO: Print Success 200 OK

    }catch(...){
        gotError("500 INTERNAL SERVER ERROR", 500);
    }

}

void httpParser::Validator::validateHeaders(std::string s) {
    // Collect all the headers
    // Check for quick exit condition
    if (s.find("\r\n\r\n") == std::string::npos){
        gotError("400 BAD REQUEST", 30);
    }

    int loc_x;
    int loc_y;
    std::string tmp;

    while ((loc_x = s.find("\r\n")) != std::string::npos){
        // Stop grabbing headers when we reach \r\n\r\n
        if (loc_x == 0) {
            s = ltrim(s, "\r\n");
            this->m_body = s;
            break;
        }

        tmp = s.substr(0, loc_x);
        loc_y = tmp.find(':');
        this->m_headers[tmp.substr(0, loc_y)] = tmp.substr(loc_y + 1, tmp.length()-2);
        s = ltrim(s, tmp.append("\r\n"));
    }

    // Complete validation
    std::string lowerHeader;
    for (auto i : this->m_headers){
        // Get lower version for case insensitivity
        lowerHeader.resize(i.first.size());
        std::transform(i.first.begin(), i.first.end(), lowerHeader.begin(), ::tolower);

        // No spaces allowed in header-value
        if (i.first.find(" ") != std::string::npos){
            gotError("400 BAD REQUEST", 31);
        }

        // No delimeters allowed in header value
        if (i.first.find_first_of(DELIMETERS) != std::string::npos){
            std::cout << i.first << std::endl;
            gotError("400 BAD REQUEST", 32);
        }

        // Search for "Host:" header
        if (lowerHeader == "host"){
            this->m_host = i.second;
        }

        if (i.first == "content-type"){
            this->m_content_type = i.second;
        }

        if (i.first == "content-length"){
            this->m_content_length = std::stoi(i.second);
        }

    }

}


bool httpParser::Validator::validateRequestTarget(const std::string& reqTarget){
    // origin-form    = absolute-path [ "?" query ]
    //                  absolute-path = 1*( "/" segment ) --> We only need to check for the "/*"
    // absolute-form  = absolute-URI --> Must accept but only for proxies
    // authority-form = authority --> Only used in CONNECT **NOT USED**
    // asterisk-form  = "*" --> Only used in OPTIONS **NOT USED**

    // Quick check for single "/" (origin-form)
    // TODO: this feels like such a cheep check
    if (reqTarget.find('/') == 0 && !reqTarget.empty()){
        this->m_req_target = reqTarget;
        return true;
    }

    // Absolute-form
    // Must be absolute-URI (http|s://lots.more.text.here/)
    if (reqTarget.find("http://") != std::string::npos ||
        reqTarget.find("https://") != std::string::npos){
        // Ensure more text than just the schemes
        // 10 --> len(http://a.a)
        if (reqTarget.size() >= 10){
            this->m_req_target = reqTarget;
            return true;
        }
    }
    // Assume false otherwise
    return false;
}

void httpParser::Validator::validateRequestLine(std::string reqLine) {
    // From RFC 7230
    // request-line   = method SP request-target SP HTTP-version CRLF
    //                    *                                       *
    // A "*" means the field above has been validated so far during execution
    // TODO: Might need to validate the \r\n as well

    auto tokens = split(reqLine, " ");

    // There should only be 3 fields contained in the request Line
    if (tokens.size() != 3){
        gotError("400 BAD REQUEST", 20);
    }

    // Validate request target
    if (!validateRequestTarget(tokens[1])){
        gotError("400 BAD REQUEST", 21);
    }

    // Validate HTTP version
    // TODO: Not sure about hard checking the version
    if (tokens[2] != "HTTP/1.1"){
        gotError("400 BAD REQUEST", 21);
    }

    this->m_method = tokens[0];
    this->m_req_target = tokens[1];
    this->m_version = tokens[2];
}

const std::string &httpParser::Validator::getMMethod() const {
    return m_method;
}

void httpParser::Validator::setMMethod(const std::string &mMethod) {
    m_method = mMethod;
}

const std::string &httpParser::Validator::getMReqTarget() const {
    return m_req_target;
}

void httpParser::Validator::setMReqTarget(const std::string &mReqTarget) {
    m_req_target = mReqTarget;
}

const std::string &httpParser::Validator::getMVersion() const {
    return m_version;
}

void httpParser::Validator::setMVersion(const std::string &mVersion) {
    m_version = mVersion;
}

const std::string &httpParser::Validator::getMBody() const {
    return m_body;
}

void httpParser::Validator::setMBody(const std::string &mBody) {
    m_body = mBody;
}

const std::map<std::string, std::string> &httpParser::Validator::getMHeaders() const {
    return m_headers;
}

void httpParser::Validator::setMHeaders(const std::map<std::string, std::string> &mHeaders) {
    m_headers = mHeaders;
}

