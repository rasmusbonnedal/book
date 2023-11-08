add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE doctest/doctest/)

add_library(rapidxml INTERFACE)
target_include_directories(rapidxml INTERFACE rapidxml/)

add_library(utf-cpp INTERFACE)
target_include_directories(utf-cpp INTERFACE utf-cpp/include/)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory(glfw)
target_include_directories(glfw PUBLIC glfw/include)
if (APPLE)
   find_library(COCOA_LIBRARY Cocoa)
   find_library(IOKIT_LIBRARY IOKit)
   target_link_libraries(glfw INTERFACE ${COCOA_LIBRARY} ${IOKIT_LIBRARY})
endif ()
set_target_properties(glfw PROPERTIES FOLDER "ext/GLFW3")
set_target_properties(update_mappings PROPERTIES FOLDER "ext/GLFW3")

# Freetype
add_subdirectory(freetype)
target_compile_options(freetype PRIVATE /wd4244 /wd4267)
set_target_properties(freetype PROPERTIES FOLDER "ext")

# Dear ImGui
add_library(imgui_glfw
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
    imgui/misc/freetype/imgui_freetype.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_glfw PUBLIC imgui imgui/backends)
target_compile_definitions(imgui_glfw PRIVATE IMGUI_ENABLE_FREETYPE)
target_compile_definitions(imgui_glfw PUBLIC IMGUI_DEFINE_MATH_OPERATORS)
target_link_libraries(imgui_glfw glfw ${OPENGL_LIBRARY} freetype)
set_target_properties(imgui_glfw PROPERTIES FOLDER "ext")

# https://github.com/mlabbe/nativefiledialog
add_library(nativefiledialog
    nativefiledialog/src/nfd_common.c
    nativefiledialog/src/nfd_win.cpp
)
target_compile_definitions(nativefiledialog PRIVATE UNICODE=1 _UNICODE=1)
target_include_directories(nativefiledialog PUBLIC nativefiledialog/src/include)
target_compile_definitions(nativefiledialog PRIVATE -D_CRT_SECURE_NO_WARNINGS)
#target_compile_definitions(nativefiledialog PUBLIC -DFTG_IGNORE_UNICODE)
set_target_properties(nativefiledialog PROPERTIES FOLDER "ext")
