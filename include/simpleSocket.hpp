#ifndef SIMPLE_SOCKET_HPP
#define SIMPLE_SOCKET_HPP

#include <vector>
#include <iostream>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace socket {

    class Socket {

    private:

        SSL_CTX *m_ctx = nullptr;
        BIO *m_ssl_bio = nullptr;
        BIO *m_conn_bio;
        char m_resp_buffer[1030];
        bool m_tls = false;
        std::vector<uint8_t> m_resp;

    public:
        void readFrom();

        void sendTo(const std::string &data);

        void connectTo(const std::string &host, const std::string &port);


        // Custom constructor
        Socket();

        ~Socket();

        // ==== SETTERS & GETTERS ====
        [[nodiscard]] const std::vector<uint8_t> &GetMResp() {
            return this->m_resp;
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