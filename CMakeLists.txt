cmake_minimum_required(VERSION 3.29)

project(COMMUNICATION_TEACHING LANGUAGES C CXX)

set(EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED CONFIG COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_library(
    native_system_handle
    src/Serial/Serial.cpp
    src/Socket/Socket.cpp 
)
target_include_directories(native_system_handle PUBLIC include)

add_executable(
    Serial_sender
    src/Serial/Serial_sender.cpp 
)
target_link_libraries(Serial_sender PRIVATE native_system_handle)

add_executable(
    Serial_receiver
    src/Serial/Serial_receiver.cpp 
)
target_link_libraries(Serial_receiver PRIVATE native_system_handle)

add_executable(
    boost_serial_sender
    src/Serial/boost_serial_sender.cpp
)
target_link_libraries(boost_serial_sender PRIVATE Boost::system Threads::Threads)

add_executable(
    boost_serial_receiver
    src/Serial/boost_serial_receiver.cpp
)
target_link_libraries(boost_serial_receiver PRIVATE Boost::system Threads::Threads)

add_executable(
    boost_serial_backend
    src/Serial/boost_serial_backend.cpp
)
target_link_libraries(boost_serial_backend PRIVATE Boost::system OpenSSL::SSL OpenSSL::Crypto Threads::Threads)

add_executable(
    boost_serial_console
    src/Serial/boost_serial_console.cpp
)
target_link_libraries(boost_serial_console PRIVATE Boost::system Threads::Threads)