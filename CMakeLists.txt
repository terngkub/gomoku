cmake_minimum_required(VERSION 3.10)
project(gomoku)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(gomoku
        include/board.hpp
        include/flag.hpp
		include/index.hpp
        include/game.hpp
        include/minimax.hpp
        src/board.cpp
        src/flag.cpp
		src/index.cpp
        src/game.cpp
        src/main.cpp
        src/minimax.cpp)
