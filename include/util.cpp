#include "util.hpp"

void gotError(const std::string &msg, const int &err){
    std::cerr << msg << std::endl;
    exit(err);
}

// trim from end of string (right)
std::string &rtrim(std::string &s, const std::string &t) {
    s.erase(s.rfind(t), t.length());
    return s;
}

// trim from beginning of string (left)
std::string &ltrim(std::string &s, const std::string &t) {
    s.erase(s.find(t), t.length());
    return s;
}

// pass string and get vector back of elements split by delim
std::vector<std::string> split(std::string &s, std::string delim){
    int loc;
    std::vector<std::string> tokens;
    while(!s.empty()){
        if((loc = s.find(delim)) == std::string::npos){
            // No more delims, push_back the remaining string and break
            tokens.push_back(s);
            break;
        }
        tokens.push_back(s.substr(0, loc));
        s.erase(0, loc+1);
    }

    return tokens;
}



