#pragma once
#include "board.hpp"
#include <vector>

class Minimax
{
public:
    Minimax(Board board, int ai);
    int operator()(int depth);

private:
    Board board;
    int const ai;
    int best_action;

    int minimax(int player, int depth, int alpha, int beta);
};