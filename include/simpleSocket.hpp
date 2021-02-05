#ifndef SIMPLE_SOCKET_HPP
#define SIMPLE_SOCKET_HPP

#include <vector>
#include <iostream>
#include <unordered_set>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace socket {

    /**
     * Used to make connections and send data.
     *
     * Example usage:
     *  Socket mysock = Socket();
     *  mysock.SetMTls(true);
     *  mysock.connectTo("www.rit.edu", "443");
     *  mysock.sendTo(httpRequest);
     *  mysock.readFrom();
     */
    class Socket {

    private:

        SSL_CTX *m_ctx;
        BIO *m_ssl_bio;
        BIO *m_conn_bio;
        char m_resp_buffer[1030];
        bool m_tls;
        std::vector<uint8_t> m_resp;

    public:
        void readFrom();

        void sendTo(const std::string &data);

        void connectTo(const std::string &host, const std::string &port);

        // Custom constructor
        Socket();

        ~Socket();

        // ==== SETTERS & GETTERS ====
        [[nodiscard]] const std::string GetMResp() {
            return std::string(this->m_resp.begin(), this->m_resp.end());
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