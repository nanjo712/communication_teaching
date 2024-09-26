#include <iostream>
#include <thread>

#include "native_system_handle/Serial.h"

int main(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL)
    {
        std::cout << "Please give a seria name" << std::endl;
    }

    std::string serialName(argv[1]);
    Serial serial(serialName);

    int cnt = 0;
    while (1)
    {
        auto size = serial.read(&cnt, sizeof(cnt));
        if (size != sizeof(cnt))
        {
            std::cout << "Error when reading to " << serialName << std::endl;
        }
        else
            std::cout << "Reading " << cnt << " to " << serialName << std::endl;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
    serial.close();
}