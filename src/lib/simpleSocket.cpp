/**
 * Name:    Simon Buchheit
 * Email:   scb5436@rit.edu
 * File:    simpleSocket.cpp
 *
 * Contains implementation functions for socket::Socket class.
 */

#include "simpleSocket.hpp"
#include <cstring>
#include <fmt/core.h>

socket::Socket::Socket() {
    this->m_ctx = nullptr;
    this->m_conn_bio = nullptr;
    this->m_ssl_bio = nullptr;
    this->m_tls = false;

    // Ensure buffer is clear of any junk data before use
    memset(this->m_resp_buffer, 0, sizeof(this->m_resp_buffer));
}

socket::Socket::~Socket() {
    // I am not convinced that this sequence truly tears down
    // the connection and respective BIOs
    BIO_free_all(this->m_conn_bio);
    if (this->m_ctx != nullptr) {
        SSL_CTX_free(this->m_ctx);
    }
}

/**
 * Using openssl's BIOs, open a connection to `host:port` and wrap the
 * connection with TLS if needed.
 *
 * @param host --> Example: `www.rit.edu`
 * @param port --> Example: `http` or `80` (will match any service in /etc/services)
 */
void socket::Socket::connectTo(const std::string &host, const std::string &port) {

    // Default connection bio that can be chained with
    // an SSL filter bio if needed
    this->m_conn_bio = BIO_new(BIO_s_connect());
    BIO_set_conn_hostname(this->m_conn_bio, fmt::format("{}:{}", host, port).c_str());

    if (this->m_tls) {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();
        this->m_ctx = SSL_CTX_new(TLS_client_method());
        SSL_CTX_set_min_proto_version(this->m_ctx, TLS1_2_VERSION);

        auto ssl = SSL_new(this->m_ctx);
        SSL_set_tlsext_host_name(ssl, host.c_str());
        SSL_set_connect_state(ssl);

        this->m_ssl_bio = BIO_new(BIO_f_ssl());
        BIO_set_ssl(m_ssl_bio, ssl, BIO_CLOSE);

        // Build BIO chain with connect BIO
        this->m_conn_bio = BIO_push(m_ssl_bio, m_conn_bio);

    }
}

/**
 * Receive data from a connection. Specifically set up for HTTP responses.
 */
void socket::Socket::readFrom() {

    int cutoff = 1030;
    while (cutoff > 0) {
        cutoff = BIO_read(this->m_conn_bio, this->m_resp_buffer, 1024);
        if (cutoff == -1) {
            if (BIO_should_retry(this->m_conn_bio)) {
                std::cout << "socket::readFrom() :: BIO read retry..." << std::endl;
            } else {
                std::cout << "socket::readFrom() :: BIO read fail... exiting" << std::endl;
                exit(42);
            }
        }
        this->m_resp.insert(this->m_resp.end(), this->m_resp_buffer, this->m_resp_buffer + cutoff);

    }

}

/**
 * @param data --> Example: "my data I want to send over the connection"
 */
void socket::Socket::sendTo(const std::string &data) {

    // Connection BIOs will automatically attempt the connection when data
    // is written to them. This is why no explicit connect call is made
    BIO_write(this->m_conn_bio, data.c_str(), data.length());

}


