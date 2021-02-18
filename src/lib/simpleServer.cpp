#include "simpleServer.hpp"

#include <fmt/format.h>

namespace server {

    // Load openssl
    void init_ssl(){
        SSL_library_init();
        SSL_load_error_strings();
    }

    // Used for fatal errors with SSL
    // https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
    void ssl_errors(const char *str) {
        std::cerr << str << std::endl;
        ERR_print_errors_fp(stderr);
        exit(10);
    }

    /**
     * Reads a chunk of 1024 bytes from a BIO. Throws
     * std::runtime_error if a problem occurs
     * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
     *
     * @param bio --> BIO to read from
     * @return --> Data read from the BIO
     */
    std::string receiveChunk(BIO *bio) {
        char buffer[1024];
        int len = BIO_read(bio, buffer, sizeof(buffer));
        if (len < 0) {
            throw(std::runtime_error("ERROR reading from BIO"));
        } else if (len > 0) {
            return std::string(buffer, len);
        } else if (BIO_should_retry(bio)) {
            return receiveChunk(bio);
        } else {
            throw(std::runtime_error("ERROR reading from empty BIO"));
        }
    }

    /**
     * Splits request headers. Used for searching for content-length
     * when reading from the BIO
     * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
     *
     * @param text --> Request headers for splitting
     * @return --> vector of split headers
     */
    std::vector<std::string> split_headers(const std::string& text) {
        std::vector<std::string> lines;
        const char *start = text.c_str();
        while (const char *end = strstr(start, "\r\n")) {
            lines.emplace_back(start, end);
            start = end + 2;
        }
        return lines;
    }


    /**
     * Receives a full HTTP request from a BIO. It reads from the BIO
     * until it can find a "content-length" header which it will use to
     * read the full request body, otherwise it just reads in all the request
     * headers and returns them as a std::string.
     * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
     *
     * @param bio --> pointer to BIO to read from
     * @return --> String representation of the HTTP request
     */
    std::string receive_http_message(BIO *bio) {
        std::string contentLen = "Content-length";
        std::transform(contentLen.begin(), contentLen.end(), contentLen.begin(), ::tolower);
        std::string headers = server::receiveChunk(bio);
        char *end_of_headers = strstr(&headers[0], "\r\n\r\n");
        while (end_of_headers == nullptr) {
            headers += server::receiveChunk(bio);
            end_of_headers = strstr(&headers[0], "\r\n\r\n");
        }
        std::string body = std::string(end_of_headers+4, &headers[headers.size()]);
        headers.resize(end_of_headers+2 - &headers[0]);
        size_t content_length = 0;
        for (const std::string& line : server::split_headers(headers)) {
            if (const char *colon = strchr(line.c_str(), ':')) {
                auto header_name = std::string(&line[0], colon);
                if (header_name == "content-length") {
                    content_length = std::stoul(colon+1);
                }
            }
        }
        while (body.size() < content_length) {
            body += server::receiveChunk(bio);
        }
        return headers + "\r\n" + body;
    }


    /**
     * Sends data across a BIO
     * Adapted From: https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
     *
     * @param bio --> BIO to write data to
     * @param resp --> Data to write, usually an HTTP request
     */
    void sendTo (BIO *bio, const std::string& resp) {
        BIO_write(bio, resp.data(), resp.size());
        BIO_flush(bio);

    }

    /**
     * This function handles the weirdness that is openssl's accept BIOs.
     * When a new connection is received it adds the new BIO to the chain
     * where it was accepted. To handle the new connection, we have to pop
     * the new BIO off the chain.
     * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
     *
     * @param listenBIO --> The BIO where you are doing the listening
     * @return --> returns the new connection BIO or nullptr if failed
     */
    server::UniquePtr<BIO> new_connection(BIO *listenBIO) {
        if (BIO_do_accept(listenBIO) <= 0) {
            return nullptr;
        }
        return server::UniquePtr<BIO>(BIO_pop(listenBIO));
    }

    std::string makeResponse(const std::string& code, const std::string& codeMsg, const std::string& content) {
        // TODO: This function call should create a response for the server to send back to the client
        //       using the information provided in the params
        // HTTP/1.1 CODE CODE_MSG
        std::string resp = fmt::format("HTTP/1.1 {} {}\r\n",  code, codeMsg);
        resp += fmt::format("Content-Length: {}\r\n", content.length());
        resp += fmt::format("\r\n\r\n");
        resp += content;
    }

    std::vector<std::string> parseResource(std::string reqTarget, const bool& absolute) {

        std::string params = "";

        if (absolute) {
            // TODO: Handle parsing resource from absolute req-target
        }

        // Default resource request
        if (reqTarget == "/") {
            return {"/index.html", params};
        }

        // Return path to target if no params or fragment found
        if (reqTarget.find('?') == std::string::npos &&
            reqTarget.find('#') == std::string::npos) {
            return {reqTarget, params};
        }

        int loc = -1;
        // Check for and strip fragment
        if ((loc = reqTarget.find('#')) != std::string::npos) {
            reqTarget.erase(loc);
        }

        // Check for params and save params if exist
        if ((loc = reqTarget.find('?')) != std::string::npos) {
            // TODO: Parse params for saving
            reqTarget.erase(loc);
        }

        // If we make it here all that should remain is just the path to resource
        return {reqTarget, params};


    }
}