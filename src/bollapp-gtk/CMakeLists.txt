find_package (PkgConfig REQUIRED)
pkg_check_modules (GTKMM REQUIRED gtkmm-3.0)

set(SRCS
    cellrenderertextcompletion.cpp
    main.cpp
)

add_executable(bollapp ${SRCS})
target_link_libraries(bollapp bolldoc ${GTKMM_LIBRARIES})
target_include_directories(bollapp PUBLIC . ${GTKMM_INCLUDE_DIRS})
if(MSVC)
    target_compile_options(bollapp PRIVATE -W4 -WX -experimental:external -external:anglebrackets -external:W0)
else()
    target_compile_options(bollapp PRIVATE -Wall -Werror)
    target_compile_options(bollapp PRIVATE -Wno-deprecated-declarations)
endif()
