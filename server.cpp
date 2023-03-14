#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define IP "127.0.0.1"

bool charArrayToInt(char buf[], int &num) {
    std::string str(buf);
    int parsed;
    try {
        parsed = std::stoi(str);
    }
    catch (...) {
        return false;
    }
    num = parsed;
    return true;
}

bool isEven(int n) {
    if (n % 2 == 0) {
        return true;
    } else {
        return false;
    }
}

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
    if ((sock) < 0) {
        perror("socket() ERROR");
        exit(2);
    }
    char buffer[4096]{};

    socklen_t len = sizeof(server);
    if (bind(sock, (struct sockaddr *) &server, len) < 0) {
        perror("bind() ERROR");
        exit(3);
    }

    while (true) {
        struct sockaddr_in client{};
        memset(buffer, 0, sizeof(buffer));
        std::cout << "Waiting for connection..." << std::endl;
        if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &len) < 0) {
            perror("recvfrom() ERROR");
            exit(4);
        }

        char buffer_ip[128]{};
        inet_ntop(AF_INET, &client.sin_addr, buffer_ip, sizeof(buffer_ip));
        std::cout << "Message from client: " << buffer << std::endl;
        std::cout << "Client's ip: " << buffer_ip << std::endl;

        int num;
        if (charArrayToInt(buffer, num)) {
            bool ans = isEven(num);
            if (ans) {
                strncpy(buffer, "Even", sizeof(buffer));
            } else {
                strncpy(buffer, "Odd", sizeof(buffer));
            }
            if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *) &client, len) < 0) {
                perror("sendto() ERROR");
                exit(5);
            }
        } else {
            strncpy(buffer, "Invalid Argument", sizeof(buffer));
            if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *) &client, len) < 0) {
                perror("sendto() ERROR");
                exit(5);
            }
        }
    }
}