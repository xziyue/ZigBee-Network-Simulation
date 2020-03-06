cmake_minimum_required(VERSION 3.1)
project(sms-test C)

# only works in 32 bit mode
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")


macro(add_and_notify name)
    message(STATUS "adding test case ${name}")
    add_subdirectory(${name})
endmacro()

if(${ZB_USE_WPAN})
    message(STATUS "adding test cases for WPAN:")
    add_and_notify(zdo_wpan_startup)
elseif(${ZB_USE_PIPE})
    message(STATUS "adding test cases for PIPE:")
    add_and_notify(zdo_startup)
    add_and_notify(zdo_addr)
    add_and_notify(zdo_startup_secur)
else()
message(FATAL_ERROR "Please specify a transport type!")
endif()



