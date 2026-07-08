cmake_minimum_required(VERSION 3.14)
project(bfc VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all source files in src/
file(GLOB BFC_SOURCES src/*.cpp)

add_executable(bfc ${BFC_SOURCES})
