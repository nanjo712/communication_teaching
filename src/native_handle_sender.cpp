#include <iostream>
#include <thread>

#include "native_system_handle/Serial.h"

int main(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL)
    {
        std::cout << "Please give a seria name\n";
    }

    std::string serialName(argv[1]);
    Serial serial(serialName);

    int cnt = 0;
    while (1)
    {
        auto size = serial.write(&cnt, sizeof(cnt));
        if (size != sizeof(cnt))
        {
            std::cout << "Error when writing to " << serialName << std::endl;
        }
        else
            std::cout << "Writing " << cnt << " to " << serialName << std::endl;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        cnt++;
    }
    serial.close();
}