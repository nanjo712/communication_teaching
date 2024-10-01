#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/write.hpp>
#include <string>
#include <iostream>
#include <thread>
int main(int argc, char* argv[])
{
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

    if (!serial_port.is_open()) return 1;

    std::cout<<"Serial port opened successfully!"<<std::endl<<"----------------------"<<std::endl;
    std::cout<<"Enter 'exit' to quit the program."<<std::endl; 

    std::string data_to_send;
    while (1)
    {
        char read_buf[1024];
        std::cout<<">>";
        std::getline(std::cin, data_to_send);
        if (data_to_send == "exit") break;
        boost::asio::write(serial_port, boost::asio::buffer(data_to_send));

        std::size_t len = serial_port.read_some(boost::asio::buffer(read_buf));
        std::cout<<"Received: "<<std::string(read_buf, len)<<std::endl;
    }
    return 0;
}