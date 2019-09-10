#pragma once
#include "board.hpp"

class Game
{
public:
    Game(int size);
    void operator()(int depth);

private:
    Board board;
    int current_player;
};