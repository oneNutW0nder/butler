#include "util.hpp"
#include <regex>

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


//}
//
//void parseHtml(){
//    // Regex from RFC 3986 (https://tools.ietf.org/html/rfc3986#appendix-B)
////    std::regex rgx("^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?" );
//    std::regex rgx("^((\\w+):)?(\\/\\/((\\w+)?(:(\\w+))?@)?([^\\/\?:]+)(:(\\d+))?)?(\\/?([^\\/\\?#][^\\?#]*)?)?(\\?([^#]+))?(#(\\w*))?");
//    std::smatch matches;
//    if (std::regex_search(url, matches, rgx)) {
//        std::cout << "Text contains the phrase 'regular expressions'\n";
//        for (size_t i = 0; i < matches.size(); ++i) {
//            if(matches[i].str().length() < 2) {
//                std::cout << i << ": '" << matches[i].str() << "'\n";
//            }
//        }
//    }
