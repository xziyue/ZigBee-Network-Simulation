cmake_minimum_required(VERSION 3.1)
project(SmartHomeSim)

set(CMAKE_CXX_STANDARD 14)

# only works on Linux so far!
add_definitions(-DUNIX)
add_definitions(-DZB_PLATFORM_LINUX_PC32)

# use linux pipes
add_definitions(-DZB_TRANSPORT_USE_LINUX_PIPES)

# use linux wpan
#add_definitions(-DZB_TRANSPORT_USE_LINUX_WPAN)

set(SHS_BINARY_DIR "${PROJECT_SOURCE_DIR}/lib")

include_directories(include)

# settings from pipe library
add_library(settings_from_pipe source/settings_from_pipe.cpp include/settings_from_pipe.h)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SHS_BINARY_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SHS_BINARY_DIR})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SHS_BINARY_DIR})

add_subdirectory(targets/zboss)

add_subdirectory(targets/zboss-dump-converter)
add_subdirectory(targets/zboss-network-sim)
add_subdirectory(targets/dump-converter)

set(BUILD_TEST TRUE)

if(BUILD_TEST)
    message(STATUS "building test...")
    add_subdirectory(test)
endif()


add_executable(test_802154_socket source/test_802154_socket.c)
set_target_properties(test_802154_socket PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)

add_executable(test_settings_from_pipe source/test_settings_from_pipe.c)
set_target_properties(test_settings_from_pipe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)
target_link_libraries(test_settings_from_pipe settings_from_pipe)


