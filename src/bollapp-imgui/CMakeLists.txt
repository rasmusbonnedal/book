file(GLOB SRCS *.cpp *.h)
list(REMOVE_ITEM SRCS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
list(REMOVE_ITEM SRCS ${CMAKE_CURRENT_SOURCE_DIR}/test-combo.cpp)

find_package(OpenGL REQUIRED)

add_library(bollapp-imgui-lib STATIC ${SRCS})
target_link_libraries(bollapp-imgui-lib
    imgui_glfw
    OpenGL::GL
    nativefiledialog
    bolldoc
    reports
    sieparse
)
target_compile_definitions(bollapp-imgui-lib PUBLIC _CRT_SECURE_NO_WARNINGS=1)

add_executable(bollbok main.cpp appicon.rc)
target_link_libraries(bollbok bollapp-imgui-lib)
install(TARGETS bollbok DESTINATION .)

add_executable(test-combo test-combo.cpp)
target_link_libraries(test-combo bollapp-imgui-lib)

function(copy_install_file file destdir)
    install(FILES ${file} DESTINATION ${destdir})
    add_custom_command(TARGET bollbok POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${file} $<TARGET_FILE_DIR:bollbok>/${destdir}
                COMMAND_EXPAND_LISTS)
endfunction()

copy_install_file(${CMAKE_CURRENT_SOURCE_DIR}/moms_mapping.json .)
