//
// Created by simon on 2/16/2021.
//

#include <iostream>
#include <argparse.hpp>

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

    return 0;
}