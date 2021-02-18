#ifndef BUTLER_CLIENT_SIMPLESERVER_HPP
#define BUTLER_CLIENT_SIMPLESERVER_HPP

#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <exception>
#include <iostream>

#include <cstring>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

/**
 * "server" namespace will contain methods that will assist in networking
 * functionality leveraging openssl's BIOs. Special thanks to Arthur O'Dwyer
 * for the wonderful tutorial on using openssl with modern C++.
 * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
 */
namespace server {

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

    // Load openssl
    void init_ssl();

    // Used for fatal errors with SSL
    // https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
    void ssl_errors(const char *str);

    /**
     * Reads a chunk of 1024 bytes from a BIO. Throws
     * std::runtime_error if a problem occurs
     * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
     *
     * @param bio --> BIO to read from
     * @return --> Data read from the BIO
     */
    std::string receiveChunk(BIO *bio);

    /**
     * Splits request headers. Used for searching for content-length
     * when reading from the BIO
     * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
     *
     * @param text --> Request headers for splitting
     * @return --> vector of split headers
     */
    std::vector<std::string> split_headers(const std::string& text);

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
    std::string receive_http_message(BIO *bio);

    /**
     * Sends data across a BIO
     * Adapted From: https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
     *
     * @param bio --> BIO to write data to
     * @param resp --> Data to write, usually an HTTP request
     */
    void sendTo (BIO *bio, const std::string& resp);

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
    server::UniquePtr<BIO> new_connection(BIO *listenBIO);

    /**
     * Server class will contain member variables and functions that are used
     * to facilitate the operation of an HTTP server. Again, special thanks to
     * Arthur O'Dwyer for the tutorial on how to use C++ to wrap C APIs.
     * https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/
     */
     // TODO: Not sure if this is going to be used anywhere
//    class Server{
//    private:
//        std::string m_str;
//        server::UniquePtr<BIO_METHOD> m_bio_methods;
//        server::UniquePtr<BIO> m_bio;
//    public:
//        BIO *bio() { return m_bio.get(); }
//        std::string str() && { return std::move(m_str); }
//
//        Server(Server&&) = delete;
//        Server& operator=(Server&&) = delete;
//
//        explicit Server(){
//            m_bio_methods.reset(BIO_meth_new(BIO_TYPE_SOURCE_SINK, "ServerBIO"));
//            if (m_bio_methods == nullptr){
//                throw std::runtime_error("FATAL: Failed at BIO_meth_new()... exiting");
//            }
//
//            BIO_meth_set_write(m_bio_methods.get(), [](BIO *bio, const char *data, int len) -> int {
//                auto *str = reinterpret_cast<std::string*>(BIO_get_data(bio));
//                str->append(data, len);
//                return len;
//            });
//
//            m_bio.reset(BIO_new(m_bio_methods.get()));
//            if (m_bio == nullptr){
//                throw std::runtime_error("FATAL: Failed at BIO_new()... exiting");
//            }
//
//            BIO_set_data(m_bio.get(), &m_str);
//            BIO_set_init(m_bio.get(), 1);
//        }
//    };


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
