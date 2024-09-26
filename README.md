# 设备间通信实验

## 串口

串口实验通过虚拟串口实现，需要使用socat创建。

以sudo权限运行以下命令：

```bash
./scripts/VirtualSerial.sh
```

该脚本会创建两个虚拟串口，分别是/dev/ttyUSB0和/dev/ttyUSB1。
socat会将这两个串口连接起来，数据从一个串口输入，会从另一个串口输出。

### 实验一 操作系统原生串口通信

运行native_handle_receiver和native_handle_sender。
你需要为这两个程序分别指定串口设备名，例如：

```bash
./native_handle_receiver /dev/ttyUSB0
./native_handle_sender /dev/ttyUSB1
```

### 实验二 使用ASIO库进行串口通信

运行boost_asio_receiver和boost_asio_sender。
你需要为这两个程序分别指定串口设备名，例如：

```bash
./boost_serial_receiver /dev/ttyUSB0
./boost_serial_sender /dev/ttyUSB1
```

### 实验三 串口通信应用实验

运行boost_serial_backend，它会监听一个串口

运行boost_serial_console，它会监听另一个串口

在boost_serial_console中输入消息，消息会被发送到boost_serial_backend；

根据boost_serial_backend的回复，可以加密或者解密消息。

**你需要设计一个程序，参考boost_serial_console的实现，向backend发送指令加密如下信息：**
```
BUPT-RobotTeam-学号
```
**随后对你得到的密文进行解密。**
**你可以通过比对解密后的明文和原文来你的串口通信程序是否正确。**

