cmake_minimum_required(VERSION 3.22)

# Create a library for eez-ui
# We use GLOB to find all source files in src/ui
file(GLOB SOURCES "src/ui/*.c" "src/ui/*.cpp")

add_library(EEZ-UI ${SOURCES})

# Include directories for this library
# PUBLIC means that targets linking to this library will also have these include paths
target_include_directories(EEZ-UI PUBLIC 
    src/ui
)

# eez-ui depends on lvgl
target_link_libraries(EEZ-UI PUBLIC lvgl)
