/**
 * Name:    Simon Buchheit
 * Email:   scb5436@rit.edu
 * File:    client.cpp
 *
 * This application makes a request to a URL provided by the user.
 * It based on the response it will perform a regex operation
 * on the response body which will match URLs. The list of URLs
 * matched by the regex is filtered down to only be URLs that do not
 * go to the same host.
 *
 * Example: input(https://www.rit.edu)
 *          output(URLs not of host `www.rit.edu`)
 */

#include <iostream>
#include <iomanip>
#include "simpleHttp.hpp"
#include "argparse.hpp"

void prettyLine(size_t val){
    for(val; val > 0; val--){
        std::cout << "=";
    }
    std::cout << std::endl;
}

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

    // This regex was taken from: https://stackoverflow.com/questions/3809401/what-is-a-good-regular-expression-to-match-a-url
    // and modified to fit my needs
    auto unqRefs = myReq.parseHtml(
            "(https?:)?(\\/\\/)(www\\.)?([-a-zA-Z0-9@:%._\\+~#=]{1,256}\\.[a-zA-Z0-9()]{1,6}\\b)([-a-zA-Z0-9()@:%_\\+.~#?&//=]*)");

    // Remove URLs with the hostname in it
    auto it = unqRefs.begin();
    while (it != unqRefs.end()) {
        if (it->find(myReq.GetMHost()) != std::string::npos) {
            it = unqRefs.erase(it);
        } else {
            ++it;
        }
    }

    std::string banner = "========== EXTERNAL REFERENCES FOR " + myReq.GetMHost() + " ==========\n";

    prettyLine(banner.length()-1);
    std::cout << banner;
    prettyLine(banner.length()-1);

    for (auto i : unqRefs) {
        std::cout << i << std::endl;
    }
    prettyLine(banner.length());
    std::cout << "Total Unique External References: " << unqRefs.size() << std::endl;

    return 0;
}

