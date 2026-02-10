cmake_minimum_required(VERSION 3.22)

# Create a library for eez-ui
# We use GLOB to find all source files in src/ui
file(GLOB SOURCES "src/ui/*.c")

add_library(eez-ui ${SOURCES})

# Include directories for this library
# PUBLIC means that targets linking to this library will also have these include paths
target_include_directories(eez-ui PUBLIC 
    src/ui
)

# eez-ui depends on lvgl
target_link_libraries(eez-ui PUBLIC lvgl)
