#include <iostream>
#include <csignal>
#include <string>
#include <thread>

#include <unistd.h>
#include <fmt/format.h>

#include "argparse.hpp"
#include "simpleServer.hpp"

/**
 * This program is an HTTP/HTTPS server. Adapted from Arthur O'Dwyer's
 * openssl with C++ tutorial: https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
 */
int main(const int argc, const char *argv[]){

    // Args: ./server --ip IP --port PORT [--cert CERT --key PEM]
    ArgumentParser args;
    args.addArgument("-i", "--ip", 1, false);
    args.addArgument("-p", "--port", 1, false);
    args.addArgument("-c", "--cert", 1, true);
    args.addArgument("-k", "--key", 1, true);
    args.parse(argc, argv);

    auto ip = args.retrieve<std::string>("ip");
    auto port = args.retrieve<std::string>("port");
    auto cert = args.retrieve<std::string>("cert");
    auto pem = args.retrieve<std::string>("key");

    if (!cert.empty() && pem.empty()){
        std::cerr << "ERROR: Provided certificate but no matching private key given..." << std::endl;
        std::cerr << args.usage();
        exit(0);
    }

    // Setup HTTPS/TLS things if needed
    bool https = false;
    auto ctx = server::UniquePtr<SSL_CTX>(SSL_CTX_new(TLS_method()));
    if (!cert.empty() && !pem.empty()){
        https = true;
        server::init_ssl();
        SSL_CTX_set_min_proto_version(ctx.get(), TLS1_2_VERSION);
        if (SSL_CTX_use_certificate_file(ctx.get(), cert.c_str(), SSL_FILETYPE_PEM) <= 0) {
            server::ssl_errors("[!] Failed to load cert file... exiting");
        }
        if (SSL_CTX_use_PrivateKey_file(ctx.get(), pem.c_str(), SSL_FILETYPE_PEM) <= 0) {
            server::ssl_errors("[!] Failed to load private key file... exiting");
        }
        std::cout << "[+] Executing in HTTPS mode" << std::endl;
    } else {
        std::cout << "[+] Executing in HTTP mode" << std::endl;
    }

    auto serverRoot = server::init_server();

    // Bind to the given IP and port
    auto listenBio = server::UniquePtr<BIO>(BIO_new_accept(fmt::format("{}:{}", ip, port).c_str()));
    if (BIO_do_accept(listenBio.get()) <= 0) {
        server::ssl_errors(fmt::format("FATAL: Could not bind to {} on port {}... exiting", ip, port).c_str());
    }

    // Setup SIGINT handler to cleanly shutdown the server
    static auto shutdown_server = [fd = BIO_get_fd(listenBio.get(), nullptr)]() {
        std::cout << "Shutting down..." << std::endl;
        close(fd);
    };
    signal(SIGINT, [](int) { shutdown_server(); });

    std::cout << "Waiting for connections!" << std::endl;
    std::cout << "Press Ctrl + C to shutdown" << std::endl;

    // Start the server loop!
    while (auto bio = server::new_connection(listenBio.get())) {
        // If HTTPS push an SLL BIO into the chain
        if (https) {
            bio = std::move(bio) | server::UniquePtr<BIO>(BIO_new_ssl(ctx.get(), 0));
        }

        server::requestHandler(std::move(bio), serverRoot, https, port);

        // This "threading" is actually sequential and less efficient than the above function call
        // The connection acception method used in this loop supports concurrent connections
        //      while the serving of requests happens sequentially.

        /* The thread below by itself seg faults and is slower version of sequential if you join() */
//        std::thread i(server::requestHandler, std::move(bio), serverRoot, std::ref(https));
//        i.join();

    }

    return 0;
}