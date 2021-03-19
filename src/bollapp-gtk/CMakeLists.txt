find_package (PkgConfig REQUIRED)
pkg_check_modules (GTKMM REQUIRED gtkmm-3.0)

set(SRCS
    bollbok_app.cpp
    cellrenderertextcompletion.cpp
    grundbok_view.cpp
    main.cpp
    main_window.cpp
    platform.cpp
    platform.h
    report_dialog.cpp
    report_dialog.h
    saldo_view.cpp
    saldo_view.h
    treeview_enable_edit_next_field.cpp
    treeview_enable_valid.cpp
    verifikat_view.cpp
)

execute_process(
    COMMAND git describe --always HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE res
    OUTPUT_VARIABLE MYAPP_VERSION
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)

set_property(GLOBAL APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/.git/index")

add_executable(bollapp ${SRCS})
target_link_libraries(bollapp bolldoc reports ${GTKMM_LIBRARIES})
target_include_directories(bollapp PUBLIC . ${GTKMM_INCLUDE_DIRS})
add_definitions(-DGIT_VERSION=\"${MYAPP_VERSION}\")

if(MSVC)
    target_compile_options(bollapp PRIVATE -W4 -WX -experimental:external -external:anglebrackets -external:W0)
else()
    target_compile_options(bollapp PRIVATE -Wall -Werror)
    target_compile_options(bollapp PRIVATE -Wno-deprecated-declarations)
endif()

add_custom_command(TARGET bollapp POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${CMAKE_CURRENT_SOURCE_DIR}/icon.png
                       $<TARGET_FILE_DIR:bollapp>)

configure_file(bollapp.desktop ~/.local/share/applications/bollapp.desktop)
