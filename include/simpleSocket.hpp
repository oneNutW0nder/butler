#ifndef SIMPLE_SOCKET_HPP
#define SIMPLE_SOCKET_HPP

#include <vector>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace socket{

    class Socket{

        private:
            // Set by constructor
            SSL_CTX* m_ctx;
            BIO* m_conn_bio;
            BIO* m_ssl_bio;
            char m_resp_buffer[1030];
            bool tls;

            // Not set by constructor
            std::vector<uint8_t> m_resp;

        public:

            void readFrom();
            void sendTo();

            void setTLS(bool tls);
            bool getTLS();

            // Custom constructor
            Socket();

    };
}

#endif