#ifndef SIMPLE_HTTP_HPP
#define SIMPLE_HTTP_HPP

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstdint>
#include <unordered_set>

namespace http {
    class Request {

        /**
         *  Allows building, sending, and parsing of
         *  HTTP requests and responses.
         *
         *  Example usage:
         *      auto myReq = http::Request();
         *      myReq.parseUrl("https://www.rit.edu")
         *      myReq.render();
         *      myreq.sendRequest();
         *      myreq.parseResp();
         */
    private:
        std::string m_method = "GET";
        std::string m_path = "/";
        std::string m_userAgent = "Butler Client";
        std::string m_connectionType = "close";

        std::string m_host;
        std::string m_port;
        std::string m_body;
        std::string m_req;
        std::string m_extra;
        std::map<std::string, std::string> m_otherHeaders;

        std::map<std::string, std::string> m_resp_headers;
        std::map<std::string, std::string> m_req_headers;
        std::string m_resp_body;
        std::string m_req_body;
        std::string m_resp; 

        bool m_redirects = false;
        bool m_tls = false;

    public:
        // Methods
        void sendRequest();

        void render();

        void parseUrl(std::string &url);

        void parseHttp(const bool resp);

        std::unordered_set<std::string> parseHtml(const std::string &rgx);


        // ==== SETTERS & GETTERS ====
        [[nodiscard]] const std::string &GetMRespBody() const {
            return this->m_resp_body;
        }

        [[nodiscard]] const std::map<std::string, std::string> &GetMRespHeaders() const {
            return this->m_resp_headers;
        }

        [[nodiscard]] const std::string &GetMReq() const {
            return this->m_req;
        }

        [[nodiscard]] const std::string &GetMMethod() const {
            return this->m_method;
        }

        void SetMMethod(const std::string &m_method) {
            this->m_method = m_method;
        }

        [[nodiscard]] const std::string &GetMPath() const {
            return this->m_path;
        }

        void SetMPath(const std::string &m_path) {
            this->m_path = m_path;
        }

        [[nodiscard]] const std::string &GetMHost() const {
            return this->m_host;
        }

        void SetMHost(const std::string &m_host) {
            this->m_host = m_host;
        }

        [[nodiscard]] const std::string &GetMPort() const {
            return this->m_port;
        }

        void SetMPort(const std::string &m_port) {
            this->m_port = m_port;
        }

        [[nodiscard]] const std::string &GetMUserAgent() const {
            return this->m_userAgent;
        }

        void SetMUserAgent(const std::string &m_user_agent) {
            this->m_userAgent = m_user_agent;
        }

        [[nodiscard]] const std::string &GetMConnectionType() const {
            return this->m_connectionType;
        }

        void SetMConnectionType(const std::string &m_connection_type) {
            this->m_connectionType = m_connection_type;
        }

        [[nodiscard]] const std::string &GetMBody() const {
            return this->m_body;
        }

        void SetMBody(const std::string &m_body) {
            this->m_body = m_body;
        }

        [[nodiscard]] const std::map<std::string, std::string> &GetMOtherHeaders() const {
            return this->m_otherHeaders;
        }

        void SetMOtherHeaders(const std::map<std::string, std::string> &m_other_headers) {
            this->m_otherHeaders = m_other_headers;
        }

        [[nodiscard]] bool GetMRedirects() const {
            return m_redirects;
        }

        void SetMRedirects(bool m_redirects) {
            this->m_redirects = m_redirects;
        }

        [[nodiscard]] const bool &GetMTls() const {
            return this->m_tls;
        }

        void SetMTls(const bool &m_tls) {
            this->m_tls = m_tls;
        }


    };

}


#endif