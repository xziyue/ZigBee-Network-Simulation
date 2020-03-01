cmake_minimum_required(VERSION 3.1)
project(sms-test C)

# only works in 32 bit mode
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")

add_subdirectory(zdo_startup)
add_subdirectory(zdo_addr)
add_subdirectory(zdo_startup_secur)
add_subdirectory(zdo_wpan_startup)