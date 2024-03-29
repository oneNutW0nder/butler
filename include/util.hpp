#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

void gotError(const std::string &msg, const int &err);

std::vector<std::string> split(std::string &s, std::string delim);

std::string &rtrim(std::string &s, const std::string &t);

std::string &ltrim(std::string &s, const std::string &t);

std::string trimSpace(std::string str);

void urlDecode(std::string &str);
#endif
