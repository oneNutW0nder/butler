#include <iostream>
#include <algorithm>
#include <string>

#include "httpParser.hpp"
#include "util.hpp"
#include "simpleServer.hpp"

/**
 * Constructor handles validating a raw request. It will try to validate
 * the given request and catch any exceptions. If an exception occurs it will
 * generate a HTTP 500 error message.
 *
 * @param request --> The raw request including all parts
 */
httpParser::Validator::Validator(std::string &request) {
    // Search for double \r\n\r\n for quick invalidation
    if (request.find("\r\n\r\n") == std::string::npos) {
        throw (server::httpException("Invalid Request", 400, "Bad Request"));
    }

    // Handle the request line
    int loc;
    if ((loc = request.find("\r\n")) == std::string::npos) {
        throw (server::httpException("Invalid Request", 400, "Bad Request"));
    }

    // Validate request line and remove it if its valid
    validateRequestLine(request.substr(0, loc));
    request.erase(0, loc + 2);

    // Remaining request should just be headers
    validateHeaders(request);
}

/**
 * Handles the validation of the request headers. There are many attempts
 * to follow the RFC (2616/7230/7231) in here.
 *
 * @param s --> The request with the request-line stripped from it (should only contain headers now)
 */
void httpParser::Validator::validateHeaders(std::string s) {

    int loc_x;
    int loc_y;
    std::string tmp;
    std::string tmp_header;


    // Grab all the headers one line at a time
    while ((loc_x = s.find("\r\n")) != std::string::npos) {
        // Stop grabbing headers when we reach \r\n\r\n
        if (loc_x == 0) {
            s = ltrim(s, "\r\n");
            this->m_body = s;
            break;
        }

        tmp = s.substr(0, loc_x);
        loc_y = tmp.find(':');
        // Lowercase the potential header
        tmp_header = tmp.substr(0, loc_y);
        std::transform(tmp_header.begin(), tmp_header.end(), tmp_header.begin(), ::tolower);

        // Check for double host header
        if (tmp_header == "host") {
            if (this->m_seenHost) {
                throw (server::httpException("Double host header", 400, "Bad Request"));
            } else {
                this->m_seenHost = true;
            }
        }

        this->m_headers[tmp.substr(0, loc_y)] = tmp.substr(loc_y + 1, tmp.length() - 2);
        s = ltrim(s, tmp.append("\r\n"));
    }

    // Complete validation
    std::string lowerHeader;
    std::string trimmedVal;
    for (const auto &i : this->m_headers) {
        // Get lower version for case insensitivity
        lowerHeader.resize(i.first.size());
        trimmedVal = i.second;
        trimmedVal = trimSpace(trimmedVal);
        std::transform(i.first.begin(), i.first.end(), lowerHeader.begin(), ::tolower);

        // No spaces allowed in header-value
        if (i.first.find(" ") != std::string::npos) {
            throw (server::httpException("Invalid Request", 400, "Bad Request"));
        }

        // No delimeters allowed in header value
        if (i.first.find_first_of(DELIMETERS) != std::string::npos) {
            throw (server::httpException("Invalid Request", 400, "Bad Request"));
        }

        // Verify Host header is not empty
        if (lowerHeader == "host") {
            if (trimmedVal.empty()) {
                throw (server::httpException("Invalid Request", 400, "Bad Request"));
            }
            this->m_host = trimmedVal;
        }

        // Save content-type for later processing
        if (lowerHeader == "content-type") {
            this->m_content_type = trimmedVal;
        }

        // Save content length for later checking
        // This can throw an exception if std::stoi() fails to convert
        if (lowerHeader == "content-length") {
            this->m_content_length = std::stoi(trimmedVal);
        }

        // See if there is transfer-encoding header
        if (lowerHeader == "transfer-encoding") {
            this->m_transfer_encoding = true;
        }

        // Used for rejecting PUT
        if (lowerHeader == "content-range") {
            this->m_content_range = true;
        }

    }

    // Host header required
    if (!this->m_seenHost && !this->m_host.empty() && this->m_version != "HTTP/1.0") {
        throw (server::httpException("Invalid Request", 400, "Bad Request"));
    }

    // Host header val must be in an absolute URI
    // This is heavily exploitable
    if (this->m_absolute_uri) {
        if (this->m_req_target.find(this->m_host) == std::string::npos) {
            throw (server::httpException("Invalid Request", 400, "Bad Request"));
        }
    }

    // Verify that the HOST header matches the DEFAULT_SERVER_NAME unless HTTP/1.0
    // Note: hackable
    if (this->m_host.find(server::DEFAULT_SERVER_NAME) == std::string::npos && this->m_version == "HTTP/1.1") {
        throw (server::httpException("Invalid Request", 400, "Bad Request"));
    }

    // Do not allow both transfer-encoding and content-length
    if (this->m_transfer_encoding && this->m_content_length != -1) {
        throw (server::httpException("Invalid Request", 400, "Bad Request"));
    }

    // Do body checking for methods and content-length
    if (!this->m_body.empty()) {
        // 400 if there is a body on a GET or HEAD or DELETE request
        if (this->m_method == "GET" || this->m_method == "HEAD" || this->m_method == "DELETE") {
            throw (server::httpException("Invalid Request", 400, "Bad Request"));
        } else if (this->m_content_length != this->m_body.size()) {
            throw (server::httpException("Length required", 411, "LENGTH REQUIRED"));
        }
    }

    // Content-range header not allowed with PUT
    if (this->m_method == "PUT" && this->m_content_range) {
        throw (server::httpException("Invalid Request", 400, "Bad Request"));
    }

}


/**
 * Validates specifically the "request-target" section of the "request-line".
 * The only forms that are supported are "origin-form" and "absolute-form" because
 * this server does not support the CONNECT or OPTIONS methods.
 *
 * @param reqTarget --> The string that represents the "request-target"
 * @return --> True if valid :: False if not
 */
bool httpParser::Validator::validateRequestTarget(const std::string &reqTarget) {
    // origin-form    = absolute-path [ "?" query ]
    //                  absolute-path = 1*( "/" segment ) --> We only need to check for the "/*"
    // absolute-form  = absolute-URI --> Must accept but only for proxies
    // authority-form = authority --> Only used in CONNECT **NOT USED**
    // asterisk-form  = "*" --> Only used in OPTIONS **NOT USED**

    // Quick check for single "/" (origin-form)
    if (reqTarget.find('/') == 0 && !reqTarget.empty()) {
        this->m_req_target = reqTarget;
        return true;
    }

    // Absolute-form
    // Must be absolute-URI (http|s://lots.more.text.here/)
    // Only used for CONNECT method but still have to accept it
    if (reqTarget.find("http://") == 0 ||
        reqTarget.find("https://") == 0) {
        // Ensure more text than just the schemes
        // 10 --> len(http://a.a)
        if (reqTarget.size() >= 10) {
            this->m_req_target = reqTarget;
            this->m_absolute_uri = true;
            return true;
        }
    }
    // Assume false otherwise
    return false;
}

/**
 * Handles the validation of the "request-line" of an HTTP request.
 * This function will set the corresponding member values if all parts
 * are found to be valid.
 *
 * @param reqLine --> String that represents the "request-line"
 */
void httpParser::Validator::validateRequestLine(std::string reqLine) {

    auto tokens = split(reqLine, " ");

    // There should only be 3 fields contained in the request Line
    if (tokens.size() != 3) {
        throw (server::httpException("Invalid Request", 400, "Bad Request"));
    }

    // Verify we support the method
    std::vector<std::string> supported = {"GET", "HEAD", "POST", "PUT", "DELETE"};
    if (std::find(supported.begin(), supported.end(), tokens[0]) == supported.end()) {
        throw (server::httpException("Unsupported Method", 501, "Not Implemented"));
    }

    // Validate request target
    if (!validateRequestTarget(tokens[1])) {
        throw (server::httpException("Invalid Request", 400, "Bad Request"));
    }

    // Validate HTTP version
    if (tokens[2] != "HTTP/1.1" && tokens[2] != "HTTP/1.0") {
        throw (server::httpException("Version not supported", 505, "HTTP Version Not Supported"));
    }

    this->m_method = tokens[0];
    this->m_req_target = tokens[1];
    this->m_version = tokens[2];
}

