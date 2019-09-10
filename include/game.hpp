#pragma once
#include "board.hpp"

class Game
{
public:
    Game(int size);
    void operator()(int depth);

private:
    Board board;
    int board_size;
    int current_player;

    int get_player_input();
};