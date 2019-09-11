#include "board.hpp"
#include <algorithm>
#include <iostream>

Board::Board(int size) :
	size{size},
	spots(size * size, 0)
{}

void Board::play(int index, int player)
{
	spots[index] = player;

	Action action;
	action.play = index;
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
	undo_valid_spots(action.valid_spots);
	undo_seq_x(action.seq_x);
	undo_seq_y(action.seq_y);
}


// ****** Valid Spots ******

// TODO
std::set<int> Board::update_valid_spots(int index, int player)
{
	// for each valid spot
	// if empty and if not there
	// insert into valid spot
	// insert into action

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
	while (spots[head] == player && head > row_start)
		++head;
	return head;
}

// TODO
int Board::get_head_y(int index) const
{
}

SequenceAction Board::update_seq_x(int index)
{
	int head = get_head_x(index);

	SequenceAction seq_action;
	seq_action.head_index = head;

	if (seq_x.find(index + 1) != seq_x.end
			&& (index + 1) % size != 0
			&& spots[index] == spots[index + 1])
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

// TODO
SequenceAction Board::update_seq_y(int index)
{
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

// TODO
int Board::heuristic(int player, int depth_score) const
{
	// apart from length, seq_x should also store availability of before and after
}

std::set<int> const & Board::next(int player) const
{
	return valid_spots;
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

bool Board::is_valid_spot(int index) const
{
	return spots[index] == 0;
}

bool Board::is_end() const
{
	return condition != Condition::Playing || valid_spots.size() == 0;
}

Condition Board::get_condition() const
{
	return condition;
}