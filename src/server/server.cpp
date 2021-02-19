#include <iostream>
#include <csignal>
#include <string>

#include <unistd.h>
#include <fmt/format.h>

#include "argparse.hpp"
#include "simpleServer.hpp"
#include "httpParser.hpp"

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

    bool https = false;
    if (!cert.empty() && !pem.empty()){
        https = true;
    }
    else if (!cert.empty() && pem.empty()){
        std::cerr << "ERROR: Provided certificate but no matching private key given..." << std::endl;
        std::cerr << args.usage();
        exit(0);
    }

    auto serverRoot = server::init_server();
    server::init_ssl();

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

    // TODO: --> Add in HTTPS bio chain
    //       --> Add in threading for handling requests
    std::cout << "Waiting for connections!" << std::endl;
    std::cout << "Press Ctrl + C to shutdown" << std::endl;

    // Start the server loop!
    while (auto bio = server::new_connection(listenBio.get())){
        try {
            // Read request and validate
            std::cout << "Connection Received" << std::endl;
            std::string req = server::receive_http_message(bio.get());

            // Validate and get resource/params
            auto valid = httpParser::Validator(req);
            // TODO: finish param/resource parsing
            auto resources = server::parseResource(valid.GetMReqTarget(), valid.GetMAbsoluteUri());
            std::cout << resources.first << " :: " << resources.second << std::endl;
            // TODO: switch on method type and do method things
            server::serveRequest(resources.first, valid.GetMMethod(), serverRoot);

            auto defResp = server::makeResponse("200", "OK", "Default content");
            server::sendTo(bio.get(), defResp);
        }
        // Catch custom server exceptions that contain info about error type and message
        catch (server::httpException& e) {
            auto resp = server::makeResponse(std::to_string(e.GetMStatusCode()), e.GetMCodeMsg(), e.GetMErrMsg());
            server::sendTo(bio.get(), resp);
        }
        // Catch all other exceptions and respond with 500 code
        catch (...) {
            auto resp = server::makeResponse("500", "Internal Server Error", "General Error");
            server::sendTo(bio.get(), resp);
        }

    }

    return 0;
}