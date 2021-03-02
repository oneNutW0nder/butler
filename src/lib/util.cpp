#include "util.hpp"

void gotError(const std::string& msg, const int& err) {
  std::cerr << msg << std::endl;
  exit(err);
}

// trim from end of string (right)
std::string& rtrim(std::string& s, const std::string& t) {
  s.erase(s.rfind(t), t.length());
  return s;
}

// trim from beginning of string (left)
std::string& ltrim(std::string& s, const std::string& t) {
  s.erase(s.find(t), t.length());
  return s;
}

// pass string and get vector back of elements split by delim
std::vector<std::string> split(std::string& s, std::string delim) {
  int loc;
  std::vector<std::string> tokens;
  while (!s.empty()) {
    if ((loc = s.find(delim)) == std::string::npos) {
      // No more delims, push_back the remaining string and break
      tokens.push_back(s);
      break;
    }
    tokens.push_back(s.substr(0, loc));
    s.erase(0, loc + 1);
  }

  return tokens;
}

// Trims leading and trailing whitespace and tabs
std::string trimSpace(std::string str) {
  int leftidx;
  int rightidx;

  // left trim
  if ((leftidx = str.find_first_not_of(" \t")) != std::string::npos) {
    str.erase(0, leftidx);
  }

  // right trim
  if ((rightidx = str.find_first_of(" \t")) != std::string::npos) {
    str.erase(rightidx, str.size());
  }

  return str;
}

void urlDecode(std::string& str) {
  std::map<std::string, std::string> encDic = {
      {"%3A", ":"}, {"%2F", "/"}, {"%3F", "?"}, {"%23", "#"}, {"%5B", "["},
      {"%5D", "]"}, {"%40", "@"}, {"%21", "!"}, {"%24", "$"}, {"%26", "&"},
      {"%27", "'"}, {"%28", "("}, {"%29", ")"}, {"%2A", "*"}, {"%2B", "+"},
      {"%2C", ","}, {"%3B", ";"}, {"%3D", "="}, {"%25", "%"}, {"%20", " "},
      {"+", " "}};
  int loc = -1;
  for (auto& i : encDic) {
    while (str.find(i.first) != std::string::npos) {
      str.replace(str.find(i.first), 3, i.second);
    }
  }
}
