//
// Created by simon on 2/9/2021.
//

#ifndef BUTLER_CLIENT_HTTPPARSER_HPP
#define BUTLER_CLIENT_HTTPPARSER_HPP

#include <vector>
#include <string>

namespace httpParser{
    class httpParser {
    private:
    public:
        httpParser(std::string request);
        std::vector<std::string> split(std::string &s, std::string delim);
        bool validateRequestLine(std::string reqLine);
        bool validateRequestTarget(std::string reqTarget);
    };
}



#endif //BUTLER_CLIENT_HTTPPARSER_HPP
