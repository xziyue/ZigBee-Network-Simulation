cmake_minimum_required(VERSION 3.1)
project(dump_converter C)

include(${SmartHomeSim_SOURCE_DIR}/cmake/FindPCAP.cmake)
include_directories(${ZBOSS_INCLUDE_DIRS})
include_directories(${PCAP_INCLUDE_DIR})

add_executable(dump_converter ${PROJECT_SOURCE_DIR}/source/dump_converter.c)

target_link_libraries(dump_converter ${PCAP_LIBRARY})
set_target_properties(dump_converter PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)