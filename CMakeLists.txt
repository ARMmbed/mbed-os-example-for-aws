# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

set(MBED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/mbed-os CACHE INTERNAL "")
set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_TARGET mbed-os-example-for-aws)

include(${MBED_PATH}/tools/cmake/app.cmake)

add_subdirectory(${MBED_PATH})
add_subdirectory(mbed-client-for-aws)

if("-DMBED_CONF_ISM43362_PROVIDE_DEFAULT=1" IN_LIST MBED_CONFIG_DEFINITIONS)
    add_subdirectory(wifi-ism43362)
endif()

add_executable(${APP_TARGET})

mbed_configure_app_target(${APP_TARGET})

project(${APP_TARGET})

target_include_directories(${APP_TARGET}
    PRIVATE
        .
)

target_sources(${APP_TARGET}
    PRIVATE
        main.cpp
        demo_mqtt.cpp
        demo_shadow.cpp
)

if("-DMBED_CONF_ISM43362_PROVIDE_DEFAULT=1" IN_LIST MBED_CONFIG_DEFINITIONS)
    target_link_libraries(${APP_TARGET}
        PRIVATE
            wifi-ism43362
    )
endif()

target_link_libraries(${APP_TARGET}
    PRIVATE
        mbed-os
        mbed-client-for-aws
)

mbed_set_post_build(${APP_TARGET})

option(VERBOSE_BUILD "Have a verbose build process")
if(VERBOSE_BUILD)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()
