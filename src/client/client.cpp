#include <iostream>
#include "simpleHttp.hpp"
#include "simpleSocket.hpp"

int main(int argc, char* argv[]){

    // parse command line args and validate

    // parse URL entered in argv
    std::cout << "Hello!"<<std::endl;

    // create Request object based on parsed URL
    http::Request myReq = http::Request("www.rit.edu", "http");
    myReq.SetMTls(false);
    myReq.render();

    std::cout << "Sending reqest?" << std::endl;

    auto resp = myReq.sendRequest();

    for(auto i: resp){
        std::cout << i;
    }

    // create Socket object

    // Send http request

    // Read response

    // parse html response

    return 0;
}