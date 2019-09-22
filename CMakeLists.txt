cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(book)

enable_testing()
add_subdirectory(ext)
add_subdirectory(src)
