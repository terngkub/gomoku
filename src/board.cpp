#include "board.hpp"
#include <algorithm>
#include <iostream>

Board::Board() :
	spots(BOARD_SIZE, 0)
{}

void Board::play(int action, int player)
{
	(void)player;
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

bool Board::is_empty_spot(int spot) const
{
	return spots[spot] == 0;
}

bool Board::is_full() const
{
	return std::all_of(spots.begin(), spots.end(), [](int spot){ return spot != 0; });
}

bool Board::is_win() const
{
	// vertical
	if (spots[0] != 0 && spots[0] == spots[3] && spots[0] == spots[6]) return true;
	if (spots[1] != 0 && spots[1] == spots[4] && spots[1] == spots[7]) return true;
	if (spots[2] != 0 && spots[2] == spots[5] && spots[2] == spots[8]) return true;

	// horizontal
	if (spots[0] != 0 && spots[0] == spots[1] && spots[0] == spots[2]) return true;
	if (spots[3] != 0 && spots[3] == spots[4] && spots[3] == spots[5]) return true;
	if (spots[6] != 0 && spots[6] == spots[7] && spots[6] == spots[8]) return true;

	// diagonal
	if (spots[0] != 0 && spots[0] == spots[4] && spots[0] == spots[8]) return true;
	if (spots[2] != 0 && spots[2] == spots[4] && spots[2] == spots[6]) return true;

	return false;
}

bool Board::is_win(int player) const
{
	// vertical
	if (spots[0] == player && spots[0] == spots[3] && spots[0] == spots[6]) return true;
	if (spots[1] == player && spots[1] == spots[4] && spots[1] == spots[7]) return true;
	if (spots[2] == player && spots[2] == spots[5] && spots[2] == spots[8]) return true;

	// horizontal
	if (spots[0] == player && spots[0] == spots[1] && spots[0] == spots[2]) return true;
	if (spots[3] == player && spots[3] == spots[4] && spots[3] == spots[5]) return true;
	if (spots[6] == player && spots[6] == spots[7] && spots[6] == spots[8]) return true;

	// diagonal
	if (spots[0] == player && spots[0] == spots[4] && spots[0] == spots[8]) return true;
	if (spots[2] == player && spots[2] == spots[4] && spots[2] == spots[6]) return true;

	return false;
}

void Board::print() const
{
	for (size_t i = 0; i < spots.size(); ++i)
	{
		if		(spots[i] == 1) std::cout << "o";
		else if	(spots[i] == 2) std::cout << "x";
		else					std::cout << "-";

		if		(i % 3 != 2)	std::cout << " ";
		else					std::cout << "\n";
	}
}