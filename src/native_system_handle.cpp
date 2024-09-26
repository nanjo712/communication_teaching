#include <termios.h>

#include <stdexcept>

#include "native_system_handle/Serial.h"

void configureSerial(int fd)
{
    struct termios options;
    tcgetattr(fd, &options);  // get current options

    // set raw input mode
    cfmakeraw(&options);

    // set baud rate
    cfsetispeed(&options, B9600);  // set input baud rate
    cfsetospeed(&options, B9600);  // set output baud rate

    // enable receiver and assert DTR
    options.c_cflag |= (CLOCAL | CREAD);

    // set data bits, no parity bit, one stop bit
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;  // disable RTS/CTS hardware flow control

    // set buffers
    options.c_cc[VTIME] = 1;  // inter-character timer used
    options.c_cc[VMIN] = 0;  // blocking read until at least 1 character arrives

    tcsetattr(fd, TCSANOW, &options);  // apply attributes
}

Serial::Serial(std::string serialName)
    : fd(open(serialName.c_str(), O_RDWR | O_NOCTTY | O_SYNC))
{
    if (fd == -1) throw std::runtime_error("Error opening serial port");
    configureSerial(fd);
}

Serial::~Serial() { close(); }

void Serial::close() { ::close(fd); }

decltype(::write(0, 0, 0)) Serial::write(const void* buf, size_t len)
{
    return ::write(fd, buf, len);
}

decltype(::read(0, 0, 0)) Serial::read(void* buf, size_t len)
{
    return ::read(fd, buf, len);
}