cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Dashboard)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add LVGL
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
add_subdirectory(Drivers/lvgl)
target_link_libraries(lvgl PUBLIC stm32cubemx)

# Exclude unused desktop/OS platform drivers from the LVGL build (STM32 only needs display/)
set(LVGL_DRIVERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/lvgl/src/drivers")
file(GLOB_RECURSE LVGL_UNUSED_DRIVERS
    "${LVGL_DRIVERS_DIR}/evdev/*.c"
    "${LVGL_DRIVERS_DIR}/libinput/*.c"
    "${LVGL_DRIVERS_DIR}/nuttx/*.c"
    "${LVGL_DRIVERS_DIR}/opengles/*.c"
    "${LVGL_DRIVERS_DIR}/qnx/*.c"
    "${LVGL_DRIVERS_DIR}/sdl/*.c"
    "${LVGL_DRIVERS_DIR}/uefi/*.c"
    "${LVGL_DRIVERS_DIR}/wayland/*.c"
    "${LVGL_DRIVERS_DIR}/windows/*.c"
    "${LVGL_DRIVERS_DIR}/x11/*.c"
    "${LVGL_DRIVERS_DIR}/draw/eve/*.c"
)
set_source_files_properties(${LVGL_UNUSED_DRIVERS} TARGET_DIRECTORY lvgl PROPERTIES HEADER_FILE_ONLY TRUE)

# Add LV3-CAN-Subsystem library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../LV3-CAN-Subsystem ${CMAKE_CURRENT_BINARY_DIR}/LV3-CAN-Subsystem)

# Add EEZ Studio UI
add_subdirectory(EEZ-UI)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/FT5206.c
    Core/Src/EEPROM.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Core/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    lvgl
    LV3-CAN-Subsystem
    EEZ-UI
)

# Apply driver implementation define to the CAN subsystem
target_compile_definitions(LV3-CAN-Subsystem PRIVATE
    LV3_CAN_DRIVER_IMPL__LV3_DASH_R0
)