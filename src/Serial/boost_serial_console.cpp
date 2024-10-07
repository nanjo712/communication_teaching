#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <string>
#include <iostream>
#include <thread>

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

    //实现了如下功能：
    //接收用户输入指令，发送至backend
    //接收backend的回复，打印到控制台
    //用户输入exit时，退出程序

    // std::string str = "help";
    // std::string str = "encrypt BUPT-RobotTe-2023212641";
    // std::string str = "decrypt JqE4hHAU2miDNoPqks9N0WviV0w1TduFq8fCxo/OFKr629TWTE5Ezzar2hDBrdc3JNCO3nVzBXtG11O0O/bSyQThVgQo3LKZB69KW2rRWoJYweT2ewoWgjzVhar/9Jfv3Il3BJciOuAiw3IKhT7SHsoaAEPGWZBxdGnO3IgfFK8i97w97CC7IJ7MNDMkDmfEJfYWXVPT/1jf7+jQSUQqqyGhYAqp6ydESrnfdJYvp7BxEPxhEyEKkSZbLqvdnu1DtnobI0h7+JgAFBcw9zu8fUqZeJpQvJx5F0K6gFo8jYT1wRHnA+0rPfhbZQgawhUKs9NcdNwehNEEvoEsxmKZEg==";

    std::array<char, 10240> buffer;
    boost::system::error_code ec;
    std::string user_input;

    while (true)
    {
        std::cout << "Enter command: ";
        std::getline(std::cin, user_input);

        if (user_input == "exit")
        {
            break;
        }

        serial.write_some(boost::asio::buffer(user_input), ec);
        if (ec)
        {
            std::cerr << "Error when writing to " << port_name << std::endl;
        }
        else
        {
            std::cout << "Writing " << user_input << " to " << port_name
                      << std::endl;
        }
        buffer.fill(0);
        auto size = serial.read_some(boost::asio::buffer(buffer), ec);
        if (ec)
        {
            std::cerr << "Error when reading from " << port_name << std::endl;
        }
        else
        {
            std::string response(buffer.data(), size);
            std::cout << "Received: " << response << std::endl;
        }
    }
}