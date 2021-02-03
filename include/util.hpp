#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <string>
#include <map>

inline std::string& rtrim(std::string& s, const char* t);
inline std::string& ltrim(std::string& s, const char* t);
inline std::string& trim(std::string& s, const char* t);

/**
 * Take in a URL (Ex. https://www.rit.edu)
 * Parse and split into:
 *      Scheme: https
 *      Host: www.rit.edu
 *      Path: Default = /
 *
 * @param url
 */
std::map<std::string, std::string> parseUrl(std::string &url);

/**
 * Take in the entire request and return the headers with associated values
 * @param req
 */
std::map<std::string, std::string> parseHeaders(std::string req);

#endif
