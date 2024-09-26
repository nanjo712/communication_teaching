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

运行Serial_receiver和Serial_sender。
你需要为这两个程序分别指定串口设备名，例如：

```bash
./Serial_receiver /dev/ttyUSB0
./Serial_sender /dev/ttyUSB1
```

### 实验二 使用ASIO库进行串口通信

运行boost_asio_receiver和boost_asio_sender。
你需要为这两个程序分别指定串口设备名，例如：

```bash
./boost_serial_receiver /dev/ttyUSB0
./boost_serial_sender /dev/ttyUSB1
```

### 实验三 串口通信应用实验

运行boost_serial_backend，它会监听/dev/ttyUSB0

backend的指令格式可以通过对该串口发送help获取

运行boost_serial_console，它会监听/dev/ttyUSB1

**你需要实现boost_serial_console，其功能如下：**

向backend发送指令加密如下信息：
```
BUPT-RobotTeam-学号
```
根据密文构造解密指令，发送至backend解密
你会得到一个回复，回复中包含你的完整明文和一个新的字符串。

如果你想要一个更加完整的console，可以实现如下功能：

- 接收用户输入指令，发送至backend
- 接收backend的回复，打印到控制台

**你可以通过先向串口发送help来得到backend的指令格式，随后再继续设计你的程序**

## 网络

### 实验 使用socket进行通信

运行boost_socket_server和boost_socket_client。

Server将绑定在本机的12345端口上，Client将连接到本机的12345端口。

基础实现中，Server会接收Client发送的字符串，并打印出来；每次运行Client，Server都会收到一个“Hello, World!”。

**你需要实现boost_socket_server和boost_socket_client，其功能如下：**

- Client将用户输入的字符串发送给Server
- 向Server发送`time`，Server会返回当前时间
- 向Server发送`echo <your string>`，Server会返回你发送的字符串
- 向Server发送`exit`，Server会关闭连接，Client会退出
- 向Server发送`help`，Server会返回帮助信息

提示：你可以通过关闭Socket来实现Server的退出。

