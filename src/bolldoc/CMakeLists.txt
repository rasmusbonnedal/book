add_subdirectory(tests)

set(SRCS
    bolldoc.cpp
    bolldoc.h
    date.cpp
    date.h
    pengar.cpp
    pengar.h
    resultat.cpp
    resultat.h
    serialize.cpp
    serialize.h
    utils.cpp
    utils.h
)

add_library(bolldoc STATIC ${SRCS})
target_link_libraries(bolldoc rapidxml)
target_include_directories(bolldoc PUBLIC .)