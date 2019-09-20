#pragma once
#include "index.hpp"
#include <bitset>
#include <iostream>
#include <set>
#include <vector>

enum class Condition
{
	Playing = 0,
	PlayerOneWin = 1,
	PlayerTwoWin = 2,
	Draw = 3
};

struct Sequence
{
	Sequence() : len_me{0}, len_op{0}, space_me{0}, space_op_one{0}, space_op_two{0} {}
	int len_me;
	int len_op;
	int space_me;
	int space_op_one;
	int space_op_two;
};

class Board
{
public:

	// Constructor - custom
	Board() = delete;
	Board(int width);

	// Destructor - default
	~Board() = default;

	// Copy - default
	Board(Board const &) = default;
	Board & operator=(Board const &) = default;

	// Move - default
	Board(Board &&) = default;
	Board & operator=(Board &&) = default;

	// ****** Getters ******

	int						get_heuristic(int player) const;
	Condition				get_condition() const;


	// Checkers

	bool					is_valid(int index, int player) const;
	bool					is_end(int player) const;


	// ****** Public Methods ******

	Board					play(int index, int player);
	std::set<int> const &	get_nexts(int player) const;
	void					print() const;


private:

	int						width;
	int						size;
	std::vector<int>		indexes;
	Condition				condition;
	int						heuristic;
	std::set<int>			valid_one;
	std::set<int>			valid_two;

	// ****** Printer ******

	void					print_board_pieces() const;
	void					print_board_indexes_row() const;


	// ***** Valid Spots ******

	void					update_valid(int index, int player);
	void					remove_valid(int index);
	void					update_valid_top(int index, int player);
	void					update_valid_middle(int index, int player);
	void					update_valid_bottom(int index, int player);
	void					insert_valid(int index, int player);


	// ****** Heuristic ******

	void					update_heuristic(int index, int player);
	template<typename T>
	void					update_heuristic_sequence(int index, int player);
	template<typename T>
	Sequence				explore_sequence(int index, int player, bool inc);
	void					update_heuristic_delta(Sequence & inc, Sequence & dec, int player);
	int						get_score(int len, int space_one, int space_two);
};


template<typename T>
void Board::update_heuristic_sequence(int index, int player)
{
	auto inc_seq = explore_sequence<T>(index, player, true);
	auto dec_seq = explore_sequence<T>(index, player, false);
	update_heuristic_delta(inc_seq, dec_seq, player);
}

template<typename T>
Sequence Board::explore_sequence(int index, int player, bool inc)
{
	T curr_idx{index, width, inc};
	Sequence seq{};

	curr_idx.move();
	if (!curr_idx.check())
		return seq;

	// There are two type of sequences:
	// 1. my pieces, space => lead to increase/combination of sequence length
	// 2. space, opponent, space => lead to opponent space reduction.

	if (indexes[curr_idx.val()] == player)
	{
		for (; curr_idx.check() && indexes[curr_idx.val()] == player; curr_idx.move())
			++seq.len_me;
		for (; curr_idx.check() && indexes[curr_idx.val()] == 0; curr_idx.move())
			++seq.space_me;
	}
	else
	{
		for (; curr_idx.check() && indexes[curr_idx.val()] == 0; curr_idx.move())
		{
			++seq.space_me;
			++seq.space_op_one;
		}
		int opponent = player ^ 3;
		for (; curr_idx.check() && indexes[curr_idx.val()] == opponent; curr_idx.move())
			++seq.len_op;
		for (; curr_idx.check() && indexes[curr_idx.val()] == 0; curr_idx.move())
			++seq.space_op_two;
	}

	return seq;
}