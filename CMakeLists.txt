cmake_minimum_required(VERSION 3.6)
project(test)

set(CMAKE_C_STANDARD 99)

include_directories(/usr/include/xcb /usr/include/X11)
link_libraries(/usr/lib/libxcb.so /usr/lib/libX11.so)

set(SOURCE_FILES main.c)
add_executable(sycbwm ${SOURCE_FILES})