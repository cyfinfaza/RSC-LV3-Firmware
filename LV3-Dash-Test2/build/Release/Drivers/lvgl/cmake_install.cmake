# Install script for directory: C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/LV3-Dash-Test2")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Users/cyfin/AppData/Local/stm32cube/bundles/gnu-tools-for-stm32/13.3.1+st.9/bin/arm-none-eabi-objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE DIRECTORY FILES "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/src" FILES_MATCHING REGEX "/[^/]*\\.h$" REGEX "/[^/]*\\_private\\.h$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE FILE FILES
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lv_version.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lvgl.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/lv_conf.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/build/Release/Drivers/lvgl/lib/liblvgl.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE FILE FILES
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lv_version.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lvgl.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/lv_conf.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" TYPE FILE FILES "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/build/Release/Drivers/lvgl/lvgl.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/build/Release/Drivers/lvgl/lib/liblvgl_thorvg.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE FILE FILES
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lv_version.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lvgl.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/lv_conf.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE DIRECTORY FILES "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/demos" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/build/Release/Drivers/lvgl/lib/liblvgl_demos.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE FILE FILES
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lv_version.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lvgl.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/lv_conf.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE DIRECTORY FILES "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/examples" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/build/Release/Drivers/lvgl/lib/liblvgl_examples.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE FILE FILES
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lv_version.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/Drivers/lvgl/lvgl.h"
    "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/lv_conf.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/cyfin/Documents/code/RSC-LV3-Firmware/LV3-Dash-Test2/build/Release/Drivers/lvgl/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
