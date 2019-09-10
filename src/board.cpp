#include "board.hpp"
#include <algorithm>
#include <iostream>

Board::Board(int size) :
	size{size},
	spots(size * size, 0)
{}

void Board::play(int action, int player)
{
	spots[action] = player;
}

void Board::redo(int action, int player)
{
	(void)player;
	spots[action] = 0;
}

int Board::heuristic(int player, int depth_score) const
{
	int score = depth_score;
	if (is_win(player)) return (score + 10);
	if (is_win(player ^ 3)) return (-score - 10);
	return 0;
}

std::vector<int> Board::next(int player) const
{
	(void)player;
	std::vector<int> nexts{};
	for (size_t i = 0; i < spots.size(); ++i)
	{
		if (spots[i] == 0)
			nexts.push_back(i);
	}
	return nexts;
}

void Board::print() const
{
	for (size_t i = 0; i < spots.size(); ++i)
	{
		if		(spots[i] == 1) 		std::cout << "o";
		else if	(spots[i] == 2) 		std::cout << "x";
		else							std::cout << "-";

		if		(i % size != size - 1)	std::cout << " ";
		else							std::cout << "\n";
	}
}

bool Board::is_empty_spot(int spot) const
{
	return spots[spot] == 0;
}

bool Board::is_end() const
{
	if (is_full() || is_win(1) || is_win(2))
		return true;
	return false;
}

bool Board::is_full() const
{
	return std::all_of(spots.begin(), spots.end(), [](int spot){ return spot != 0; });
}

bool Board::is_win(int player) const
{
	for (int i = 1; i < int(spots.size()); ++i)
	{
		if (detect_horizontal(player, i) == 5 || detect_vertical(player, i) == 5)
			return true;
	}
	return false;
}

int Board::detect_horizontal(int player, int index) const
{
	int count = 1;
	if (index >= 1 && spots[index - 1] == player)
		return 0;
	for (int i = 1; i <= 5 && index + i < 360 && spots[index + i] == player; ++i)
		++count;
	return count;
}

int Board::detect_vertical(int player, int index) const
{
	int count = 1;
	if (index >= 19 && spots[index - 19] == player)
		return 0;
	for (int i = 1; i < 5 && index + i * 19 < 360 && spots[index + i * 19] == player; ++i)
		++count;
	return count;
}
