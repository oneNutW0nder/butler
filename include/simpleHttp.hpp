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
        // Set by constructor
        std::string m_method;
        std::string m_path;
        std::string m_host;
        std::string m_port;
        std::string m_userAgent;
        std::string m_connectionType;
        std::string m_body;
        std::map<std::string, std::string> m_otherHeaders;
        bool m_redirects;

        // Class variables not set by constructor
        std::string m_req;

    public:

        // Methods
        void sendRequest();
        void render();
        void redirect();
        void printRequest();

        // Custom Constructor
        Request(std::string method = "GET", std::string path = "/", std::string host = "",
                std::string port = "80", std::string userAgent = "Butler Client", std::string connectionType = "close",
                std::string body = "", std::map<std::string, std::string> otherHeaders = {}, bool redirects = false);

        const std::string &GetMMethod() const {
            return m_method;
        }
        
        void SetMMethod(const std::string &m_method) {
            Request::m_method = m_method;
        }

        const std::string &GetMPath() const {
            return m_path;
        }

        void SetMPath(const std::string &m_path) {
            Request::m_path = m_path;
        }

        const std::string &GetMHost() const {
            return m_host;
        }

        void SetMHost(const std::string &m_host) {
            Request::m_host = m_host;
        }

        const std::string &GetMPort() const {
            return m_port;
        }

        void SetMPort(const std::string &m_port) {
            Request::m_port = m_port;
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
            Request::m_body = m_body;
        }

        const std::map<std::string, std::string> &GetMOtherHeaders() const {
            return m_otherHeaders;
        }

        void SetMOtherHeaders(const std::map<std::string, std::string> &m_other_headers) {
            m_otherHeaders = m_other_headers;
        }

        bool IsMRedirects() const {
            return m_redirects;
        }

        void SetMRedirects(bool m_redirects) {
            Request::m_redirects = m_redirects;
        }

        const std::string &GetMReq() const {
            return m_req;
        }

        void SetMReq(const std::string &m_req) {
            Request::m_req = m_req;
        }

        };

}



#endif