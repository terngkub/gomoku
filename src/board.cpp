#include "board.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <unordered_map>

// Constructor

Board::Board(int width) :
	width{width},
	size{width * width},
	heuristic{0},
	condition{Condition::Playing},
	indexes(size, 0),
	bs{},
	valids{},
	history{}
{}


// Getters

Condition Board::get_condition() const
{
	return condition;
}

int Board::get_heuristic(int player) const
{
	return player == 1 ? heuristic : -heuristic;
}

std::bitset<722> const & Board::get_bitset() const
{
	return bs;
}

std::set<int> const & Board::get_nexts(int player) const
{
	(void)player;
	return valids;
}


// Checkers

bool Board::is_first_turn() const
{
	return history.size() == 0;
}

bool Board::is_valid_spot(int index) const
{
	return indexes[index] == 0;
}

bool Board::is_end() const
{
	return condition != Condition::Playing;
}


// Public Methods

void Board::play(int index, int player)
{
	indexes[index] = player;
	(player == 1) ? bs.set(index * 2 + 1) : bs.set(index * 2);

	Action action;
	action.play = index;

	update_heuristic(index, player, action);

	action.play_on_valid_spot = false;
	if (valids.find(index) != valids.end())
	{
		action.play_on_valid_spot = true;
		valids.erase(index);
	}
	action.valids = update_valids(index, player);

	history.push(action);
}

void Board::undo()
{
	auto action = history.top();
	history.pop();
	indexes[action.play] = 0;
	bs.reset(action.play * 2);
	bs.reset(action.play * 2 + 1);
	if (action.play_on_valid_spot)
		valids.insert(action.play);
	if (action.is_end)
		condition = Condition::Playing;
	undo_valids(action.valids);
	undo_heuristic(action);
}

void Board::print() const
{
	print_index();
	print_board();
	print_index();
}


// ****** Valid indexes ******

std::set<int> Board::update_valids(int index, int player)
{
	(void)player;
	std::set<int> actions;

	// top >> (middle, left, right)
	if (index / width != 0)
	{
		insert_if_valid(actions, index - width);
		if (index % width != 0) insert_if_valid(actions, index - width - 1);
		if (index % width != width - 1) insert_if_valid(actions, index - width + 1);
	}

	// middle (left, right)
	if (index % width != 0) insert_if_valid(actions, index - 1);
	if (index % width != width - 1) insert_if_valid(actions, index + 1);

	// bottom >> (middle, left, right)
	if (index / width != width - 1)
	{
		insert_if_valid(actions, index + width);
		if (index % width != 0) insert_if_valid(actions, index + width - 1);
		if (index % width != width - 1) insert_if_valid(actions, index + width + 1);
	}

	return actions;
}

void Board::insert_if_valid(std::set<int> & actions, int index)
{
	if (indexes[index] == 0 && valids.find(index) == valids.end())
	{
		valids.insert(index);
		actions.insert(index);
	}
}

void Board::undo_valids(std::set<int> const & action_valids)
{
	for (auto spot : action_valids)
		valids.erase(spot);
}


// ****** Heuristic ******

void Board::update_heuristic(int index, int player, Action & action)
{
	update_heuristic_sequence<IndexX>(index, player, action);
	update_heuristic_sequence<IndexY>(index, player, action);
	update_heuristic_sequence<IndexU>(index, player, action);
	update_heuristic_sequence<IndexD>(index, player, action);
}

void Board::update_heuristic_delta(Sequence & one, Sequence & two, int player, Action & action)
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

	if (one.len_me + two.len_me + 1 > 4)
	{
		action.is_end = true;
		condition = player == 1 ? Condition::PlayerOneWin : Condition::PlayerTwoWin;
	}

	if (player == 1)
	{
		heuristic += (delta_me - delta_op);
		action.delta_heuristic += (delta_me - delta_op);
	}
	else
	{
		heuristic -= (delta_me - delta_op);
		action.delta_heuristic -= (delta_me - delta_op);
	}
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

void Board::undo_heuristic(Action const & action)
{
	heuristic -= action.delta_heuristic;
}


// ****** Printer ******

void Board::print_index() const
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

void Board::print_board() const
{
	for (auto i = 0; i < int(indexes.size()); ++i)
	{
		if		(i % width == 0)			std::cout << ((i / width + 1) % 10) << " ";
		if		(indexes[i] == 1) 			std::cout << "o";
		else if	(indexes[i] == 2) 			std::cout << "x";
		else								std::cout << "-";

		if		(i % width != width - 1)	std::cout << " ";
		else								std::cout << " " << ((i / width + 1) % 10) << "\n";
	}
}