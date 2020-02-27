cmake_minimum_required(VERSION 3.1)
project(new_dump_converter)

set(CMAKE_CXX_STANDARD 14)

include(${SmartHomeSim_SOURCE_DIR}/cmake/FindPCAP.cmake)
include_directories(${ZBOSS_INCLUDE_DIRS})
include_directories(${PCAP_INCLUDE_DIR})
include_directories(${SmartHomeSim_SOURCE_DIR}/include)

add_executable(new_dump_converter ${PROJECT_SOURCE_DIR}/source/dump_converter.cpp)

target_link_libraries(new_dump_converter ${PCAP_LIBRARY})
set_target_properties(new_dump_converter PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)