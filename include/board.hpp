#pragma once
#include "index.hpp"
#include <iostream>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

struct SequenceInfo
{
	SequenceInfo() : len_me{0}, len_op{0}, space_me{0}, space_op_one{0}, space_op_two{0} {}
	int len_me;
	int len_op;
	int space_me;
	int space_op_one;
	int space_op_two;
};

enum Condition
{
	Playing = 0,
	PlayerOneWin = 1,
	PlayerTwoWin = 2,
	Draw = 3
};

struct SequenceAction
{
	bool is_combined;
	int head_index;
	int combined_index;
	int combined_length;
};

struct Action
{
	Action(): play{0}, play_on_valid_spot{false}, valid_spots{}, delta_heuristic{0}, is_end{false} {}
	int play;
	bool play_on_valid_spot;
	std::set<int> valid_spots;
	double delta_heuristic;
	bool is_end;
};

class Board
{
public:
	Board(int size);

	void play(int index, int player);
	void undo();
	int get_heuristic(int player) const;
	std::set<int> const & next(int player) const;
	void print() const;
	bool is_first_turn() const;
	bool is_valid_spot(int index) const;
	bool is_end() const;
	Condition get_condition() const;

private:
	int size;
	std::vector<int> spots;
	Condition condition;
	std::stack<Action> history;

	// ***** Valid Spots ******
	std::set<int> valid_spots;
	std::set<int> update_valid_spots(int index, int player);
	void insert_if_valid(std::set<int> & actions, int index);
	void undo_valid_spots(std::set<int> const & action_valid_spots);

	// ****** Heuristic ******

	int heuristic;
	void update_heuristic(int index, int player, Action & action);
	void update_heuristic_delta(SequenceInfo & one, SequenceInfo & two, int player, Action & action);
	void undo_heuristic(Action const & action);
	
	template<typename T>
	void update_heuristic_sequence(int index, int player, Action & action)
	{
		auto one = explore_sequence<T>(index, player, true);
		auto two = explore_sequence<T>(index, player, false);
		update_heuristic_delta(one, two, player, action);
	}

	template<typename T>
	SequenceInfo explore_sequence(int index, int player, bool inc)
	{
		T i{index, 19, inc};
		SequenceInfo info{};

		i.move();
		if (!i.check())
		{
			return info;
		}
		if (spots[i.val()] == player)
		{
			for (; i.check() && spots[i.val()] == player; i.move())
				++info.len_me;
			for (; i.check() && spots[i.val()] == 0; i.move())
				++info.space_me;
		}
		else
		{
			for (; i.check() && spots[i.val()] == 0; i.move())
			{
				++info.space_me;
				++info.space_op_one;
			}
			int opponent = player ^ 3;
			for (; i.check() && spots[i.val()] == opponent; i.move())
				++info.len_op;
			for (; i.check() && spots[i.val()] == 0; i.move())
				++info.space_op_two;
		}
		return info;
	}
};
