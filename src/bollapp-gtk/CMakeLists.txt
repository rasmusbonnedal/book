find_package (PkgConfig REQUIRED)
pkg_check_modules (GTKMM REQUIRED gtkmm-3.0)

set(SRCS
    bollbok_app.cpp
    cellrenderertextcompletion.cpp
    grundbok_view.cpp
    main.cpp
    main_window.cpp
    treeview_enable_edit_next_field.cpp
    treeview_enable_valid.cpp
    verifikat_view.cpp
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

add_custom_command(TARGET bollapp POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${CMAKE_CURRENT_SOURCE_DIR}/icon.png
                       $<TARGET_FILE_DIR:bollapp>)

configure_file(bollapp.desktop ~/.local/share/applications/bollapp.desktop)
