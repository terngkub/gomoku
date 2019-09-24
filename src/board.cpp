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
	heuristic{0},
	condition{Condition::Playing},
	indexes(size, 0),
	valids{},
	history{}
{}


// ****** Getters ******

int Board::get_size() const
{
	return size;
}

Condition Board::get_condition() const
{
	return condition;
}

int Board::get_heuristic(int player) const
{
	return player == 1 ? heuristic : -heuristic;
}

std::vector<int> const & Board::get_indexes() const
{
	return indexes;
}

std::set<int> const & Board::get_nexts(int player) const
{
	(void)player;
	return valids;
}


// ****** Checkers ******

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


// ****** Public Methods ******

void Board::play(int index, int player)
{
	update_indexes(index, player);
	update_valids(index, player);
	update_heuristic(index, player);
	save_history();
}

void Board::undo()
{
	auto action = load_history();
	undo_indexes(action);
	undo_valids(action);
	undo_heuristic(action);
	undo_condition(action);
}

void Board::print() const
{
	print_index();
	print_board();
	print_index();
}


// ****** History ******

void Board::save_history()
{
	history.push(current_action);
	current_action = Action{};
}

Action Board::load_history()
{
	auto action = history.top();
	history.pop();
	return action;
}


// ****** Indexes ******

void Board::update_indexes(int index, int player)
{
	indexes[index] = player;
	current_action.index = index;
	current_action.player = player;
}

void Board::undo_indexes(Action const & action)
{
	indexes[action.index] = 0;
}


// ****** Valid indexes ******

void Board::update_valids(int index, int player)
{
	if (valids.find(index) != valids.end())
	{
		current_action.play_on_valid_spot = true;
		valids.erase(index);
	}
	update_valids_top(index, player);
	update_valids_middle(index, player);
	update_valids_bottom(index, player);
}

void Board::update_valids_top(int index, int player)
{
	(void)player;
	int top = index - width;
	if (top >= 0)
	{
		insert_valid(top);
		insert_left_right(index, top);
	}
}

void Board::update_valids_middle(int index, int player)
{
	(void)player;
	insert_left_right(index, index);
}

void Board::update_valids_bottom(int index, int player)
{
	(void)player;
	int bottom = index + width;
	if (bottom < size)
	{
		insert_valid(bottom);
		insert_left_right(index, bottom);
	}
}

void Board::insert_left_right(int based_index, int inserted_index)
{
	int col = based_index % width;
	if (col != 0)
		insert_valid(inserted_index - 1);
	if (col != width - 1)
		insert_valid(inserted_index + 1);
}

void Board::insert_valid(int index)
{
	if (indexes[index] == 0 && valids.find(index) == valids.end())
	{
		valids.insert(index);
		current_action.valids.insert(index);
	}
}

void Board::undo_valids(Action const & action)
{
	if (action.play_on_valid_spot)
		valids.insert(action.index);
	for (auto index : action.valids)
		valids.erase(index);
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

	if (one.len_me + two.len_me + 1 > 4)
	{
		current_action.is_end = true;
		condition = player == 1 ? Condition::PlayerOneWin : Condition::PlayerTwoWin;
	}

	if (player == 1)
	{
		heuristic += (delta_me - delta_op);
		current_action.delta_heuristic += (delta_me - delta_op);
	}
	else
	{
		heuristic -= (delta_me - delta_op);
		current_action.delta_heuristic -= (delta_me - delta_op);
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


// ****** Condition ******

void Board::undo_condition(Action const & action)
{
	if (action.is_end)
		condition = Condition::Playing;
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