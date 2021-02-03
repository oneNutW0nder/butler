#include <iostream>
#include <string>
#include <vector>
#include <fmt/format.h>

#include <string.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>

int main(){

    bool tls = false;
    SSL_CTX* ctx = nullptr;

    std::string host = "www.rit.edu";
    std::string scheme = "http";

    std::string req = "GET / HTTP/1.1\r\nHost: www.rit.edu\r\nConnection: close\r\n\r\n";

    // Use and create connection bio by default
    // Will be chained with SSL bio if needed
    auto conn_bio = BIO_new(BIO_s_connect());
    BIO_set_conn_hostname(conn_bio, fmt::format("{}:{}", host, scheme).c_str());

    if(tls){
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();

        ctx = SSL_CTX_new(TLS_client_method());
        SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

        auto ssl = SSL_new(ctx);
        SSL_set_tlsext_host_name(ssl, host.c_str());
        SSL_set_connect_state(ssl);

        auto ssl_bio = BIO_new(BIO_f_ssl());
        BIO_set_ssl(ssl_bio, ssl, BIO_CLOSE);

        // Build BIO chain with connect BIO
        conn_bio = BIO_push(ssl_bio, conn_bio);
    }

    // Send req
    BIO_write(conn_bio, req.c_str(), req.length());

    char buffer[1030];
    int cutoff = 1030;
    std::vector<uint8_t> data;

    // read all data
    while(cutoff > 0 ){
        cutoff = BIO_read(conn_bio, buffer, 1024);
        std::cout << "Packet length recieved: " << cutoff << std::endl;
        data.insert(data.end(), buffer, buffer+cutoff);
    }

    for(auto i: data){
        std::cout << i;
    }

    // Cleanup
    BIO_free_all(conn_bio);
    if (ctx != nullptr){
        SSL_CTX_free(ctx);
    }

    return 0;
}