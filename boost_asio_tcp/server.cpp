#include <iostream>
#include <boost/asio.hpp>
#include <thread>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_BUFFER_LEN 4096

using namespace boost;

void handleConnection(asio::ip::tcp::socket sock) {
    try {
        std::cout << "New connection from: " << sock.remote_endpoint().address().to_string() << std::endl;
        while (true) {
            char buf[MAX_BUFFER_LEN]{};
            system::error_code err;

            sock.read_some(asio::buffer(buf), err);
            if (err == asio::error::eof) {
                std::cout << sock.remote_endpoint().address().to_string() << " has disconnected." << std::endl;
                return;
            } else if (err) {
                throw system::system_error(err);
            }
            std::cout << "Message from client : " << buf << std::endl;
            asio::write(sock, asio::buffer("Hello from server!"));
        }
    }
    catch (std::exception &e) {
        std::cerr << "Exception in thread : " << e.what() << std::endl;
    }
};

int main() {
    asio::io_context ioContext;

    asio::ip::tcp::acceptor acc(ioContext,
                                asio::ip::tcp::endpoint(asio::ip::address::from_string(SERVER_IP), SERVER_PORT));
    while (true) {
        std::thread(handleConnection, acc.accept()).detach();
    }
    return 0;
}