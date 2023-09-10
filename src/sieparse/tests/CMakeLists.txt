file(GLOB TEST_SRCS *.cpp *.h)

add_executable(test_sieparse ${TEST_SRCS})
target_link_libraries(test_sieparse doctest sieparse)
add_test(test_sieparse test_sieparse)
