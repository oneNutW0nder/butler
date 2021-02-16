//
// Created by simon on 2/16/2021.
//

#include <iostream>
#include <fmt/format.h>
#include <csignal>
#include <unistd.h>

#include "argparse.hpp"
#include "simpleServer.hpp"

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

    bool https = false;
    if (!cert.empty() && !pem.empty()){
        https = true;
    }
    else if (!cert.empty() && pem.empty()){
        std::cerr << "ERROR: Provided certificate but no matching private key given..." << std::endl;
        std::cerr << args.usage();
        exit(0);
    }

    // TODO: Configure and start the server
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

    // Start the server loop!
    std::cout << "Waiting for connections!" << std::endl;
    while (auto bio = server::new_connection(listenBio.get())){
        try {
            // TODO: Server handling / logic
            std::cout << "Connection Received" << std::endl;
        } catch (...) {
            std::cerr << "errors lol" << std::endl;
            // TODO: Print error message to console and possibly log file
        }
    }

    return 0;
}