# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}" CACHE PATH "Path to the Pico SDK")
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(forge_firmware C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(forge_firmware
    src/main.c
    src/usb_descriptors.c
    src/midi.c
    src/mux.c
    src/pad.c
    src/sequencer.c
)

target_include_directories(forge_firmware PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(forge_firmware
    pico_stdlib
    hardware_adc
    tinyusb_device
    tinyusb_board
    m
)

target_compile_options(forge_firmware PRIVATE
    -Wall
    -Wextra
    -O2
)

pico_add_extra_outputs(forge_firmware)
pico_enable_stdio_usb(forge_firmware 0)
pico_enable_stdio_uart(forge_firmware 1)