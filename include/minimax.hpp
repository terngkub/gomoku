#pragma once
#include "board.hpp"
#include <bitset>
#include <unordered_map>
#include <vector>

struct BoardHash
{
    std::size_t operator()(std::vector<int> const & indexes) const
    {
        std::size_t hash = 0;
        for (auto index : indexes)
            hash = hash * 3 + index;
        return hash;
    }
};

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
    std::unordered_map<std::vector<int>, int, BoardHash> visited_map;
    int complexity;

    int minimax(int player, int depth, int alpha, int beta);
};