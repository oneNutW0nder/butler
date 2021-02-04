#include <iostream>
#include "simpleHttp.hpp"
#include "argparse.hpp"

int main(int argc, const char *argv[]) {

    ArgumentParser parser;
    parser.addFinalArgument("url");
    parser.parse(argc, argv);

    auto url = parser.retrieve<std::string>("url");
    auto myReq = http::Request();

    myReq.parseUrl(url);
    myReq.render();

    myReq.sendRequest();
    myReq.parseResp();

    // parse html response


    return 0;
}

