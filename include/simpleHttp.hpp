#ifndef SIMPLE_HTTP_HPP
#define SIMPLE_HTTP_HPP

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stdint.h>

namespace http{

class Request{
    private:
        std::string m_method = "GET";
        std::string m_path = "/";
        std::string m_userAgent = "Butler Client";
        std::string m_connectionType = "close";

        std::string m_host;
        std::string m_port;
        std::string m_body;
        std::string m_req;
        std::map<std::string, std::string> m_otherHeaders;

        bool m_redirects = false;
        bool m_tls = false;

    public:
        // Custom Constructor
        Request(std::string host, std::string port);

        // Methods
        std::vector<uint8_t> sendRequest();
        void render();
        void redirect();
        void printRequest();

        // ==== SETTERS & GETTERS ====
        const std::string &GetMMethod() const {
            return m_method;
        }
        
        void SetMMethod(const std::string &m_method) {
            this->m_method = m_method;
        }

        const std::string &GetMPath() const {
            return m_path;
        }

        void SetMPath(const std::string &m_path) {
            this->m_path = m_path;
        }

        const std::string &GetMHost() const {
            return m_host;
        }

        void SetMHost(const std::string &m_host) {
            this->m_host = m_host;
        }

        const std::string &GetMPort() const {
            return m_port;
        }

        void SetMPort(const std::string &m_port) {
            this->m_port = m_port;
        }

        const std::string &GetMUserAgent() const {
            return m_userAgent;
        }

        void SetMUserAgent(const std::string &m_user_agent) {
            m_userAgent = m_user_agent;
        }

        const std::string &GetMConnectionType() const {
            return m_connectionType;
        }

        void SetMConnectionType(const std::string &m_connection_type) {
            m_connectionType = m_connection_type;
        }

        const std::string &GetMBody() const {
            return m_body;
        }

        void SetMBody(const std::string &m_body) {
            this->m_body = m_body;
        }

        const std::map<std::string, std::string> &GetMOtherHeaders() const {
            return m_otherHeaders;
        }

        void SetMOtherHeaders(const std::map<std::string, std::string> &m_other_headers) {
            m_otherHeaders = m_other_headers;
        }

        bool GetMRedirects() const {
            return m_redirects;
        }

        void SetMRedirects(bool m_redirects) {
            this->m_redirects = m_redirects;
        }

        const bool &GetMTls() const {
                return this->m_tls;
            }

        void SetMTls(const bool &m_tls){
            this->m_tls = m_tls;
        }


    };

}



#endif