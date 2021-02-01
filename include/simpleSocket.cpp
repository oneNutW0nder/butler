#include "simpleSocket.hpp"
#include <string.h>

socket::Socket::Socket(){
    this->m_ctx = nullptr;
    this->m_conn_bio = nullptr;
    this->m_ssl_bio = nullptr;
    this->tls = false;

    memset(this->m_resp_buffer, 0, sizeof(this->m_resp_buffer));
}

void socket::Socket::readFrom(){

}

void socket::Socket::sendTo(){

}

void socket::Socket::setTLS(bool tls){
    this->tls = tls;
}

bool socket::Socket::getTLS(){
    return this->tls;
}