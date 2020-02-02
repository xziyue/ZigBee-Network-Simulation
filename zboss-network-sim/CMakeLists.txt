cmake_minimum_required(VERSION 3.1)
project(network_simulator)

set(CMAKE_CXX_STANDARD 14)

find_package(LibXml2 REQUIRED)

include_directories(${ZBOSS_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${LIBXML2_INCLUDE_DIR})

file(GLOB src ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/source/*.cpp)


add_executable(network_simulator ${src})
target_link_libraries(network_simulator ${zboss_components} ${LIBXML2_LIBRARIES})

set_target_properties(network_simulator PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)