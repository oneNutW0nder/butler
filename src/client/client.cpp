#include <iostream>
#include "simpleHttp.hpp"
#include "simpleSocket.hpp"

int main(int argc, char* argv[]){

    // parse command line args and validate

    // parse URL entered in argv
    std::cout << "Hello!"<<std::endl;

    // create Request object based on parsed URL
    http::Request myReq = http::Request();
    myReq.render();
    myReq.printRequest();

    // create Socket object

    // Send http request

    // Read response

    // parse html response

    return 0;
}