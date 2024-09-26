#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <string>

int main(int argc, char* argv[])
{
    std::string port_name = "/dev/ttyUSB1";

    boost::asio::io_context io_context;
    boost::asio::serial_port serial_port(io_context, port_name);
}