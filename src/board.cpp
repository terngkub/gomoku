#include "board.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <unordered_map>

// ****** Constructor ******

Board::Board(int width) :
	width{width},
	size{width * width},
	indexes(size, 0),
	bitset{},
	condition{Condition::Playing},
	heuristic{0},
	valid_one{size / 2},
	valid_two{size / 2}
{}

// ****** Getters ******

int Board::get_heuristic(int player) const
{
	return player == 1 ? heuristic : -heuristic;
}

Condition Board::get_condition() const
{
	return condition;
}

std::bitset<722> Board::get_bitset() const
{
	return bitset;
}


// ****** Checkers ******

bool Board::is_valid(int index, int player) const
{
	//return player == 1 ? valid_one.find(index) != valid_one.end() : valid_two.find(index) != valid_two.end();
	(void)player;
	return indexes[index] == 0;
}

bool Board::is_end(int player) const
{
	if (condition != Condition::Playing)
		return true;
	return player == 1 ? valid_one.size() == 0 : valid_two.size() == 0;
}


// ****** Public Methods ******

Board Board::play(int index, int player)
{
	Board new_board{*this};
	new_board.indexes[index] = player;
	if (player == 1)
		new_board.bitset[index * 2] = 1;
	else
		new_board.bitset[index * 2 + 1] = 1;
	new_board.update_valid(index, player);
	new_board.update_heuristic(index, player);
	return new_board;
}

std::set<int> const & Board::get_nexts(int player) const
{
	return player == 1 ? valid_one : valid_two;
}

void Board::print() const
{
	print_board_indexes_row();
	print_board_pieces();
	print_board_indexes_row();
}


// ****** Printer ******

void Board::print_board_pieces() const
{
	for (auto i = 0; i < int(indexes.size()); ++i)
	{
		// print board indexes on the left
		if		(i % width == 0)			std::cout << ((i / width + 1) % 10) << " ";

		// print board pieces
		if		(indexes[i] == 1) 			std::cout << "o";
		else if	(indexes[i] == 2) 			std::cout << "x";
		else								std::cout << "-";

		// print board indexes on the right
		if		(i % width != width - 1)	std::cout << " ";
		else								std::cout << " " << ((i / width + 1) % 10) << "\n";
	}
}

void Board::print_board_indexes_row() const
{
	std::cout << "  ";
	for (auto i = 1; i <= width; ++i)
	{
		std::cout << (i % 10);
		if (i != width)
			std::cout << " ";
	}
	std::cout << "\n";
}


// ****** Valid Spots ******

void Board::update_valid(int index, int player)
{
	remove_valid(index);
	update_valid_top(index, player);
	update_valid_middle(index, player);
	update_valid_bottom(index, player);
}

void Board::remove_valid(int index)
{
	if (valid_one.find(index) != valid_one.end())
		valid_one.erase(index);
	if (valid_two.find(index) != valid_two.end())
		valid_two.erase(index);
}

void Board::update_valid_top(int index, int player)
{
	int top_mid = index - width;

	if (top_mid < 0)
		return;

	// middle
	insert_valid(top_mid, player);

	// left - if not on the left border
	if (index % width != 0)
		insert_valid(top_mid - 1, player);

	// right - if not on the right border
	if (index % width != width - 1)
		insert_valid(top_mid + 1, player);
}

void Board::update_valid_middle(int index, int player)
{
	// left - if not on the left border
	if (index % width != 0)
		insert_valid(index - 1, player);

	// right - if not on the right border
	if (index % width != width - 1)
		insert_valid(index + 1, player);
}

void Board::update_valid_bottom(int index, int player)
{
	int bot_mid = index + width;

	if (bot_mid >= size)
		return;

	// middle
	insert_valid(bot_mid, player);

	// left - if not on the left border
	if (index % width != 0)
		insert_valid(bot_mid - 1, player);

	// right - if not on the right border
	if (index % width != width - 1)
		insert_valid(bot_mid + 1, player);
}

void Board::insert_valid(int index, int player)
{
	(void)player;
	if (indexes[index] == 0 && valid_one.find(index) == valid_one.end())
		valid_one.insert(index);
	if (indexes[index] == 0 && valid_two.find(index) == valid_two.end())
		valid_two.insert(index);
}


// ****** Heuristic ******

void Board::update_heuristic(int index, int player)
{
	update_heuristic_sequence<IndexX>(index, player);
	update_heuristic_sequence<IndexY>(index, player);
	update_heuristic_sequence<IndexU>(index, player);
	update_heuristic_sequence<IndexD>(index, player);
}

void Board::update_heuristic_delta(Sequence & one, Sequence & two, int player)
{
	// increase my score
	auto space_one = two.len_me > 0 ? 1 : two.space_me + 1;
	auto space_two = one.len_me > 0 ? 1 : one.space_me + 1;
	auto old_me = get_score(one.len_me, one.space_me, space_one) + get_score(two.len_me, two.space_me, space_two);
	auto new_me = get_score(one.len_me + two.len_me + 1, one.space_me, two.space_me);
	auto delta_me = new_me - old_me;

	// reduce opponent score
	auto old_op = get_score(one.len_op, one.space_op_one + two.space_op_one + 1, one.space_op_two) + get_score(two.len_op, two.space_op_one + one.space_op_one + 1, two.space_op_two);
	auto new_op = get_score(one.len_op, one.space_op_one, one.space_op_two) + get_score(two.len_op, two.space_op_one, two.space_op_two);
	auto delta_op = new_op - old_op;

	// set winning condition
	if (one.len_me + two.len_me + 1 > 4)
		condition = player == 1 ? Condition::PlayerOneWin : Condition::PlayerTwoWin;

	// update delta
	if (player == 1)
		heuristic += (delta_me - delta_op);
	else
		heuristic -= (delta_me - delta_op);
}

int Board::get_score(int len, int space_one, int space_two)
{
	static const std::unordered_map<int, int> len_score
	{
		{0, 0},
		{1, 0},
		{2, 10},
		{3, 100},
		{4, 1000},
		{5, 10000}
	};
	auto space_max = 5 - len;
	auto s1 = space_one < space_max ? space_one : space_max;
	auto s2 = space_two < space_max ? space_two : space_max;
	if (len + s1 + s2 < 5)
		return 0;
	auto space_bonus = s1 != 0 && s2 != 0 ? 2 : 1;
	if (len < 5)
		return len_score.at(len) * space_bonus;
	return len_score.at(5);
}
