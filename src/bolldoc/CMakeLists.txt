set(SRCS
    bolldoc.cpp
    bolldoc.h
)

set(TEST_SRCS
    test_bolldoc.cpp
)

add_library(bolldoc STATIC ${SRCS})
target_link_libraries(bolldoc rapidxml)

add_executable(test_bolldoc ${TEST_SRCS})
target_link_libraries(test_bolldoc doctest)

add_test(test_bolldoc test_bolldoc)