cmake_minimum_required(VERSION 3.1)
project(SmartHomeSim)

set(CMAKE_CXX_STANDARD 14)

# only works on Linux so far!
add_definitions(-DUNIX)
add_definitions(-DZB_PLATFORM_LINUX_PC32)

set(SHS_BINARY_DIR "${PROJECT_SOURCE_DIR}/lib")

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SHS_BINARY_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SHS_BINARY_DIR})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SHS_BINARY_DIR})

add_subdirectory(zboss)

add_subdirectory(zboss-dump-converter)
add_subdirectory(zboss-network-sim)
add_subdirectory(dump-converter)

set(BUILD_TEST TRUE)

if(BUILD_TEST)
    message(STATUS "building test...")
    add_subdirectory(test)
endif()



add_executable(shs_test source/test-main.cpp)




