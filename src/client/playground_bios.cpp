#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

static int always_true_callback(X509_STORE_CTX *ctx, void *arg)
{
    return 1;
}


int main(){

    std::string req = "GET / HTTP/1.1\r\nHost: https://google.com\r\nConnection: close\r\n\r\n";

    auto ctx = SSL_CTX_new(TLS_client_method());
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

    // Load system certs
    SSL_CTX_set_default_verify_paths(ctx);
    SSL_CTX_set_cert_verify_callback(ctx, always_true_callback, NULL);

    auto bio = BIO_new_connect("google.com:https");
    if(bio == nullptr){
        std::cout << "Error bio connect" << std::endl;
        return -1;
    }

    std::cout << "in between!" << std::endl;

    if(BIO_do_connect(bio) <= 0) {
        std::cout << "Error connecting to server" << std::endl;
        return -1;
    }
    

    auto ssl_bio = BIO_new_ssl(ctx, 1);

    BIO_write(ssl_bio, req.c_str(), req.length());
    std::cout << "here!" << std::endl;

    char buffer[1030];
    int cutoff = 1030;
    std::vector<uint8_t> data;

    std::cout << "down here!" << std::endl;
    while(cutoff > 0 ){
        cutoff = BIO_read(ssl_bio, buffer, 1024);
        std::cout << "Packet length recieved: " << cutoff << std::endl;
        data.insert(data.end(), buffer, buffer+cutoff);
    }

    for(auto i: data){
        std::cout << i;
    }




    return 0;
}