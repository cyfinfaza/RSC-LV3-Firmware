cmake_minimum_required(VERSION 3.22)

# Create LV3-CAN-Subsystem library
add_library(LV3-CAN-Subsystem STATIC
    Src/LV3_CAN.c
    Driver/LV3_CAN_Driver.c
)

# Add include directories for this library
target_include_directories(LV3-CAN-Subsystem PUBLIC
    Src
    Driver
)

# Link against stm32cubemx if needed for HAL dependencies
target_link_libraries(LV3-CAN-Subsystem PUBLIC
    stm32cubemx
)
