#include <iostream>
#include "simpleHttp.hpp"
#include "util.hpp"
#include "argparse.hpp"
#include <map>

int main(int argc, const char *argv[]) {

    ArgumentParser parser;
    parser.addFinalArgument("url");

    // Usage will be printed automatically if invalid format
    parser.parse(argc, argv);
    auto url = parser.retrieve<std::string>("url");


    std::map<std::string, std::string> urlInfo = parseUrl(url);
    for( auto i : urlInfo){
        std::cout << i.first << " :: " << i.second << std::endl;
    }

    auto myReq = http::Request(urlInfo["host"], urlInfo["port"]);

    if(urlInfo["scheme"] == "https")
        myReq.SetMTls(true);
    else
        myReq.SetMTls(false);

    myReq.SetMPath(urlInfo["path"]);
    myReq.render();

    std::cout << myReq.GetMReq() << std::endl;

    auto resp = myReq.sendRequest();

//    for (auto i: resp) {
//        std::cout << i;
//    }

    // parse html response

    return 0;
}