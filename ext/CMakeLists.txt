add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE doctest/doctest/)

add_library(rapidxml INTERFACE)
target_include_directories(rapidxml INTERFACE rapidxml/)
