#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <string>

void gotError(const std::string &msg, const int &err);

std::string &rtrim(std::string &s, const std::string &t);

std::string &ltrim(std::string &s, const std::string &t);



#endif
