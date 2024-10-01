#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex io_mutex; //chatgpt 给出的互斥锁
std::atomic<bool> run_flag(true); //控制线程退出的标志
std::condition_variable cv;
char read_buf[1024];

void read_handler(boost::asio::serial_port& serial_port, const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (!ec) {
        std::lock_guard<std::mutex> lock(io_mutex);
        std::cout << "Received: " << std::string(read_buf, bytes_transferred) << std::endl;
        std::cout << ">>";
        std::cout.flush();

        //再次启动异步读取,和嵌入式中断回调函数好像( ﾟ∀。) 
        serial_port.async_read_some(boost::asio::buffer(read_buf, 1024),
            [&serial_port](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                read_handler(serial_port, ec, bytes_transferred);
            });
    } else if (ec != boost::asio::error::operation_aborted) {
        std::cerr << "Error: " << ec.message() << std::endl;
    }
}

void read_data(boost::asio::serial_port& serial_port) {
    
    serial_port.async_read_some(boost::asio::buffer(read_buf, 1024),
        [&serial_port](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            read_handler(serial_port, ec, bytes_transferred);
        });
}

//读线程函数
/*void read_data(boost::asio::serial_port& serial_port)
{
    char read_buf[1024];
    while(run_flag){
        boost::system::error_code ec;
        size_t len =serial_port.read_some(boost::asio::buffer(read_buf), ec);
        if(ec){
            if(ec == boost::asio::error::operation_aborted){
                break;
            }
            std::cerr<<"Error: "<<ec.message()<<std::endl;
            continue;
        }

        std::lock_guard<std::mutex> lock(io_mutex);
        std::cout<<"Received: "<<std::string(read_buf, len)<<std::endl;
        std::cout<<">>";
        std::cout.flush();
    }
    /*std::size_t len = serial_port.read_some(boost::asio::buffer(read_buf));
    std::cout<<"Received: "<<std::string(read_buf, len)<<std::endl;
}*/

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

    // chatgpt 给出的读线程实例，lambda表达式不太懂( ﾟ∀。)，但能用
    std::thread read_thread([&io_context, &serial_port]() {//[]中的内容是传递给lambda表达式的参数
        read_data(serial_port);
        io_context.run();
    });

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
            {
                std::lock_guard<std::mutex> lock(io_mutex);
                run_flag=false;
                serial_port.cancel();
            }
            cv.notify_all();
            break;
        }
        boost::asio::write(serial_port, boost::asio::buffer(data_to_send));

        /*std::size_t len = serial_port.read_some(boost::asio::buffer(read_buf));
        std::cout<<"Received: "<<std::string(read_buf, len)<<std::endl;*/
    }
    printf("exiting...\n");
    read_thread.join();
    return 0;
}