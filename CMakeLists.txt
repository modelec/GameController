cmake_minimum_required(VERSION 3.25)
project(GameController)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(TCPSocket REQUIRED TCPSocket)

add_executable(GameController
        main.cpp
        utils.h
        GameControllerHandler.h)

target_link_libraries(GameController ${SDL2_LIBRARIES})
target_link_libraries(GameController TCPSocket)
