add_executable(test_file_handler test_file_handler.cpp)
target_include_directories(test_file_handler PRIVATE ..)
target_link_libraries(test_file_handler doctest bollapp-imgui-lib)
add_test(NAME test_file_handler COMMAND test_file_handler)
