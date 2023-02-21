add_subdirectory(tests)

set(SRCS
    report.cpp
    report.h
)

add_library(reports STATIC ${SRCS})
target_link_libraries(reports bolldoc utf-cpp)
target_include_directories(reports PUBLIC .)
if(MSVC)
	target_compile_options(reports PRIVATE -W4 -WX -experimental:external -external:anglebrackets -external:W3)
    target_compile_options(reports PRIVATE -D_CRT_SECURE_NO_WARNINGS)
else()
	target_compile_options(reports PRIVATE -Wall -Werror)
endif()
