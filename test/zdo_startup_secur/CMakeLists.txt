cmake_minimum_required(VERSION 3.1)
project(sms-test-zdo-secur-startup C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ZBOSS_TRACE_FLAGS}")

include_directories(${ZBOSS_INCLUDE_DIRS})


add_executable(zdo_start_sec_zc zdo_start_sec_zc.c)
add_executable(zdo_start_sec_ze zdo_start_sec_ze.c)
add_executable(zdo_start_sec_zr zdo_start_sec_zr.c)
target_link_libraries(zdo_start_sec_zc ${zboss_components})
target_link_libraries(zdo_start_sec_zr ${zboss_components})

target_compile_definitions(zdo_start_sec_ze PRIVATE ZB_ED_ROLE)
target_link_libraries(zdo_start_sec_ze ${zboss_components_ed})

set_target_properties(zdo_start_sec_zc PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)
set_target_properties(zdo_start_sec_zr PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)
set_target_properties(zdo_start_sec_ze PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)

add_custom_target(zdo_startup_secur)
add_dependencies(zdo_startup_secur zdo_start_sec_zc zdo_start_sec_ze zdo_start_sec_zr
        new_dump_converter network_simulator)


