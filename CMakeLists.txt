cmake_minimum_required(VERSION 3.14)
project(gomoku)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(gomoku
        include/board.hpp
        include/flag.hpp
        include/game.hpp
        include/minimax.hpp
        src/board.cpp
        src/flag.cpp
        src/game.cpp
        src/main.cpp
        src/minimax.cpp)
