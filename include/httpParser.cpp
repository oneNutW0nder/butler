//
// Created by simon on 2/9/2021.
//
#include <iostream>

#include "httpParser.hpp"
#include "util.hpp"

// parser class takes a full request and parses everything
httpParser::Validator::Validator(std::string &request){
    try{
        // Search for double \r\n\r\n for quick invalidation
        if (request.find("\r\n\r\n") == std::string::npos){
            gotError("400 BAD REQUEST", 10);
        }

        // Handle the request line
        int loc;
        if((loc = request.find("\r\n")) == std::string::npos){
            gotError("400 BAD REQUEST", 11);
        }

        validateRequestLine(request.substr(0, loc));

    }catch(...){
        gotError("500 INTERNAL SERVER ERROR", 500);
    }

}

// pass string and get vector back of elements split by delim
std::vector<std::string> httpParser::Validator::split(std::string &s, std::string delim){
    int loc;
    std::vector<std::string> tokens;
    while(!s.empty()){
        if((loc = s.find(delim)) == std::string::npos){
            // No more delims, push_back the remaining string and break
            tokens.push_back(s);
            break;
        }
        tokens.push_back(s.substr(0, loc));
        s.erase(0, loc+1);
    }

    return tokens;
}

bool httpParser::Validator::validateRequestTarget(std::string reqTarget){
    // origin-form    = absolute-path [ "?" query ]
    // absolute-path = 1*( "/" segment ) --> We only need to check for the "/*"
    // absolute-form  = absolute-URI --> Must accept but only for proxies
    // authority-form = authority --> Only used in CONNECT
    // asterisk-form  = "*" --> Only used in OPTIONS
    return true;
}

bool httpParser::Validator::validateRequestLine(std::string reqLine) {
    // From RFC 7230
    // request-line   = method SP request-target SP HTTP-version CRLF
    //                    *                                       *
    // A "*" means the field above has been validated so far during execution
    // TODO: Might need to validate the \r\n as well
    std::cout << reqLine << std::endl;

    auto tokens = split(reqLine, " ");
    for (auto i : tokens){
        std::cout << i << " : " << i.length()<< std::endl;
    }

    // There should only be 3 fields contained in the request Line
    if (tokens.size() != 3){
        gotError("400 BAD REQUEST", 20);
    }

    // Validate request target

    // Validate HTTP version
    // TODO: Not sure about hard checking the version
    if (tokens[2] != "HTTP/1.1"){
        gotError("400 BAD REQUEST", 21);
    }
    return true;
}
