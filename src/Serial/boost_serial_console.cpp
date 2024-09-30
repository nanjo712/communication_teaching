#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::string port_name = "/dev/ttyUSB1";
    // "BUPT-RobotTeam-2023211735"
    std::string my_info = argv[2];

    boost::asio::io_context io_context;
    boost::asio::serial_port serial_port(io_context, port_name);

    std::string command;
    std::array<char, 1024> buffer;

    /* TODO: decrypt core dumped */
    if (memcmp(argv[1], "encrypt", 7) == 0 ||
        memcmp(argv[1], "decrypt", 7) == 0 || memcmp(argv[1], "help", 4) == 0)
    {
        command = argv[1];
    }
    else
    {
        std::cerr << "Wrong command usgae" << std::endl;
        return 1;
    }

    /* backend parse the info to be encrypted or decryped started from the index
    8, which means you should add a space or any other character to occupy this
    pos.
     */
    serial_port.write_some(boost::asio::buffer(command + " " + my_info));
    auto read_size = serial_port.read_some(boost::asio::buffer(buffer));
    std::string response = std::string(buffer.data(), read_size);
    std::cout << response << std::endl;

    return 0;
}