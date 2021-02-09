#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>

void gotError(const std::string &msg, const int &err);

std::string &rtrim(std::string &s, const std::string &t);

std::string &ltrim(std::string &s, const std::string &t);

std::vector<std::string> split(std::string &s, std::string &delim);

bool validateRequestLine(std::string reqLine);

#endif
