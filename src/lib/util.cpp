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

std::string exec(std::string command) {
  char buffer[128];
  std::string result = "";

  // Open pipe to file
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return "popen failed!";
  }

  // read till end of process:
  while (!feof(pipe)) {
    // use buffer to read and add to result
    if (fgets(buffer, 128, pipe) != NULL) result += buffer;
  }

  pclose(pipe);
  return result;
}
