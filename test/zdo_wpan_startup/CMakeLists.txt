cmake_minimum_required(VERSION 3.1)
project(sms-test-zdo-startup C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ZBOSS_TRACE_FLAGS}")

include_directories(${ZBOSS_INCLUDE_DIRS})

add_executable(zdo_wpan_start_zc zdo_wpan_start_zc.c)
add_executable(zdo_wpan_start_ze zdo_wpan_start_ze.c)
add_executable(zdo_wpan_start_zr zdo_wpan_start_zr.c)
target_link_libraries(zdo_wpan_start_zc ${zboss_components} settings_from_pipe)
target_link_libraries(zdo_wpan_start_zr ${zboss_components} settings_from_pipe)

target_compile_definitions(zdo_wpan_start_ze PRIVATE ZB_ED_ROLE)
target_link_libraries(zdo_wpan_start_ze ${zboss_components_ed} settings_from_pipe)

set_target_properties(zdo_wpan_start_zc PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)
set_target_properties(zdo_wpan_start_zr PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)
set_target_properties(zdo_wpan_start_ze PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)

add_custom_target(zdo_wpan_startup)
add_dependencies(zdo_wpan_startup zdo_wpan_start_zc zdo_wpan_start_ze zdo_wpan_start_zr)


