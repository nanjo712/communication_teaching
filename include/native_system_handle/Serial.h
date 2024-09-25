#ifndef SERIAL_H_
#define SERIAL_H_

#include <fcntl.h>
#include <string>
#include <unistd.h>

void configureSerial(int fd);

class Serial
{
public:
    Serial(std::string serialName);
    ~Serial();
    
    void close();
    decltype(::write(0,0,0)) write(const void* buf, size_t len);
    decltype(::read(0,0,0)) read(void* buf, size_t len);
private:
    int fd;
};

#endif