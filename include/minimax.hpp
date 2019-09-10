#pragma once
#include "board.hpp"
#include <vector>

class Minimax
{
public:
    Minimax(Board board, int depth, int ai);
    int operator()();

private:
    Board board;
    int const max_depth;
    int const ai;
    int best_action;

    int minimax(int player, int depth, int alpha, int beta);
};