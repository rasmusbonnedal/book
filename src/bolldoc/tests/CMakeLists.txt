set(TEST_SRCS
    test_bolldoc.cpp
    test_date.cpp
    test_pengar.cpp
    test_resultat.cpp
    test_serialize.cpp
    test_utils.cpp
)

add_executable(test_bolldoc ${TEST_SRCS})
target_link_libraries(test_bolldoc doctest bolldoc)
add_test(test_bolldoc test_bolldoc)
