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
	std::cout << "heuristic: " << heuristic << "\n";
	print_index();
	print_board();
	print_index();
	std::cout << "\n";
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

Sequence Board::get_next_sequence(Index & i) const
{
	Sequence seq{};

	if (!i.check())
		return seq;

	seq.player = indexes[i.val()];

	for (; i.check() && indexes[i.val()] == seq.player; i.move())
		++seq.count;

	return seq;
}

SequenceFormat Board::get_sequence_format(std::vector<Sequence> const & seq_vector, int player) const
{
	SequenceFormat sf{};

	if (seq_vector.empty())
		return sf;

	auto it = seq_vector.cbegin();
	auto end = seq_vector.cend();

	// space_one
	if (it->player == 0)
	{
		sf.space_one = it->count;
		++it;
	}
	// len
	if (it != end && it->player == player)
	{
		sf.len = it->count;
		++it;
		// space_two
		if (it != end && it->player == 0)
			sf.space_two = it->count;
	}

	return sf;
}

int Board::calculate_delta(std::vector<Sequence> const & one, std::vector<Sequence> const & two, int player)
{
	auto delta_me = calculate_delta_me(one, two, player);
	auto delta_op = calculate_delta_op(one, two, player);
	return delta_me - delta_op;
}

int Board::calculate_delta_me(std::vector<Sequence> const & vector_one, std::vector<Sequence> const & vector_two, int player)
{
	auto sf_one = get_sequence_format(vector_one, player);
	auto sf_two = get_sequence_format(vector_two, player);

	SequenceFormat cal_one
	{
		sf_one.space_one + sf_two.space_one + 1,
		sf_one.len,
		sf_one.space_two
	};
	SequenceFormat cal_two
	{
		sf_one.space_one + sf_two.space_one + 1,
		sf_two.len,
		sf_two.space_two
	};

	auto score_one = get_score(cal_one);
	auto score_two = get_score(cal_two);

	auto score_new = 0;
	auto score_old = score_one + score_two;

	if (sf_one.space_one != 0)
	{
		// no combine
		if (sf_two.space_one != 0)
		{
			SequenceFormat sf_new
			{
				sf_one.space_one,
				1,
				sf_two.space_one,
			};
			score_new = score_old + get_score(sf_new);
		}
		// combine with sf_two
		else
		{
			SequenceFormat sf_new
			{
				sf_one.space_one,
				sf_two.len + 1,
				sf_two.space_two
			};
			score_new = score_one + get_score(sf_new);
			update_condition(sf_new, player);
		}
	}
	else
	{
		// combine with sf_one
		if (sf_two.space_one != 0)
		{
			SequenceFormat sf_new
			{
				sf_two.space_one,
				sf_one.len + 1,
				sf_one.space_two
			};
			score_new = score_two + get_score(sf_new);
			update_condition(sf_new, player);
		}
		// combine with both
		else
		{
			SequenceFormat sf_new{
				sf_one.space_two,
				sf_one.len + sf_two.len + 1,
				sf_two.space_two
			};
			score_new = get_score(sf_new);
			update_condition(sf_new, player);
		}
	}

	return score_new - score_old;
}

int Board::calculate_delta_op(std::vector<Sequence> const & one, std::vector<Sequence> const & two, int player) const
{
	auto sf_one = get_sequence_format(one, player ^ 3);
	auto sf_two = get_sequence_format(two, player ^ 3);

	SequenceFormat old_sf_one
	{
		sf_one.space_one + sf_two.space_one + 1,
		sf_one.len,
		sf_one.space_two
	};
	SequenceFormat old_sf_two
	{
		sf_one.space_one + sf_two.space_one + 1,
		sf_two.len,
		sf_two.space_two
	};

	auto score_old = get_score(old_sf_one) + get_score(old_sf_two);
	auto score_new = get_score(sf_one) + get_score(sf_two);

	return score_new - score_old;
}

void Board::update_delta(int player, int delta)
{
	if (player == 1)
	{
		heuristic += delta;
		current_action.delta_heuristic += delta;
	}
	else
	{
		heuristic -= delta;
		current_action.delta_heuristic -= delta;
	}
}

int Board::get_score(SequenceFormat const & seq) const
{
	static const std::unordered_map<int, int> len_score_one
	{
		{0, 0},
		{1, 0},
		{2, 10},
		{3, 100},
		{4, 1000},
		{5, 10000}
	};
	static const std::unordered_map<int, int> len_score_two
	{
		{0, 0},
		{1, 0},
		{2, 10},
		{3, 200},
		{4, 3000},
		{5, 10000}
	};
	if (seq.len + seq.space_one + seq.space_two < 5)
		return 0;
	if (seq.len < 5)
	{
		if (seq.space_one != 0 && seq.space_two != 0)
			return len_score_two.at(seq.len);
		return len_score_one.at(seq.len);
	}
	return len_score_one.at(5);
}

void Board::undo_heuristic(Action const & action)
{
	heuristic -= action.delta_heuristic;
}


// ****** Condition ******

void Board::update_condition(SequenceFormat const & seq, int player)
{
	if (seq.len >= 5)
	{
		current_action.is_end = true;
		condition = player == 1 ? Condition::PlayerOneWin : Condition::PlayerTwoWin;
	}
}

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