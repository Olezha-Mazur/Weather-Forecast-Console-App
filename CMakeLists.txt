cmake_minimum_required(VERSION 3.26)
project(labwork7)

set(CMAKE_CXX_STANDARD 17)

if (WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

add_subdirectory(lib)
add_subdirectory(bin)
