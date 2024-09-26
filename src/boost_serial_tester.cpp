#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <iostream>
#include <thread>
#include <iostream>
#include <cstring>



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

    if (!serial.is_open()) return 1;

    std::array<char, 1> cnt;
    boost::system::error_code ec;
    while (1)
    {
        serial.write_some(boost::asio::buffer(cnt), ec);
        if (ec)
        {
            std::cerr << "Error when writing to " << port_name << std::endl;
        }
        else 
        {
            std::cout << "Writing " << (int)cnt[0] << " to " << port_name << std::endl;     
        }
        cnt[0] ++;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }    

}