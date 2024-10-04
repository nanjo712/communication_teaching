#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::serial_port;

int main(int argc, char* argv[])
{
    // 串口名称
    std::string port_name = "/dev/ttyUSB1";
    
    try {
        // 创建io_context
        boost::asio::io_context io_context;

        // 打开串口
        serial_port serial(io_context, port_name);
        
        // 设置串口参数（复制于backend）
        serial.set_option(boost::asio::serial_port::baud_rate(115200));
        serial.set_option(boost::asio::serial_port::character_size(8));
        serial.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
        serial.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
        serial.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));

        // 发送信息
        std::string initial_message = "encrypt BUPT-RobotTeam-2024211688";
        boost::asio::write(serial, boost::asio::buffer(initial_message));
        std::cout << "Sent: " << initial_message << std::endl;

        // 接收回复
        char reply[3000];
        size_t reply_length = boost::asio::read(serial, boost::asio::buffer(reply, sizeof(reply)));
        
        // 输出收到的回复(这是密文？)
        std::cout << "Received: ";
        std::cout.write(reply, reply_length);
        std::cout << std::endl;

        // 构造decrypt命令（解密）
        std::string decrypt_command = "decrypt ";
        std::string content(reply, reply_length);  // 将回复转换为字符串
        decrypt_command += content;  // 追加接收到的内容

        
        // 再次发送decrypt命令
        boost::asio::write(serial, boost::asio::buffer(decrypt_command));
        std::cout << "Sent modified command: " << decrypt_command << std::endl;

        // 接收回复（明文和字符串）
        char reply2[3000];
        size_t reply2_length = boost::asio::read(serial, boost::asio::buffer(reply2, sizeof(reply2)));
       //显示 回复
       printf("message:");
        std::cout.write(reply, reply_length);
        printf("\n");
       
       
        // 关闭串口
        serial.close();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
