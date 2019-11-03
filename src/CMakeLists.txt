if (WIN32)
	add_subdirectory(bollapp-mfc)
elseif (UNIX)
	add_subdirectory(bollapp-gtk)
endif ()
add_subdirectory(bolldoc)

file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)
find_program(CLANG_FORMAT NAMES clang-format-8 clang-format)
if (CLANG_FORMAT)
    add_custom_target(
            clang-format
            COMMAND ${CLANG_FORMAT}
            -i
            ${SOURCE_FILES} ${HEADER_FILES}
    )
endif ()
