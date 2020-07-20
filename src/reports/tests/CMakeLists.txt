set(TEST_SRCS
    test_report.cpp
)

add_executable(test_reports ${TEST_SRCS})
target_link_libraries(test_reports doctest reports)
add_test(test_reports test_reports)
