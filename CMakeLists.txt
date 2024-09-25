cmake_minimum_required(VERSION 3.29)

project(COMMUNICATION_TEACHING LANGUAGES C CXX)

find_package(Boost REQUIRED CONFIG)

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
    native_handle_recevier
    src/native_handle_recevier.cpp 
)
target_link_libraries(native_handle_recevier PRIVATE native_system_handle)

add_custom_target(
    run_sender 
    COMMAND native_handle_sender /dev/ttyUSB0
)

add_custom_target(
    run_receiver 
    COMMAND native_handle_recevier /dev/ttyUSB1
)
