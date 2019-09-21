#add_subdirectory(tests)

set(SRCS
    main.cpp
	bollapp.rc
)

add_definitions(-D_AFXDLL -D_WIN32_WINNT=_WIN32_WINNT_WIN10)
set(CMAKE_MFC_FLAG 2)
add_executable(bollapp WIN32 ${SRCS})
target_link_libraries(bollapp bolldoc)
target_include_directories(bollapp PUBLIC .)
if(MSVC)
	target_compile_options(bollapp PRIVATE -W4 -WX -experimental:external -external:anglebrackets -external:W0)
else()
	target_compile_options(bollapp PRIVATE -Wall -Werror)
endif()
