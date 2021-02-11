#ifndef BUTLER_CLIENT_HTTPPARSER_HPP
#define BUTLER_CLIENT_HTTPPARSER_HPP

#include <vector>
#include <string>
#include <map>

#define DELIMETERS "(),/:;<=>?@[\\]{}"

namespace httpParser{
    /**
     * httpParser::Validator is used to validate the syntax of an HTTP/1.1
     * request. The methods used in validation try to adhere to the rules of
     * RFC 2616, 7230, and 7231.
     *
     * Usage:
     *      std::string myrequest = getSomeRequest();
     *      auto example = httpParser::Validator(myrequest);
     *
     * If the request is valid "example" will have member values set based on the
     * request that was parsed. If the request is invalid, the current state is that
     * the program will print an error message and exit.
     */
    class Validator {

    private:
        std::string m_method;
        std::string m_req_target;
        std::string m_version;
        std::string m_body;
        std::string m_host;

        int m_content_length = -1;
        std::string m_content_type;
        bool m_transfer_encoding;
        bool m_content_range;
        bool m_seenHost = false;
        bool m_absolute_uri = false;

        std::map<std::string, std::string> m_headers;

    public:
        explicit Validator(std::string &request);

        bool validateRequestTarget(const std::string &reqTarget);
        void validateRequestLine(std::string reqLine);
        void validateHeaders(std::string s);

    };
}



#endif //BUTLER_CLIENT_HTTPPARSER_HPP
