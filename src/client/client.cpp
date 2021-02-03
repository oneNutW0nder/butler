#include <iostream>
#include "simpleHttp.hpp"
#include "util.hpp"
#include "argparse.hpp"

int main(int argc, const char *argv[]) {

    ArgumentParser parser;
    parser.addFinalArgument("url");

    // Usage will be printed automatically if invalid format
    parser.parse(argc, argv);
    auto url = parser.retrieve<std::string>("url");


    parseUrl("https://www.rit.edu/");

    // parse URL entered in argv
//    std::cout << "Hello!" << std::endl;
//
//    // create Request object based on parsed URL
//    auto myReq = http::Request("www.rit.edu", "http");
//    myReq.SetMTls(false);
//    myReq.render();
//
//    auto resp = myReq.sendRequest();
//
//    for (auto i: resp) {
//        std::cout << i;
//    }

    // parse html response

    return 0;
}