#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>

// struct sockaddr_in casts to sockaddr struct later
// sockaddr struct is straight cancer so we do it this way

// Use inet_pton() to set sockaddr_in.sin_addr value
// This is done to convert IP string to IP in bytes inside the struct

int main(){

    int port = 80;
    int cutoff = 1024;
    std::string scheme = "http://";
    std::string domain = "127.0.0.1";
    // std::string domain = "93.184.216.34";

    std::string req = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: Close\r\n\r\n";

    struct sockaddr_in myaddr;

    std::vector<uint8_t> data;
    char buffer[1030];

    // This needs to be done for compatibility purposes between
    // sockaddr_in and sockaddr
    memset(&myaddr, 0, sizeof(myaddr));

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(port);

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        puts("Failed to create socket... Exiting");
        return -1;
    }

    // TODO: Get IP via DNS

    // Convert IP from string to binary and place in sockadd_in struct field
    if(inet_pton(AF_INET, domain.c_str(), &myaddr.sin_addr) <= 0) {
        puts("Failed to convert address");
        return -1;
    }

    // Try connecting
    if(connect(sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0){
        puts("Failed to connect");
        return -1;
    }

    send(sockfd, req.c_str(), req.length(), 0);
    // read(sockfd, buff, 10);

    // Read response
    while(cutoff > 0 ){
        cutoff = recv(sockfd, buffer, 1024, 0);
        std::cout << "Packet length recieved: " << cutoff << std::endl;
        data.insert(data.end(), buffer, buffer+cutoff);
    }

    for(auto i: data){
        std::cout << i;
    }


    
    return 0;
}