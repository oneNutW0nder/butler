//
// Created by simon on 2/9/2021.
//

#ifndef BUTLER_CLIENT_HTTPPARSER_HPP
#define BUTLER_CLIENT_HTTPPARSER_HPP

#include <vector>
#include <string>
#include <map>

#define DELIMETERS "(),/:;<=>?@[\\]{}"

namespace httpParser{
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

        std::map<std::string, std::string> m_headers;

    public:
        explicit Validator(std::string &request);

        bool validateRequestTarget(const std::string &reqTarget);
        void validateRequestLine(std::string reqLine);
        void validateHeaders(std::string s);

        [[nodiscard]] const std::string &getMMethod() const;
        [[nodiscard]] const std::string &getMReqTarget() const;
        [[nodiscard]] const std::string &getMVersion() const;
        [[nodiscard]] const std::string &getMBody() const;
        [[nodiscard]] const std::map<std::string, std::string> &getMHeaders() const;

        void setMMethod(const std::string &mMethod);
        void setMReqTarget(const std::string &mReqTarget);
        void setMVersion(const std::string &mVersion);
        void setMBody(const std::string &mBody);
        void setMHeaders(const std::map<std::string, std::string> &mHeaders);

    };
}



#endif //BUTLER_CLIENT_HTTPPARSER_HPP
