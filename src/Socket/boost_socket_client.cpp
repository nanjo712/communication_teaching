#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <iostream>
#include<stdio.h>
#include<cstdlib>

int main()
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve("localhost", "12345"));

    std::string message = "time";
    socket.write_some(boost::asio::buffer(message));
    std::string response;
   response.resize(1024);
    socket.read_some(boost::asio::buffer(response));
    std::cout << "Response: " << response << std::endl;
//从这里开始
//发送功能
char yonghushuru[128];
 printf("enter:");
std::cin.getline(yonghushuru,128);
socket.write_some(boost::asio::buffer(yonghushuru));
response.resize(1024);
socket.read_some(boost::asio::buffer(response));
    std::cout << "Response: " << response << std::endl;
 //退出功能    
if(response=="EXIT")
exit(0);
printf("send:%s",yonghushuru);


    return 0;
}