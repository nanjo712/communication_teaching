#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
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

    std::array<char, 11> cnt ={"2023212113"};
    
    boost::system::error_code ec;
    while (1)
    {
        serial_port.write_some(boost::asio::buffer(cnt), ec);
        if (ec)
        {
            std::cerr << "Error when writing to " << port_name << std::endl;
        }
        else
        {
            std::cout << "Writing " << (int)cnt[0] << " to " << port_name
                      << std::endl;
        }
        cnt[0]++;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
}