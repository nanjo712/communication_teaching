#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <iostream>
#include <string>
#include <thread>

class SerialConsole
{
   public:
    SerialConsole(boost::asio::io_context& ioc, std::string port_name)
    {
        m_serial_port = std::make_shared<boost::asio::serial_port>(
            boost::asio::serial_port(ioc, port_name));
    }

    ~SerialConsole() { m_serial_port->close(); }

    void start()
    {
        while (true)
        {
            std::cout << "$ ";
            std::cin >> m_command;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                break;
            }
            if (m_command == "exit")
            {
                std::cout << "Exit..." << std::endl;
                break;
            }
            if (m_command != "encrypt" && m_command != "decrypt" &&
                m_command != "help")
            {
                std::cerr << "Wrong command usgae" << std::endl;
                m_info.clear();
                m_request = "help";
            }
            else if (m_command != "help")
            {
                std::cin >> m_info;
                m_request = m_command + " " + m_info;
            }
            else
            {
                m_info.clear();
                m_request = m_command;
            }
            /* backend parse the info to be encrypted or decryped started from
            the index 8, which means you should add a space or any other
            character to occupy this pos.*/
            m_serial_port->write_some(boost::asio::buffer(m_request));
            auto read_size =
                m_serial_port->read_some(boost::asio::buffer(m_buffer));
            std::string response = std::string(m_buffer.data(), read_size);
            std::cout << response << std::endl;
        }
    }

   private:
    std::shared_ptr<boost::asio::serial_port> m_serial_port;
    std::string m_command;
    std::string m_info;
    std::string m_request;
    std::array<char, 1024> m_buffer;
};

int main()
{
    std::string port_name = "/dev/ttyUSB1";

    boost::asio::io_context io_context;

    SerialConsole console = SerialConsole(io_context, port_name);
    console.start();

    return 0;
}