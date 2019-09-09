#pragma once
#include "board.hpp"

class Game
{
public:
    Game();
    void operator()();

private:
    Board board;
    int current_player;
};