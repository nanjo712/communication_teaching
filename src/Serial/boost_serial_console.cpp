#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>

std::mutex io_mutex; //chatgpt 给出的互斥锁

//读线程函数
void read_data(boost::asio::serial_port& serial_port)
{
    char read_buf[1024];
    while(1){
        boost::system::error_code ec;
        size_t len =serial_port.read_some(boost::asio::buffer(read_buf), ec);
        if(ec){
            std::cerr<<"Error: "<<ec.message()<<std::endl;
            break;
        }

        if(len>0){
            std::lock_guard<std::mutex> lock(io_mutex);
            std::cout<<"Received: "<<std::string(read_buf, len)<<std::endl;
            std::cout<<">>";
            std::cout.flush();
        }
    }
    /*std::size_t len = serial_port.read_some(boost::asio::buffer(read_buf));
    std::cout<<"Received: "<<std::string(read_buf, len)<<std::endl;*/
}

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

    // chatgpt 给出的读线程实例，不太懂( ﾟ∀。)，但能用
    std::thread read_thread(read_data, std::ref(serial_port));

    std::string data_to_send;
    //char read_buf[1024];
    
    while (1)
    {
        {
            std::lock_guard<std::mutex> lock(io_mutex);
            std::cout<<">>";
            std::cout.flush();//强制将缓冲区的数据输出
        }
        std::getline(std::cin, data_to_send);
        if (data_to_send == "exit"){
            read_thread.join();
            return 0;
        }
        boost::asio::write(serial_port, boost::asio::buffer(data_to_send));

        /*std::size_t len = serial_port.read_some(boost::asio::buffer(read_buf));
        std::cout<<"Received: "<<std::string(read_buf, len)<<std::endl;*/
    }
    read_thread.join();
    return 0;
}