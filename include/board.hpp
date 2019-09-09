#pragma once
#include <array>
#include <vector>

#define BOARD_SIZE 9

class Board
{
public:
    Board();
    void play(int action, int player);
    void redo(int action, int player);
    int heuristic(int player, int depth_score) const;
    std::vector<int> next(int player) const;
    bool is_empty_spot(int spot) const;
    bool is_full() const;
    bool is_win() const;
    bool is_win(int player) const;
    void print() const;

    std::vector<int> spots;
};