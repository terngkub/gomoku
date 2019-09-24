#pragma once
#include "action.hpp"
#include "index.hpp"
#include "sequence.hpp"
#include <bitset>
#include <set>
#include <stack>
#include <vector>

enum class Condition
{
	Playing = 0,
	PlayerOneWin = 1,
	PlayerTwoWin = 2,
	Draw = 3
};

class Board
{
public:

	// Constructors and Destructor
	Board() = delete;
	Board(int width);
	~Board() = default;

	// Getter
	int							get_size() const;
	Condition					get_condition() const;
	int							get_heuristic(int player) const;
	std::vector<int> const &	get_indexes() const;
	std::set<int> const &		get_nexts(int player) const;

	// Checkers
	bool						is_first_turn() const;
	bool						is_valid_spot(int index) const;
	bool						is_end() const;

	// Public Methods
	void						play(int index, int player);
	void						undo();
	void						print() const;


private:

	int							width;
	int							size;
	int							heuristic;
	Condition					condition;
	std::vector<int>			indexes;
	std::set<int>				valids;
	std::stack<Action>			history;
	Action						current_action;

	// History
	void						save_history();
	Action						load_history();

	// Indexes
	void						update_indexes(int index, int player);
	void						undo_indexes(Action const & action);

	// Valids
	void						update_valids(int index, int player);
	void						update_valids_top(int index, int player);
	void						update_valids_middle(int index, int player);
	void						update_valids_bottom(int index, int player);
	void						insert_left_right(int based_index, int inserted_index);
	void						insert_valid(int index);
	void						undo_valids(Action const & action);

	// Heuristic
	void						update_heuristic(int index, int player);
	template<typename T>
	void						update_heuristic_sequence(int index, int player);
	template<typename T>
	Sequence					explore_sequence_normal(int index, int player, bool inc);
	void						update_heuristic_delta(Sequence & one, Sequence & two, int player);
	int							get_score(int len, int space_one, int space_two);
	void						undo_heuristic(Action const & action);
	
	// Condition
	void						undo_condition(Action const & action);

	// Printer
	void						print_index() const;
	void						print_board() const;
};

template<typename T>
void Board::update_heuristic_sequence(int index, int player)
{
	auto one = explore_sequence_normal<T>(index, player, true);
	auto two = explore_sequence_normal<T>(index, player, false);
	update_heuristic_delta(one, two, player);
}

template<typename T>
Sequence Board::explore_sequence_normal(int index, int player, bool inc)
{
	T i{index, 19, inc};
	Sequence info{};

	i.move();
	if (!i.check())
		return info;

	// There are 2 types of sequence:
	// 1. my pieces, space => increase of my length or combination
	// 2. space, opponent, space => opponent space reduction

	if (indexes[i.val()] == player)
	{
		for (; i.check() && indexes[i.val()] == player; i.move())
			++info.len_me;
		for (; i.check() && indexes[i.val()] == 0; i.move())
			++info.space_me;
	}
	else
	{
		for (; i.check() && indexes[i.val()] == 0; i.move())
		{
			++info.space_me;
			++info.space_op_one;
		}
		int opponent = player ^ 3;
		for (; i.check() && indexes[i.val()] == opponent; i.move())
			++info.len_op;
		for (; i.check() && indexes[i.val()] == 0; i.move())
			++info.space_op_two;
	}
	return info;
}