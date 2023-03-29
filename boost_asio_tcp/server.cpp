
#include <iostream>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_BUFFER_LEN 4096

using boost::asio::ip::tcp;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    explicit Connection(tcp::socket socket)
            : sock(std::move(socket)),
              buf{} {
        std::cout << "New connection from: " << sock.remote_endpoint().address().to_string() << std::endl;
    }

    void start() {
        do_read();
    }

    void do_read() {
        auto self(shared_from_this());
        sock.async_read_some(
                boost::asio::buffer(buf),
                [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec) {
                        do_write();
                    }
                    std::cout << "Message from client: " << buf.data() << std::endl;
                });
    }

    void do_write() {
        auto self(shared_from_this());
        boost::asio::async_write(sock,
                                 boost::asio::buffer("Hello from server!"),
                                 [this, self](boost::system::error_code err, std::size_t len) -> void {
                                     if (!err) {
                                         sock.shutdown(tcp::socket::shutdown_send);
                                         std::cout << sock.remote_endpoint().address().to_string()
                                                   << " has disconnected." << std::endl;
                                     }
                                 });
    }

private:
    tcp::socket sock;
    std::array<char, MAX_BUFFER_LEN> buf;
};

class Server {
public:
    explicit Server(boost::asio::io_service &io_service, const std::string &ip = SERVER_IP, int port = SERVER_PORT)
            : acceptor_(io_service, tcp::endpoint(boost::asio::ip::address::from_string(ip), port)) {
        do_accept();
    }

    void do_accept() {
        acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<Connection>(std::move(socket))->start();
                    }
                    do_accept();
                });
    }

private:
    tcp::acceptor acceptor_;
};

int main(int argc, char *argv[]) {
    try {
        std::string ip;
        int port;
        boost::program_options::options_description desc("Options");
        desc.add_options()
                ("help,h", "This help message")
                ("ip,i", boost::program_options::value<std::string>(&ip)->default_value(SERVER_IP), "Server ip address")
                ("port,p", boost::program_options::value<int>(&port)->default_value(SERVER_PORT), "Server port");

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        boost::asio::io_service ios;
        Server server(ios, ip, port);
        while (true) {
            try {
                ios.run();
                break;
            } catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}