#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <array>
#include <sstream>
#include <csignal>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"

#define MAX_BUFFER_LEN 4096
#define MAX_CONNECTIONS 5

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

    struct sockaddr_in server = {
            .sin_family = AF_INET,
            .sin_port = htons(SERVER_PORT)
    };

    if (inet_pton(AF_INET, SERVER_IP, &server.sin_addr) <= 0) {
        std::cerr << "inet_pton() failed" << std::endl;
        exit(1);
    }

    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "socket() failed" << std::endl;
        exit(2);
    }

    socklen_t len = sizeof(server);
    if (bind(server_fd, (struct sockaddr *) &server, len) < 0) {
        std::cerr << "bind() failed" << std::endl;
        exit(3);
    }

    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        std::cerr << "listen() failed" << std::endl;
        exit(4);
    }

    while (true) {
        std::cout << "Waiting for connection..." << std::endl;

        struct sockaddr_in client{};
        const int clientSocket = accept(server_fd, (struct sockaddr *) &client, &len);

        if (clientSocket < 0) {
            std::cerr << "accept() failed" << std::endl;
            exit(5);
        }

        if (recv(clientSocket, buffer.data(), sizeof(buffer), 0) <= 0) {
            std::cerr << "recv() failed" << std::endl;
            exit(6);
        }

        message = convertByteArrayToString(buffer);
        std::cout << "Message from client: " << message << std::endl;

        message = "Hello from server!";
        convertStringToByteArray(message, buffer);

        if (send(clientSocket, &buffer, sizeof(buffer), 0) <= 0) {
            std::cerr << "send() failed" << std::endl;
            exit(7);
        }
        buffer.fill(std::byte(0));
        message = "";
        close(clientSocket);
    }
    shutdown(server_fd, SHUT_RDWR);
}