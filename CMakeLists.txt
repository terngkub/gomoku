cmake_minimum_required(VERSION 3.10)
project(gomoku)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3")

include_directories(include)

add_executable(gomoku
        include/action.hpp
        include/board.hpp
        include/flag.hpp
		include/index.hpp
        include/game.hpp
        include/minimax.hpp
        include/sequence.hpp
        src/action.cpp
        src/board.cpp
        src/flag.cpp
		src/index.cpp
        src/game.cpp
        src/main.cpp
        src/minimax.cpp
        src/sequence.cpp)
