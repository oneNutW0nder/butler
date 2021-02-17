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

    server::init_ssl();

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
    // TODO: add in HTTPS bio chain
    std::cout << "Waiting for connections!" << std::endl;
    while (auto bio = server::new_connection(listenBio.get())){
        try {
            throw(server::httpException("testing custom exception", 500, "SERVER ERROR"));
            std::cout << "Connection Received" << std::endl;
            std::string req = server::receive_http_message(bio.get());
            // TODO: Change things in VALIDATOR
            auto valid = httpParser::Validator(req);
            std::cout << "Valid Data received:" << std::endl;
            std::cout << req << std::endl;
            std::cout << valid.GetMMethod() << std::endl;

            std::string tmp = "HTTP/1.1 200 OK\r\nContent-Length: 9\r\n\r\nHi berber";
            server::sendTo(bio.get(), tmp);

            // TODO: --> Feed request into parser
            //       --> If parser is valid grab the requested resource and log to file
            //       --> Serve request based on the method/resource
            //       --> Respond with appropriate response
        }
        // Catch integers which represent error HTTP code
        catch (server::httpException& e) {
            // HTTP/1.1 e
            std::string resp = "HTTP/1.1 ";
            resp += std::to_string(e.GetMStatusCode());
            resp += e.GetMCodeMsg();
            resp += "\r\n\r\n";
            std::cout << resp << std::endl;
            break;
        }
        // Catch all other exceptions and respond with 500 code
        catch (...) { std::cerr << "all other errors lol" << std::endl; }

    }

    return 0;
}