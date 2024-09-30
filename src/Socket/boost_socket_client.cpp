#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve("localhost", "12345"));

    std::string command = argv[1];
    std::string request;

    if (command == "echo")
    {
        std::string message = argv[2];
        request += " " + message;
    }
    socket.write_some(boost::asio::buffer(request));
    std::string response;
    response.resize(1024);
    if (command != "exit")
        socket.read_some(boost::asio::buffer(response));
    else
        response = "Session closed, client closed...";
    std::cout << "Response: " << response << std::endl;

    return 0;
}