#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <iostream>

int main()
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve("localhost", "12345"));
    
    while (true) {
        std::string message;
        std::cout << "Enter message: ";
        std::getline(std::cin, message);
        if (message.empty()) break; // Exit loop if input is empty

        socket.write_some(boost::asio::buffer(message));
        std::string response;
        response.resize(1024);
        size_t len = socket.read_some(boost::asio::buffer(response));
        std::cout << "Response: " << response.substr(0, len) << std::endl;
    }

    return 0;
}