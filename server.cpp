#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <array>
#include <stdexcept>
#include <charconv>

#define PORT 8080
#define IP "127.0.0.1"

bool charArrayToInt(std::array<char, 4096> buf, int &num) {
    std::string str(buf.data());
    auto res = std::from_chars(str.data(), str.data() + str.size(), num);
    if (res.ec == std::errc()){
        return true;
    } else {
        return false;
    }
}

bool isEven(int n) {
    return !(n % 2);
}

int main() {
    struct sockaddr_in server = {
            .sin_family = AF_INET,
            .sin_port = htons(PORT)
    };
    if (inet_pton(AF_INET, IP, &server.sin_addr) <= 0) {
        throw std::runtime_error("inet_pton() ERROR");
    }

    const int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if ((sock) < 0) {
        throw std::runtime_error("socket() ERROR");
    }
    std::array<char, 4096> buffer{};

    socklen_t len = sizeof(server);
    if (bind(sock, (struct sockaddr *) &server, len) < 0) {
        throw std::runtime_error("bind() ERROR");
    }

    while (true) {
        struct sockaddr_in client{};
        buffer.fill(0);
        std::cout << "Waiting for connection..." << std::endl;
        if (recvfrom(sock, buffer.data(), buffer.size(), 0, (struct sockaddr *) &client, &len) < 0) {
            throw std::runtime_error("recvfrom() ERROR");
        }

        char buffer_ip[128]{};
        inet_ntop(AF_INET, &client.sin_addr, buffer_ip, sizeof(buffer_ip));
        std::cout << "Message from client: " << buffer.data() << std::endl;
        std::cout << "Client's ip: " << buffer_ip << std::endl;

        int num;
        if (charArrayToInt(buffer, num)) {
            bool ans = isEven(num);
            if (ans) {
                strncpy(buffer.data(), "Even", buffer.size());
            } else {
                strncpy(buffer.data(), "Odd", buffer.size());
            }
            if (sendto(sock, buffer.data(), std::strlen(buffer.data()), 0, (struct sockaddr *) &client, len) < 0) {
                throw std::runtime_error("sendto() ERROR");
            }
        } else {
            strncpy(buffer.data(), "Invalid Argument", buffer.size());
            if (sendto(sock, buffer.data(), std::strlen(buffer.data()), 0, (struct sockaddr *) &client, len) < 0) {
                throw std::runtime_error("sendto() ERROR");
            }
        }
    }
}
