add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE doctest/doctest/)

add_library(rapidxml INTERFACE)
target_include_directories(rapidxml INTERFACE rapidxml/)

add_library(utf-cpp INTERFACE)
target_include_directories(utf-cpp INTERFACE utf-cpp/include/)
