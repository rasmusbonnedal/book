file(GLOB SRCS *.cpp *.h)

find_package(OpenGL REQUIRED)

add_executable(bollapp-imgui ${SRCS})
target_link_libraries(bollapp-imgui
    imgui_glfw
    OpenGL::GL
    nativefiledialog
    bolldoc
)
