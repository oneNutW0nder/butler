#include "util.hpp"
#include <regex>

// trim from end of string (right)
inline std::string& rtrim(std::string& s, const std::string& t){
    int loc = s.rfind(t);
    s.erase(loc, t.length());
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const std::string& t){
    int loc = s.find(t);
    s.erase(loc, t.length());
    return s;
}


std::map<std::string, std::string> parseUrl(std::string &url){

    std::string scheme;
    std::string host;
    std::string port;
    std::string path;
    std::string other;

    int loc;

    if ((loc = url.find("https://")) != std::string::npos) {
        scheme = "https";
        port = "443";
        url = ltrim(url, "https://");
    }
    else if ((loc = url.find("http://")) != std::string::npos){
        scheme = "http";
        port = "80";
        url = ltrim(url, "http://");
    }
    else{
        // This block handles receiving a path such as /absolute/path or "relative/path"
        // TODO:
    }

    // No path remaining in URL so we can setup defaults
    // Ex: url = www.rit.edu
    if ((loc = url.find("/")) == std::string::npos){
        host = url;
        path = "/";
        url = ltrim(url, host);
    }else{
        host = url.substr(0, loc);
        path = url.substr(loc, url.length());
        url = ltrim(url, host);
        url = ltrim(url, path);
    }

    // Check for nonstandard port
    if ((loc = host.find(":")) != std::string::npos){
        port = host.substr(loc+1, host.length());
        url = rtrim(url, port);
    }

    // Return anything that wasn't parsed for the user to handle
    if(!url.empty()){
        other = url;
    }

    return {{"scheme", scheme},
            {"host", host},
            {"port", port},
            {"path", path},
            {"other", other}};
}

std::map<std::string, std::string> parseHeaders(std::string& req){

    int loc_x;
    int loc_y;

    // Return an empty map if we don't find the end of the headers
    if((loc_x = req.find("\r\n\r\n")) == std::string::npos){
        return {};
    }

    std::map<std::string, std::string> headers;
    std::string tmp;

    while((loc_x = req.find("\r\n")) != std::string::npos){
        if (loc_x == 0){
            break;
        }
        tmp = req.substr(0, loc_x);
//        tmp = ltrim(tmp, ":");
        // If we are somehow not in a valid header this will fail
        loc_y = tmp.find(" ");
        headers[tmp.substr(0, loc_y)] = tmp.substr(loc_y + 1, tmp.length()-2);

        req = ltrim(req, (tmp + "\r\n"));
    }

    return headers;
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
