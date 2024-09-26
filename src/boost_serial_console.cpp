#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2 || argv[1] == NULL)
    {
        std::cout << "Please give a seria name\n";
        assert(0);
    }
    std::string port_name(argv[1]);

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
    std::string feedback;
    feedback.resize(1024);
    while (1)
    {
        std::getline(std::cin, command);
        serial.write_some(boost::asio::buffer(command), ec);
        if (ec)
        {
            std::cerr << "Error when writing to " << port_name << std::endl;
        }

        auto size = serial.read_some(boost::asio::buffer(feedback), ec);
        feedback.resize(size);
        if (ec)
        {
            std::cerr << "Error when reading to " << port_name << std::endl;
        }
        std::cout << "Feedback: \n" << feedback << std::endl;
    }
}