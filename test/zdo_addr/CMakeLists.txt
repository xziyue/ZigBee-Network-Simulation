cmake_minimum_required(VERSION 3.1)
project(sms-test-zdo-addr C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ZBOSS_TRACE_FLAGS}")

include_directories(${ZBOSS_INCLUDE_DIRS})


add_executable(zdo_addr_zc zdo_test_nwk_addr_zc.c)
add_executable(zdo_addr_zr zdo_test_nwk_addr_zr.c)
target_link_libraries(zdo_addr_zc ${zboss_components})
target_link_libraries(zdo_addr_zr ${zboss_components})

#target_compile_definitions(zdo_start_ze PRIVATE ZB_ED_ROLE)
#target_link_libraries(zdo_start_ze ${zboss_components_ed})

set_target_properties(zdo_addr_zc PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)
set_target_properties(zdo_addr_zr PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SmartHomeSim_SOURCE_DIR}/bin)

add_custom_target(zdo_addr)
add_dependencies(zdo_addr zdo_addr_zc zdo_addr_zr new_dump_converter network_simulator)


