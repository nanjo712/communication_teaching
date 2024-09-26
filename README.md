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

- 接收用户输入指令，发送至backend
- 接收backend的回复，打印到控制台

向backend发送指令加密如下信息：
```
BUPT-RobotTeam-学号
```
根据密文构造解密指令，发送至backend解密
你会得到一个回复，回复中包含你的完整明文和一个新的字符串。
