cmake_minimum_required(VERSION 3.1)
project(sms-test C)

# only works in 32 bit mode
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")

get_directory_property(DirDefs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS)

list(FIND DirDefs "ZB_TRANSPORT_USE_LINUX_WPAN" _indexwpan)
list(FIND DirDefs "ZB_TRANSPORT_USE_LINUX_PIPES" _indexpipe)

macro(add_and_notify name)
    message(STATUS "adding test case ${name}")
    add_subdirectory(${name})
endmacro()

if(${_indexwpan} GREATER -1)
    message(STATUS "using Linux wpan mode")
    add_and_notify(zdo_wpan_startup)
elseif(${_indexpipe} GREATER -1)
    message(STATUS "using Linux PIPE mode")
    add_and_notify(zdo_startup)
    add_and_notify(zdo_addr)
    add_and_notify(zdo_startup_secur)
else()
message(FATAL_ERROR "Please specify a transport type!")
endif()



