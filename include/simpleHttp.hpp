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

            // Custom Constructor
            Request(std::string method = "GET", std::string path = "/", std::string host = "", 
                    std::string port = "80", std::string userAgent = "Butler Client", std::string connectionType = "close",
                    std::string body = "", std::map<std::string, std::string> otherHeaders = {}, bool redirects = false);

    };

}



#endif