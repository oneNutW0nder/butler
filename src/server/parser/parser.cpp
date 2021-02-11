#include <iostream>
#include <fstream>
#include <string>

#include "argparse.hpp"
#include "util.hpp"
#include "httpParser.hpp"


int main(int argc, const char* argv[]){

    ArgumentParser args;
    args.addFinalArgument("filePath");
    args.parse(argc, argv);

    auto path = args.retrieve<std::string>("filePath");

    std::ifstream fd(path, std::ios::in);
    if(!fd.is_open()){
        gotError("Could not open file", 1);
    }

    std::string line;
    std::string contents;

    fd.seekg(0, std::ios::end);
    contents.resize(fd.tellg());
    fd.seekg(0, std::ios::beg);
    fd.read(&contents[0], contents.size());

    // Do the validation
    auto myValid = httpParser::Validator(contents);

    return 0;
}