cmake_minimum_required(VERSION 3.8)
project(flowey-fight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_executable(floweyfight ./src/flowey.cpp)

#target_include_directories(floweyfight PRIVATE ./lib/cppLibrary/)
target_include_directories(floweyfight SYSTEM PRIVATE ./lib/cppLibrary)

# user32 and gdi32 are linked automatically by Windows toolchain normally, but ensure:
target_link_libraries(floweyfight PRIVATE user32 gdi32)

# Blocks *conversion from size_t to int potential loss of data* compiler warning
target_compile_options(floweyfight PRIVATE /wd4267) 