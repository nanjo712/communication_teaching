#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <iostream>

int main()
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve("localhost", "12345"));

    std::string message;
    while (true)
    {
        std::cout << "Send > ";
        std::getline(std::cin, message);
        socket.write_some(boost::asio::buffer(message));
        std::string response;
        response.resize(1024);
        socket.read_some(boost::asio::buffer(response));
        std::cout << "Response: " << response << std::endl;
    }

    return 0;
}