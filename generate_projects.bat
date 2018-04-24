@echo off
mkdir build
pushd build
set CMAKE_PREFIX_PATH="C:/Qt/5.10.1/msvc2017_64/"
rem set CMAKE_PREFIX_PATH=
cmake -G "Visual Studio 15 2017 Win64" ..
popd
