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

add_executable(bollapp-imgui main.cpp)
target_link_libraries(bollapp-imgui bollapp-imgui-lib)

add_executable(test-combo test-combo.cpp)
target_link_libraries(test-combo bollapp-imgui-lib)
