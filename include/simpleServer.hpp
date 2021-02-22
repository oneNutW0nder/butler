#ifndef BUTLER_CLIENT_SIMPLESERVER_HPP
#define BUTLER_CLIENT_SIMPLESERVER_HPP

#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <exception>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <map>
#include <mutex>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

namespace server {

    const std::string SERVER_ROOT = "butler-server";        // Folder name used as root dir
    const std::string DEFAULT_SERVER_NAME  = "localhost";   // Name of the server for HOST checking

    std::mutex mut;

    // Special deleter functions used alongside smart pointers
    template<typename T> struct DeleterOf;
    template<> struct DeleterOf<SSL_CTX> { void operator() (SSL_CTX *p) const { SSL_CTX_free(p); } };
    template<> struct DeleterOf<SSL> { void operator() (SSL *p) const { SSL_free(p); } };
    template<> struct DeleterOf<BIO> { void operator() (BIO *p) const { BIO_free_all(p); } };
    template<> struct DeleterOf<BIO_METHOD> { void operator() (BIO_METHOD *p) const { BIO_meth_free(p); } };

    template<typename OpenSSLType>
    using UniquePtr = std::unique_ptr<OpenSSLType, server::DeleterOf<OpenSSLType>>;

    // Overload "|" operator for better readability when making BIO chains
    server::UniquePtr<BIO> operator|(server::UniquePtr<BIO> lower, server::UniquePtr<BIO> upper){
        BIO_push(upper.get(), lower.release());
        return upper;
    }

    // Struct to hold request information which needs to be passed around to fulfill the request
    struct requestInfo {
        std::string method;
        std::string resource;
        std::map<std::string, std::string> params; // Params from request-target
        std::string body;                          // Body will contain POST params for POST
        std::string serverRoot;
    };

    void init_ssl();
    void ssl_errors(const char *str);
    void sendTo (BIO *bio, const std::string& resp);
    void requestHandler(UniquePtr<BIO> bio, std::string serverRoot, const bool& https);

    std::filesystem::path init_server();
    std::pair<std::string, std::string> parseResource(std::string reqTarget, const bool& absolute);
    std::string receiveChunk(BIO *bio, const bool& https);
    std::string receive_http_message(BIO *bio, const bool& https);
    std::vector<std::string> split_headers(const std::string& text);
    server::UniquePtr<BIO> new_connection(BIO *listenBIO);

    std::string makeResponse(const std::string& code, const std::string& codeMsg,
                             const std::string& content, const std::map<std::string, std::string>& otherHeaders);

    // TODO: Support request params being sent around
    std::string serveRequest(struct requestInfo* requestInfo);

    /**
     * Custom exception type. Used to throw exceptions with information used to
     * set the proper response code and response message for whatever error occured.
     */
    class httpException : public std::exception{
    private:
        int m_status_code;
        const char* m_code_msg;
        const std::string m_err_msg;
    public:
        httpException(std::string err_msg, int status_code, const char* code_msg) :
                m_err_msg (std::move(err_msg)),
                m_status_code (status_code),
                m_code_msg (code_msg)
        {}


        [[nodiscard]] const char* what() const noexcept override {
            return this->m_err_msg.c_str();
        }

        [[nodiscard]] const std::string& GetMErrMsg() const { return this->m_err_msg; }
        [[nodiscard]] int GetMStatusCode() const { return this->m_status_code; }
        [[nodiscard]] const char* GetMCodeMsg() const { return this->m_code_msg; }

    };
}

#endif //BUTLER_CLIENT_SIMPLESERVER_HPP
