cmake_minimum_required(VERSION 3.29)

project(COMMUNICATION_TEACHING LANGUAGES C CXX)

set(EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED CONFIG COMPONENTS system)
find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_library(
    native_system_handle
    src/native_system_handle.cpp 
)
target_include_directories(native_system_handle PUBLIC include)

add_executable(
    native_handle_sender
    src/native_handle_sender.cpp 
)
target_link_libraries(native_handle_sender PRIVATE native_system_handle)

add_executable(
    native_handle_receiver
    src/native_handle_receiver.cpp 
)
target_link_libraries(native_handle_receiver PRIVATE native_system_handle)

add_executable(
    boost_serial_sender
    src/boost_serial_sender.cpp
)
target_link_libraries(boost_serial_sender PRIVATE Boost::system)

add_executable(
    boost_serial_receiver
    src/boost_serial_receiver.cpp
)
target_link_libraries(boost_serial_receiver PRIVATE Boost::system)

add_executable(
    boost_serial_backend
    src/boost_serial_backend.cpp
)
target_link_libraries(boost_serial_backend PRIVATE Boost::system OpenSSL::SSL OpenSSL::Crypto)

add_executable(
    boost_serial_console
    src/boost_serial_console.cpp
)
target_link_libraries(boost_serial_console PRIVATE Boost::system)