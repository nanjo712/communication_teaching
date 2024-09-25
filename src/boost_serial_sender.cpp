#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2 || argv[1] == NULL)
    {
        std::cout << "Please give a seria name\n";
    }
    std::string port_name(argv[1]);

    boost::asio::io_context io_context;
    boost::asio::serial_port serial(io_context, port_name);

    serial.set_option(boost::asio::serial_port::baud_rate(115200));
    serial.set_option(boost::asio::serial_port::character_size(8));
    serial.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    serial.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    serial.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    
}