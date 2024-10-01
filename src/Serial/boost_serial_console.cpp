#include <boost/asio/io_context.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <string>
#include <iostream>
#include <thread>
using namespace std;
using namespace boost::asio;

void send_command(boost::asio::serial_port &serial_port, const std::string &command) {
    boost::asio::write(serial_port, boost::asio::buffer(command));
}
std::string read_response(boost::asio::serial_port &serial_port) 
{
    std::array<char, 1024> buffer;
    size_t size = serial_port.read_some(boost::asio::buffer(buffer));
    return std::string(buffer.data(), size);
}

int main(int argc, char* argv[])
{
    cout <<"success"<<endl;
    std::string port_name = "/dev/ttyUSB1";

    boost::asio::io_context io_context;
    boost::asio::serial_port serial_port(io_context, port_name);

    serial_port.set_option(boost::asio::serial_port::baud_rate(115200));
    serial_port.set_option(boost::asio::serial_port::character_size(8));
    serial_port.set_option(boost::asio::serial_port::stop_bits(
        boost::asio::serial_port::stop_bits::one));
    serial_port.set_option(boost::asio::serial_port::parity(
        boost::asio::serial_port::parity::none));
    serial_port.set_option(boost::asio::serial_port::flow_control(
        boost::asio::serial_port::flow_control::none));
    if (!serial_port.is_open()) 
    {
        std::cerr << "Failed to open serial port." << std::endl;
        return 1;
    }
    std::string command;
    while (1) {
        std::cout << "> ";
        std::getline(std::cin, command);
        send_command(serial_port, command);
        std::string response = read_response(serial_port);
        std::cout << "Response: " << response << std::endl;
    }

    return 0;
}
