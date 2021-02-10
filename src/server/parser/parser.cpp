#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "argparse.hpp"
#include "simpleHttp.hpp"
#include "util.hpp"
#include "httpParser.hpp"

// Used in the switch case
enum Methods{
    BadReq, // Default value when nothing to match
    Get,
    Head,
    Post,
    Put,
    Delete
};



void validate(std::string &request){


    // TODO: might need to send the \r\n for validation
}

int main(int argc, const char* argv[]){

    ArgumentParser args;
    args.addFinalArgument("filePath");
    args.parse(argc, argv);

    auto path = args.retrieve<std::string>("filePath");
    std::cout << path << std::endl;


    std::map<std::string, Methods> method_map;
    method_map["GET"] = Get;
    method_map["HEAD"] = Head;
    method_map["POST"] = Post;
    method_map["PUT"] = Put;
    method_map["DELETE"] = Delete;

    std::string line;
    std::string method;
    std::string contents;

    std::ifstream fd(path, std::ios::in);
    if(!fd.is_open()){
        gotError("Could not open file", 1);
    }

    // Grab the method really quick
    fd >> method;
    fd.seekg(0);

    switch (method_map[method]) {
        case Get:
        case Head: // Extra method of my choice
        case Post:
        case Put:
        case Delete: {
            fd.seekg(0, std::ios::end);
            contents.resize(fd.tellg());
            fd.seekg(0, std::ios::beg);
            fd.read(&contents[0], contents.size());
            auto myValid = httpParser::Validator(contents);
            break;
        }
        default:
            gotError("400 BAD REQUEST", 99);
            break;

    }

    return 0;
}