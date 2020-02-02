cmake_minimum_required(VERSION 3.1)
project(zboss C)

# only works in 32 bit mode
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")

list(APPEND ZBOSS_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/zdo" "${PROJECT_SOURCE_DIR}/osif/include")
set(ZBOSS_INCLUDE_DIRS ${ZBOSS_INCLUDE_DIRS} PARENT_SCOPE)
include_directories(${ZBOSS_INCLUDE_DIRS})

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -pipe -std=gnu99 -W -Wall -Wpointer-arith -Wcast-align \
 -Wno-format-zero-length  -Wno-format-zero-length -Wno-int-to-pointer-cast \
 -Wno-pointer-to-int-cast -Wno-unused-value -Wno-unused-local-typedefs \
 -Wno-unused-but-set-variable -Wno-shift-negative-value")
else()
    message(FATAL_ERROR "only supports GCC so far!")
endif()

set(ZBOSS_TRACE_FLAGS "-DZB_TRACE_LEVEL=4 -DZB_TRACE_MASK=-1" PARENT_SCOPE)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("DEBUG Build")
    add_definitions(-DZB_TRACE_LEVEL)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ZBOSS_TRACE_FLAGS} -DDEBUG")
endif()

#message(STATUS ${CMAKE_C_FLAGS})

list(APPEND zboss_components "aps" "common" "osif" "mac" "nwk" "secur" "zdo")
set(zboss_components ${zboss_components} PARENT_SCOPE)

foreach(tar ${zboss_components})
    file(GLOB "${tar}_files" "${zboss_SOURCE_DIR}/${tar}/*.c")
    add_library(${tar} STATIC ${${tar}_files})
    list(APPEND zboss_components_ed "${tar}_ed")
    add_library("${tar}_ed" STATIC ${${tar}_files})
    target_compile_definitions("${tar}_ed" PRIVATE ZB_ED_ROLE)
    set_target_properties("${tar}_ed" PROPERTIES OUTPUT_NAME "${tar}.ed")
endforeach()

set(zboss_components_ed ${zboss_components_ed} PARENT_SCOPE)

target_link_libraries(nwk mac aps)
target_link_libraries(zdo mac aps nwk)

target_link_libraries(nwk_ed mac_ed aps_ed)
target_link_libraries(zdo_ed mac_ed aps_ed nwk_ed)

#add_executable(zboss_build_all zboss_dummy.c)
#target_link_libraries(zboss_build_all ${zboss_components} ${zboss_components_ed})
