cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(book)

enable_testing()
add_subdirectory(ext)
add_subdirectory(src)

if (WIN32)
    set (CMAKE_PREFIX_PATH "C:\\Qt\\5.10.1\\msvc2017_64\\")
elseif (APPLE AND EXISTS /usr/local/opt/qt5)
    list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)
find_package(Qt5Xml)

add_executable(book WIN32 main.cpp)

target_link_libraries(book Qt5::Widgets Qt5::Xml)

if (WIN32)
	get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
	get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
	set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")
	add_custom_command(TARGET book POST_BUILD
		COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --no-opengl-sw --no-translations $<TARGET_FILE_DIR:book>)
endif()
