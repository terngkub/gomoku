#pragma once
#include "board.hpp"
#include <bitset>
#include <unordered_map>
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
    std::unordered_map<std::bitset<722>, int> visited_map;
    int complexity;

    int minimax(int player, int depth, int alpha, int beta);
};