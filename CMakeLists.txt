cmake_minimum_required(VERSION 3.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS "1")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Bollbok VERSION 1.0.2)

enable_testing()
add_subdirectory(ext)
add_subdirectory(src)
add_subdirectory(installer)
