#!/bin/bash

# 定义虚拟串口设备名
DEVICE1=/dev/ttyUSB0
DEVICE2=/dev/ttyUSB1

# 检查 socat 是否已安装
if ! command -v socat &> /dev/null; then
    echo "socat not found. Please install socat."
    exit 1
fi

# 启动 socat 在后台创建虚拟串口
(socat -d -d pty,raw,echo=0,link=${DEVICE1},mode=666 pty,raw,echo=0,link=${DEVICE2},mode=777) &

# 等待 1s 
sleep 1

# 输出虚拟串口设备路径
echo "Virtual serial ports created:"
ls -l ${DEVICE1} ${DEVICE2}

# 记录 socat 的 PID
SOCAT_PID=$!

# 输出 socat 的 PID
echo "socat running in background with PID: $SOCAT_PID"

# 提示用户结束虚拟串口服务
read -p "Press Enter to stop the virtual serial ports..."

# 结束 socat 进程
kill -9 $SOCAT_PID

echo "Virtual serial ports stopped."