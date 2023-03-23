#include <iostream>
#include <boost/asio.hpp>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_BUFFER_LEN 4096

using namespace boost;

int main() {
    char buf[MAX_BUFFER_LEN];
    system::error_code err;

    asio::io_context ioContext;

    asio::ip::tcp::socket sock(ioContext);
    asio::ip::tcp::endpoint serverEndpoint(asio::ip::address::from_string(SERVER_IP), SERVER_PORT);
    sock.connect(serverEndpoint);

    std::cout << "Connected to: " << sock.remote_endpoint().address().to_string() << std::endl;
    asio::write(sock, asio::buffer("Hello from client!"));
    sock.read_some(asio::buffer(buf));
    std::cout << "Message from server : " << buf << std::endl;

    return 0;
}
