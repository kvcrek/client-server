#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define IP "127.0.0.1"

int main() {
    struct sockaddr_in server = {
            .sin_family = AF_INET,
            .sin_port = htons(PORT)
    };
    if (inet_pton(AF_INET, IP, &server.sin_addr) <= 0) {
        perror("inet_pton() ERROR");
        exit(1);
    }
    const int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket() ERROR");
        exit(2);
    }
    char buffer[4096]{};

    while (true) {
        std::cout << "Enter a number: " ;
        std::cin >> buffer;
        std::cin.get();


        socklen_t len = sizeof(server);
        if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *) &server, len) < 0) {
            perror("sendto() ERROR ");
            exit(3);
        }

        struct sockaddr_in from{};
        memset(buffer, 0, sizeof(buffer));

        if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &from, &len) < 0) {
            perror("recvfrom() ERROR");
            exit(4);
        }
        std::cout << "Reply: " << buffer << std::endl;
    }
}
