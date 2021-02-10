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



