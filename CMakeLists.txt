cmake_minimum_required(VERSION 3.25)
project(GameController)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Modelec COMPONENTS
        TCPClient
        Utils
        CLParser
        REQUIRED
)

add_executable(GameController
        main.cpp
        GameControllerHandler.h)

target_link_libraries(GameController ${SDL2_LIBRARIES})
target_link_libraries(GameController Modelec::TCPClient Modelec::Utils Modelec::CLParser)
