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
target_link_libraries(bolldoc rapidxml utf-cpp)
target_include_directories(bolldoc PUBLIC .)
if(MSVC)
	target_compile_options(bolldoc PRIVATE -W4 -WX -experimental:external -external:anglebrackets -external:W3)
else()
	target_compile_options(bolldoc PRIVATE -Wall -Werror)
endif()
