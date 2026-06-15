cmake_minimum_required(VERSION 3.17)

set(CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/../dependencies/cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_DIR}")

include(${CMAKE_DIR}/arm/K1921VK035.cmake)
include(${CMAKE_DIR}/common.cmake)

project(K1946VK035_BLDC_BOARD VERSION 1.0 LANGUAGES C ASM)

# ---------------------------------------------------------------------------
# Platform SDK (headers only, no niietsdk build)
# ---------------------------------------------------------------------------
# Using local system_K1921VK035.c and K1921VK035.h from project

# ---------------------------------------------------------------------------
# Target
# ---------------------------------------------------------------------------
add_executable(K1946VK035_BLDC_BOARD)

target_sources(K1946VK035_BLDC_BOARD
    PRIVATE
        K1921VK035_POWER_BOARD/src/main.c
        K1921VK035_POWER_BOARD/src/PWM_Control.c
        K1921VK035_POWER_BOARD/src/system_K1921VK035.c
        K1921VK035_POWER_BOARD/asm/startup_K1921VK035.S
)

target_include_directories(K1946VK035_BLDC_BOARD
    PRIVATE
        K1921VK035_POWER_BOARD/include
)

target_link_libraries(K1946VK035_BLDC_BOARD
    PRIVATE
        K1921VK035
        NANO
        NOSYS
        m
)

target_compile_definitions(K1946VK035_BLDC_BOARD PRIVATE
)

target_compile_options(K1946VK035_BLDC_BOARD PRIVATE
    $<$<CONFIG:Release>:-Os>
)

target_link_options(K1946VK035_BLDC_BOARD PRIVATE
    -Wl,-Map=${CMAKE_BINARY_DIR}/K1946VK035_BLDC_BOARD.map
)

target_link_libraries(K1946VK035_BLDC_BOARD
    PRIVATE
        K1921VK035
        NANO
        NOSYS
        m
)

# Override -nostartfiles from K1921VK035 since we provide our own startup.S
set_target_properties(K1946VK035_BLDC_BOARD PROPERTIES
    LINK_FLAGS "-Wl,--entry=Reset_Handler"
)

# Use niietsdk's default linker script
gcc_add_linker_script(K1946VK035_BLDC_BOARD PRIVATE ${K1921VK035_LD_SCRIPT})
gcc_generate_hex_file(K1946VK035_BLDC_BOARD)
