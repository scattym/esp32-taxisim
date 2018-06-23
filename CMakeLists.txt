# NOTE: This file is only used for configuring CLion, use regular `make` for building.

set (IDF_PATH "/home/sysadmin/esp/esp-idf/")

cmake_minimum_required(VERSION 3.6)
project(esp_idf_template)

set(CMAKE_SYSTEM_NAME xtensa-esp32-elf)
set(CMAKE_C_COMPILER xtensa-esp32-elf-gcc)
set(CMAKE_FIND_ROOT_PATH /home/sysadmin/esp/xtensa-esp32-elf/)

set(CMAKE_CXX_STANDARD 11)


file(GLOB_RECURSE SOURCES "${IDF_PATH}/*.c")

set(COMPONENTS
        app_update
        bootloader
        bootloader_support
        bt
        coap
        cxx
        driver
        esp32
        esptool_py
        ethernet
        expat
        fatfs
        freertos
        idf_test
        json
        log
        lwip
        mbedtls
        micro-ecc
        newlib
        nghttp
        nvs_flash
        openssl
        partition_table
        sdmmc
        spi_flash
        soc
        soc/esp32
        tcpip_adapter
        ulp
        vfs
        wpa_supplicant
        xtensa-debug-module)

set (INCLUDE_DIRS "")
foreach (component ${COMPONENTS})
    list (APPEND INCLUDE_DIRS ${IDF_PATH}/components/${component}/include)
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)
include_directories(${INCLUDE_DIRS})
include_directories("build/include")
include_directories("components/gwp5043/include/")
include_directories("components/touchpad_api/include/")
include_directories("components/simple_wifi/include/")

set(SOURCE_FILES
        main/taxisim.c
        components/gwp5043/gwp5043.c
        components/gwp5043/include/gwp5043.h
        components/touchpad_api/touchpad_api.c
        components/touchpad_api/include/touchpad_api.h
        components/simple_wifi/simple_wifi.c
        components/simple_wifi/include/simple_wifi.h)
list (APPEND SOURCE_FILES ${SOURCES})

add_executable(esp_dimmer ${SOURCE_FILES})