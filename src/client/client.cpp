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

    // TODO: Make the output pretty for the grader :)
    //       also check for an empty unqRefs
    for (auto i : unqRefs) {
        std::cout << i << std::endl;
    }
    std::cout << unqRefs.size() << std::endl;

    return 0;
}

