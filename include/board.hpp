#pragma once
#include <array>
#include <vector>

#define BOARD_SIZE 361

class Board
{
public:
    Board();
    void play(int action, int player);
    void redo(int action, int player);
    int heuristic(int player, int depth_score) const;
    std::vector<int> next(int player) const;
    void print() const;
    bool is_empty_spot(int spot) const;

    bool is_end() const;
    bool is_full() const;
    bool is_win(int player) const;
    int detect_horizontal(int player, int index) const;
    int detect_vertical(int player, int index) const;

private:
    std::vector<int> spots;
};