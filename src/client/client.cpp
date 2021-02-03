#include <iostream>
#include "simpleHttp.hpp"

int main(int argc, char* argv[]){

    // parse command line args and validate

    // parse URL entered in argv
    std::cout << "Hello!"<<std::endl;

    // create Request object based on parsed URL
    auto myReq = http::Request("www.rit.edu", "http");
    myReq.SetMTls(false);
    myReq.render();

    std::cout << "Sending reqest?" << std::endl;

    auto resp = myReq.sendRequest();

    for(auto i: resp){
        std::cout << i;
    }

    // parse html response

    return 0;
}