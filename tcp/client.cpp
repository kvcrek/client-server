#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <array>
#include <csignal>

#define MAX_BUFFER_LEN 4096
#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"

std::string convertByteArrayToString(std::array<std::byte, MAX_BUFFER_LEN> buffer) {
    std::stringstream ss;
    for (auto &byte: buffer) {
        if (byte != std::byte(0)) {
            ss << static_cast<char>(byte);
        }
    }
    return ss.str();
}

std::array<std::byte, MAX_BUFFER_LEN>
convertStringToByteArray(std::string str, std::array<std::byte, MAX_BUFFER_LEN> &buffer) {
    for (auto i = 0; i < str.size(); ++i) {
        buffer[i] = static_cast<std::byte>(str[i]);
    }
    return buffer;
}

int main() {
    std::array<std::byte, MAX_BUFFER_LEN> buffer{};
    std::string message;

    const int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        std::cerr << "Socket creation error" << std::endl;
        exit(1);
    }

    struct sockaddr_in server = {
            .sin_family = AF_INET,
            .sin_port = htons(SERVER_PORT)
    };

    if (inet_pton(AF_INET, SERVER_IP, &server.sin_addr) <= 0) {
        std::cerr << "inet_pton() failed" << std::endl;
        exit(2);
    }

    if (connect(client_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        std::cerr << "connect() failed" << std::endl;
        exit(3);
    }

    message = "Hello from client!";
    convertStringToByteArray(message, buffer);

    if (send(client_fd, &buffer, sizeof(buffer), 0) < 0) {
        std::cerr << "send() failed" << std::endl;
        exit(4);
    }

    buffer.fill(std::byte(0));
    message = "";

    if (recv(client_fd, &buffer, sizeof(buffer), 0) < 0) {
        std::cerr << "recv() failed" << std::endl;
        exit(5);
    }

    message = convertByteArrayToString(buffer);

    std::cout << "Message from server: " << message;

    close(client_fd);
}