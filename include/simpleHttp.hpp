#ifndef SIMPLE_HTTP_HPP
#define SIMPLE_HTTP_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

namespace simpleHtpp{

    class Request{
        private:
            std::string m_method;
            std::string m_uri;
            std::string m_host;
            std::string m_port;
            std::string m_userAgent;
            std::string m_connectionType;
            std::string m_body;

            std::map<std::string, std::string> m_otherHeaders;

            bool m_redirects;

        public:

            // Methods
            void sendRequest();
            void render();
            void redirect();

            // Custom Constructor
            Request(std::string method = "GET", std::string uri = "/", std::string host = "", 
                    std::string port = "80", std::string userAgent = "Butler Client", std::string connectionType = "close",
                    std::string body = "", std::map<std::string, std::string> otherHeaders = {}, bool redirects = false){

                        m_method = method;
                        m_uri = uri;
                        m_host = host;
                        m_port = port;
                        m_userAgent = userAgent;
                        m_connectionType = connectionType;
                        m_body = body;
                        m_otherHeaders = otherHeaders;
                        m_redirects = redirects;

                    }

    };

}



#endif