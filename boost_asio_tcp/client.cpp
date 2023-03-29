#include <iostream>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_BUFFER_LEN 4096

using namespace boost;

int main(int argc, char *argv[]) {
    try {
        std::string ip = SERVER_IP;
        int port = SERVER_PORT;
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

        char buf[MAX_BUFFER_LEN];
        system::error_code err;

        asio::io_context ioContext;

        asio::ip::tcp::socket sock(ioContext);
        asio::ip::tcp::endpoint serverEndpoint(asio::ip::address::from_string(ip), port);
        sock.connect(serverEndpoint);

        std::cout << "Connected to: " << sock.remote_endpoint().address().to_string() << std::endl;
        asio::write(sock, asio::buffer("Hello from client!"));
        sock.read_some(asio::buffer(buf));
        std::cout << "Message from server : " << buf << std::endl;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
