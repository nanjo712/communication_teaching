#include <boost/asio.hpp>
#include <iomanip>
#include <iostream>
#include <memory>

using boost::asio::ip::tcp;

std::string get_current_time()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

class Session : public std::enable_shared_from_this<Session>
{
   public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}
    ~Session() { std::cout << "Session closed" << std::endl; }

    void start() { do_read(); }

   private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(
            boost::asio::buffer(read_buffer),
            [this, self](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    // Modify Code here
                    read_buffer[length] = '\0';
                    std::cout << "Received: " << read_buffer << std::endl;
                    std::string command = std::string(read_buffer, 4);
                    if (command == "time")
                    {
                        std::string time = get_current_time();
                        std::copy(time.begin(), time.end(), write_buffer);
                        do_write(time.size());
                    }
                    else if (command == "echo")
                    {
                        std::copy(&read_buffer[5], &read_buffer[length],
                                  write_buffer);
                        do_write(length - 5);
                    }
                    else if (command == "exit")
                    {
                        this->close();
                    }
                    else if (command == "help")
                    {
                        std::string help_msg = show_usage();
                        std::copy(help_msg.begin(), help_msg.end(),
                                  write_buffer);
                        do_write(help_msg.size());
                    }
                    else
                    {
                        std::copy(read_buffer, read_buffer + length,
                                  write_buffer);
                        do_write(length);
                    }
                }
            });
    }

    std::string show_usage()
    {
        std::stringstream ss;
        ss << "Usage:\n";
        ss << "  help                - Display this help message\n";
        ss << "  time                - Return current time\n";
        ss << "  echo <your script>  - Return the srcipt you sended\n";
        ss << "  exit                - Close connection and exit\n";
        return ss.str();
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_, boost::asio::buffer(write_buffer, length),
            [this, self](std::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    do_read();
                }
            });
    }

    void close() { socket_.close(); }

    tcp::socket socket_;
    char read_buffer[1024];
    char write_buffer[1024];
};

class Server
{
   public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

   private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<Session>(std::move(socket))->start();
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        boost::asio::io_context io_context;
        Server s(io_context, 12345);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
