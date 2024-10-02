#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
    std::string port_name = "/dev/ttyUSB1";

    boost::asio::io_context io_context;
    boost::asio::serial_port serial(io_context, port_name);

    serial.set_option(boost::asio::serial_port::baud_rate(115200));
    serial.set_option(boost::asio::serial_port::character_size(8));
    serial.set_option(boost::asio::serial_port::stop_bits(
        boost::asio::serial_port::stop_bits::one));
    serial.set_option(boost::asio::serial_port::parity(
        boost::asio::serial_port::parity::none));
    serial.set_option(boost::asio::serial_port::flow_control(
        boost::asio::serial_port::flow_control::none));

    if (!serial.is_open()) return 1;

    while (1) {
        std::string message = "";
        std::cout << "\033[32mSend > \033[0m";
        std::getline(std::cin, message);
        // std::cout << std::endl;

        boost::system::error_code ec;

        serial.write_some(boost::asio::buffer(message), ec);
        if (ec) {
            std::cerr << "\033[34mError when writing to " << port_name << "\033[0m" << std::endl;
        }
        else {
            std::cout << "\033[34mWriting '" << message << "' to " << port_name
                      << "\033[0m" << std::endl;
        }

        std::array<char, 1024> responseBuffer;
        while (1) {
            auto size = serial.read_some(boost::asio::buffer(responseBuffer));
            std::string response(responseBuffer.data(), size);
            std::cout << "\033[32mResponse < \033[0m" << response << std::endl;
            break;
        }
    }
}