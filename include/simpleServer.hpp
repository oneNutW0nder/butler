#ifndef BUTLER_CLIENT_SIMPLESERVER_HPP
#define BUTLER_CLIENT_SIMPLESERVER_HPP

#include <memory>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

/**
 * "server" namespace will contain methods that will assist in networking
 * functionality leveraging openssl's BIOs. Special thanks to Arthur O'Dwyer
 * for the wonderful tutorial on using openssl with modern C++.
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
    void init_ssl(){
        SSL_library_init();
        SSL_load_error_strings();
    }

    void ssl_errors(const char *str) {
        std::cerr << str << std::endl;
        ERR_print_errors_fp(stderr);
        exit(10);
    }

    /**
     * This function handles the weirdness that is openssl's accept BIOs.
     * When a new connection is received it adds the new BIO to the chain
     * where it was accepted. To handle the new connection, we have to pop
     * the new BIO off the chain.
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

    /**
     * Server class will contain member variables and functions that are used
     * to facilitate the operation of an HTTP server. Again, special thanks to
     * Arthur O'Dwyer for the tutorial on how to use C++ to wrap C APIs.
     */
    class Server{
    private:
        std::string m_str;
        server::UniquePtr<BIO_METHOD> m_bio_methods;
        server::UniquePtr<BIO> m_bio;
    public:
        BIO *bio() { return m_bio.get(); }
        std::string str() && { return std::move(m_str); }

        Server(Server&&) = delete;
        Server& operator=(Server&&) = delete;

        explicit Server(){
            m_bio_methods.reset(BIO_meth_new(BIO_TYPE_SOURCE_SINK, "ServerBIO"));
            if (m_bio_methods == nullptr){
                throw std::runtime_error("FATAL: Failed at BIO_meth_new()... exiting");
            }

            BIO_meth_set_write(m_bio_methods.get(), [](BIO *bio, const char *data, int len) -> int {
                auto *str = reinterpret_cast<std::string*>(BIO_get_data(bio));
                str->append(data, len);
                return len;
            });

            m_bio.reset(BIO_new(m_bio_methods.get()));
            if (m_bio == nullptr){
                throw std::runtime_error("FATAL: Failed at BIO_new()... exiting");
            }

            BIO_set_data(m_bio.get(), &m_str);
            BIO_set_init(m_bio.get(), 1);
        }
    };
}

#endif //BUTLER_CLIENT_SIMPLESERVER_HPP
