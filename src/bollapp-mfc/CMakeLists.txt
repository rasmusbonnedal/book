#add_subdirectory(tests)

set(SRCS
    bollapp.rc
    bollframe.cpp
    bollframe.h
    bollview.cpp
    bollview.h
    main.cpp
    mfcbolldoc.cpp
    mfcbolldoc.h
    mfcstring.h
)

add_definitions(-D_AFXDLL -D_WIN32_WINNT=_WIN32_WINNT_WIN10)
add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_MFC_FLAG 2)
add_executable(bollapp WIN32 ${SRCS})
target_link_libraries(bollapp bolldoc)
target_include_directories(bollapp PUBLIC .)
if(MSVC)
    target_compile_options(bollapp PRIVATE -W4 -WX -experimental:external -external:anglebrackets -external:W0)
else()
    target_compile_options(bollapp PRIVATE -Wall -Werror)
endif()
