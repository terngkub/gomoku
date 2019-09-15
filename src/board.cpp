#include "board.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <unordered_map>

Board::Board(int size) :
	size{size},
	spots(size * size, 0),
	condition{Condition::Playing},
	history{},
	valid_spots{},
	heuristic{0}
{}

void Board::play(int index, int player)
{
	spots[index] = player;

	Action action;
	action.play = index;

	// TODO move this to function and send action as ref
	action.play_on_valid_spot = false;
	if (valid_spots.find(index) != valid_spots.end())
	{
		action.play_on_valid_spot = true;
		valid_spots.erase(index);
	}
	action.valid_spots = update_valid_spots(index, player);

	update_heuristic(index, player, action);
	history.push(action);
}

void Board::undo()
{
	auto action = history.top();
	history.pop();
	spots[action.play] = 0;
	if (action.play_on_valid_spot)
		valid_spots.insert(action.play);
	if (action.is_end)
		condition = Condition::Playing;
	undo_valid_spots(action.valid_spots);
	undo_heuristic(action);
}


// ****** Valid Spots ******

std::set<int> Board::update_valid_spots(int index, int player)
{
	(void)player;
	std::set<int> actions;


	// top >> (middle, left, right)
	if (index / size != 0)
	{
		insert_if_valid(actions, index - size);
		if (index % size != 0) insert_if_valid(actions, index - size - 1);
		if (index % size != size - 1) insert_if_valid(actions, index - size + 1);
	}

	// middle (left, right)
	if (index % size != 0) insert_if_valid(actions, index - 1);
	if (index % size != size - 1) insert_if_valid(actions, index + 1);

	// bottom >> (middle, left, right)
	if (index / size != size - 1)
	{
		insert_if_valid(actions, index + size);
		if (index % size != 0) insert_if_valid(actions, index + size - 1);
		if (index % size != size - 1) insert_if_valid(actions, index + size + 1);
	}

	return actions;
}

void Board::insert_if_valid(std::set<int> & actions, int index)
{
	if (spots[index] == 0 && valid_spots.find(index) == valid_spots.end())
	{
		valid_spots.insert(index);
		actions.insert(index);
	}
}

void Board::undo_valid_spots(std::set<int> const & action_valid_spots)
{
	for (auto spot : action_valid_spots)
		valid_spots.erase(spot);
}

int Board::get_heuristic(int player) const
{
	(void)player;
	return heuristic;
}

std::set<int> const & Board::next(int player) const
{
	(void)player;
	return valid_spots;
}

void Board::print() const
{
	std::cout << "  ";
	for (auto i = 1; i <= size; ++i)
	{
		std::cout << (i % 10);
		if (i != size)
			std::cout << " ";
	}
	std::cout << "\n";

	for (auto i = 0; i < int(spots.size()); ++i)
	{
		if		(i % size == 0)			std::cout << ((i / size + 1) % 10) << " ";
		if		(spots[i] == 1) 		std::cout << "o";
		else if	(spots[i] == 2) 		std::cout << "x";
		else							std::cout << "-";

		if		(i % size != size - 1)	std::cout << " ";
		else							std::cout << " " << ((i / size + 1) % 10) << "\n";
	}

	std::cout << "  ";
	for (auto i = 1; i <= size; ++i)
	{
		std::cout << (i % 10);
		if (i != size)
			std::cout << " ";
	}
	std::cout << "\n";

	std::cout << "heuristic: " << heuristic << "\n";
}

bool Board::is_first_turn() const
{
	return history.size() == 0;
}

bool Board::is_valid_spot(int index) const
{
	return spots[index] == 0;
}

bool Board::is_end() const
{
	// TODO has to check that there is no valid spot left
	return condition != Condition::Playing;
}

Condition Board::get_condition() const
{
	return condition;
}


// ****** Heuristic ******

void Board::update_heuristic(int index, int player, Action & action)
{
	update_heuristic_sequence<IndexX>(index, player, action);
	update_heuristic_sequence<IndexY>(index, player, action);
	update_heuristic_sequence<IndexU>(index, player, action);
	update_heuristic_sequence<IndexD>(index, player, action);
}

int get_score(int len, int space_one, int space_two)
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

void Board::update_heuristic_delta(SequenceInfo & one, SequenceInfo & two, int player, Action & action)
{
	// increase my score
	auto space_one = two.len_me > 0 ? 1 : two.space_me + 1;
	auto space_two = one.len_me > 0 ? 1 : one.space_me + 1;
	auto old_me = get_score(one.len_me, one.space_me, space_one) + get_score(two.len_me, two.space_me, space_two);
	auto new_me = get_score(one.len_me + two.len_me + 1, one.space_me, two.space_me);
	auto delta_me = new_me - old_me;
	// zero or positive

	// reduce opponent score
	auto old_op = get_score(one.len_op, one.space_op_one + two.space_op_one + 1, one.space_op_two) + get_score(two.len_op, two.space_op_one + one.space_op_one + 1, two.space_op_two);
	auto new_op = get_score(one.len_op, one.space_op_one, one.space_op_two) + get_score(two.len_op, two.space_op_one, two.space_op_two);
	auto delta_op = new_op - old_op;
	// zero or negative

	if (one.len_me + two.len_me + 1 > 4)
	{
		action.is_end = true;
		condition = player == 1 ? Condition::PlayerOneWin : Condition::PlayerTwoWin;
	}

	// std::cout << "update before " << heuristic << "\n";
	// std::cout << "delta " << delta_me + delta_op << "\n";
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
	// std::cout << "update after " << heuristic << "\n";
}

void Board::undo_heuristic(Action const & action)
{
	// std::cout << "undo before " << heuristic << "\n";
	heuristic -= action.delta_heuristic;
	// std::cout << "undo after " << heuristic << "\n";
}