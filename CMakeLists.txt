cmake_minimum_required(VERSION 3.8)
project(flowey-fight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_executable(floweyfight ./src/flowey.cpp)

target_include_directories(floweyfight PRIVATE ./lib/cppLibrary/)

# Link system libraries (Windows)
#target_link_libraries(floweyfight PRIVATE opengl3)

## user32 and gdi32 are linked automatically by Windows toolchain normally, but ensure:
target_link_libraries(floweyfight PRIVATE user32 gdi32)