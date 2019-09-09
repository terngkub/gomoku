#pragma once
#include "board.hpp"
#include <vector>

#define MINIMAX_DEPTH 9

class Minimax
{
public:
    Minimax(Board board, int ai);
    int operator()();

private:
    Board board;
    int const ai;
    int best_action;

    int minimax(int player, int depth, int alpha, int beta);
};