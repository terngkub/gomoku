#pragma once
#include "board.hpp"
#include <bitset>
#include <unordered_map>
#include <vector>

class Minimax
{
public:
    Minimax(int depth, int ai);
    int operator()(Board & board);

private:
    int const max_depth;
    int const ai;
    int best_action;
    int complexity;

    int minimax(Board & board, int player, int depth, int alpha, int beta);
};