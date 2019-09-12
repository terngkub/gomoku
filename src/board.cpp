#include "board.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

Board::Board(int size) :
	size{size},
	spots(size * size, 0)
{}

void Board::play(int index, int player)
{
	spots[index] = player;

	Action action;
	action.play = index;
	action.play_on_valid_spot = false;
	if (valid_spots.find(index) != valid_spots.end())
	{
		action.play_on_valid_spot = true;
		valid_spots.erase(index);
	}
	action.valid_spots = update_valid_spots(index, player);
	action.seq_x = update_seq_x(index);
	action.seq_y = update_seq_y(index);
	history.push(action);
}

void Board::undo()
{
	auto action = history.top();
	history.pop();
	spots[action.play] = 0;
	if (action.play_on_valid_spot)
		valid_spots.insert(action.play);
	undo_valid_spots(action.valid_spots);
	undo_seq_x(action.seq_x);
	undo_seq_y(action.seq_y);
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


// ****** Sequence ******

int Board::get_head_x(int index) const
{
	int player = spots[index];
	int row_start = (index / size) * size;
	int head = index;
	while (head >= row_start && spots[head] == player)
		--head;
	++head;
	return head;
}

int Board::get_head_y(int index) const
{
	int player = spots[index];
	int col_start = index % size;
	int head = index;
	while (head >= col_start && spots[head] == player)
		head -= size;
	head += size;
	return head;
}

SequenceAction Board::update_seq_x(int index)
{
	int head = get_head_x(index);

	SequenceAction seq_action;
	seq_action.head_index = head;

	if (index + 1 < int(spots.size())
			&& (index + 1) % size != 0
			&& spots[index] == spots[index + 1]
			&& seq_x.find(index + 1) != seq_x.end())
			// the last condition is actually too much
	{
		seq_action.is_combined = true;
		seq_action.combined_index = index + 1;
		seq_action.combined_length = seq_x[index];

		seq_x.erase(index + 1);
		seq_x[head] += 1 + seq_action.combined_length;
	}
	else
	{
		seq_x[head] += 1;

		seq_action.is_combined = false;
	}

	if (seq_x[head] >= 5)
		condition = (spots[index] == 1) ? Condition::PlayerOneWin : Condition::PlayerTwoWin;

	return seq_action;
}

SequenceAction Board::update_seq_y(int index)
{
	int head = get_head_y(index);

	SequenceAction seq_action;
	seq_action.head_index = head;

	if (index + size < int(spots.size())
			&& (index + size) % size != 0
			&& spots[index] == spots[index + size]
			&& seq_y.find(index + size) != seq_y.end())
			// the last condition is actually too much
	{
		seq_action.is_combined = true;
		seq_action.combined_index = index + size;
		seq_action.combined_length = seq_y[index];

		seq_y.erase(index + size);
		seq_y[head] += 1 + seq_action.combined_length;
	}
	else
	{
		seq_y[head] += 1;

		seq_action.is_combined = false;
	}

	if (seq_y[head] >= 5)
		condition = (spots[index] == 1) ? Condition::PlayerOneWin : Condition::PlayerTwoWin;


	return seq_action;
}

void Board::undo_seq_x(SequenceAction const & seq_action)
{
	if (seq_action.is_combined)
	{
		seq_x[seq_action.combined_index] = seq_action.combined_length;
		seq_x[seq_action.head_index] -= (1 + seq_action.combined_length);
	}
	else
	{
		seq_x[seq_action.head_index] -= 1;
	}
}

void Board::undo_seq_y(SequenceAction const & seq_action)
{
	if (seq_action.is_combined)
	{
		seq_y[seq_action.combined_index] = seq_action.combined_length;
		seq_y[seq_action.head_index] -= (1 + seq_action.combined_length);
	}
	else
	{
		seq_y[seq_action.head_index] -= 1;
	}
}

int Board::heuristic(int player, int depth_score) const
{
	// TODO
	// heuristic calculation is very slow
	// need to update the score on play/undo
	(void)player;
	(void)depth_score;
	int score = 0;
	return score;
}

std::set<int> const & Board::next(int player) const
{
	(void)player;
	return valid_spots;
}

void Board::print() const
{
	for (auto i = 0; i < int(spots.size()); ++i)
	{
		if		(spots[i] == 1) 		std::cout << "o";
		else if	(spots[i] == 2) 		std::cout << "x";
		else							std::cout << "-";

		if		(i % size != size - 1)	std::cout << " ";
		else							std::cout << "\n";
	}

	std::cout << "valid:\n";
	for (auto spot : valid_spots)
		std::cout << "(" << spot / size + 1<< ", " << spot % size + 1<< ") ";
	std::cout << "\n";

	std::cout << "x:\n";
	for (auto & pair : seq_x)
		std::cout << pair.first << " " << pair.second << "\n";
	std::cout << "y:\n";
	for (auto & pair : seq_y)
		std::cout << pair.first << " " << pair.second << "\n";
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