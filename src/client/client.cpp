#include <iostream>
#include <string>
#include <vector>

#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>


// struct sockaddr_in casts to sockaddr struct
// sockaddr struct is straight cancer so we do it this way

int main(){

    int status, sockfd;
    struct addrinfo hints, *servinfo, *p;
    char buffer[1024];

    std::string req = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: Close\r\n\r\n";
    std::vector<uint8_t> data;

    // Make sure struct is empty
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Get server information from hostname
    if(status = getaddrinfo("localhost", "http", &hints, &servinfo) != 0) {
        fprintf(stderr, "getaddr info: %s\n", gai_strerror(status));
        return -1;
    }

    // Loop through linked list from getaddrinfo() and try connecting
    for(p = servinfo; p != NULL; p = p->ai_next){
        // Check to see if we can make a socket
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            std::cerr << "socket error... continuing" << std::endl;
            continue;
        }

        // Check to see if we can connect using that socket
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            std::cerr << "connection error... continuing" << std::endl;
            continue;
        }

        // break if we create a valid socket and successfully connect
        // Alternatively, we reached the end of the linked list and failed completely
        break;

    }

    // Check for the end of the linked list
    if(p == NULL){
        std::cerr << "Failed to connect to host" << std::endl;
    }

    // If we reach here we are connected via sockfd
     send(sockfd, req.c_str(), req.length(), 0);
    // read(sockfd, buff, 10);

    // Read response
    int cutoff = 1024;
    while(cutoff > 0 ){
        cutoff = recv(sockfd, buffer, 1024, 0);
        std::cout << "Packet length recieved: " << cutoff << std::endl;
        data.insert(data.end(), buffer, buffer+cutoff);
    }

    for(auto i: data){
        std::cout << i;
    }


    std::cout << "Testing completed" << std::endl;

    return 0;
}