#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>

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

    boost::system::error_code ec;
    std::string command;
    while (1)
    {
        /* Modify Code here */
    }
}