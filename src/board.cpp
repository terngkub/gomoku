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

std::set<int> Board::next(int player) const
{
	(void)player;
	std::set<int> nexts{};
	for (int i = 0; i < int(spots.size()); ++i)
	{
		if (spots[i] != 0)
		{
			auto tl = i - size - 1;
			auto tm = i - size;
			auto tr = i - size + 1;
			auto ml = i - 1;
			auto mr = i + 1;
			auto bl = i + size - 1;
			auto bm = i + size;
			auto br = i + size + 1;

			if (tl > 0 && spots[tl] == 0) nexts.insert(tl);
			if (tm > 0 && spots[tm] == 0) nexts.insert(tm);
			if (tr > 0 && spots[tr] == 0) nexts.insert(tr);
			if (ml > 0 && spots[ml] == 0) nexts.insert(ml);
			if (mr < int(spots.size()) && spots[mr] == 0) nexts.insert(mr);
			if (bl < int(spots.size()) && spots[bl] == 0) nexts.insert(bl);
			if (bm < int(spots.size()) && spots[bm] == 0) nexts.insert(bm);
			if (br < int(spots.size()) && spots[br] == 0) nexts.insert(br);
		}
	}
	if (nexts.size() == 0)
	{
		if (std::all_of(spots.begin(), spots.end(), [](int spot){ return spot == 0; }))
			nexts.insert((size - 1) * size / 2 + size / 2);
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
	for (int i = 0; i < int(spots.size()); ++i)
	{
		if (spots[i] != player) continue;
		if (detect_horizontal(player, i) == 5 || detect_vertical(player, i) == 5)
			return true;
	}
	return false;
}

int Board::detect_horizontal(int player, int index) const
{
	int count = 1;
	if (index > 0 && spots[index - 1] == player)
		return 0;
	for (int i = 1; i < 5 && index + i < int(spots.size()) && spots[index + i] == player; ++i)
		++count;
	return count;
}

int Board::detect_vertical(int player, int index) const
{
	int count = 1;
	if (index >= size && spots[index - size] == player)
		return 0;
	for (int i = 1; i < 5 && index + i * size < int(spots.size()) && spots[index + i * size] == player; ++i)
		++count;
	return count;
}
