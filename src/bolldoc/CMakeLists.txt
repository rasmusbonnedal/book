set(SRCS
    bolldoc.cpp
    bolldoc.h
    date.cpp
    date.h
    serialize.cpp
    serialize.h
    utils.cpp
    utils.h
)

add_library(bolldoc STATIC ${SRCS})
target_link_libraries(bolldoc rapidxml)

add_executable(test_bolldoc test_bolldoc.cpp test_date.cpp test_serialize.cpp test_utils.cpp)
target_link_libraries(test_bolldoc doctest bolldoc)
add_test(test_bolldoc test_bolldoc)
