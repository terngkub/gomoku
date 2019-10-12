#pragma once
#include "action.hpp"
#include "index.hpp"
#include "sequence.hpp"
#include <bitset>
#include <iostream>
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
	template <typename T>
	std::vector<Sequence>		get_sequence_vector(int index, bool inc) const;
	Sequence					get_next_sequence(Index & i) const;
	SequenceFormat				get_sequence_format(std::vector<Sequence> const & seq_vector, int player) const;

	int							calculate_delta(std::vector<Sequence> const & one, std::vector<Sequence> const & two, int player);
	int							calculate_delta_me(std::vector<Sequence> const & vector_one, std::vector<Sequence> const & vector_two, int player);
	int							calculate_delta_op(std::vector<Sequence> const & one, std::vector<Sequence> const & two, int player) const;

	int							get_score(SequenceFormat const & seq) const;

	void						update_delta(int player, int delta);
	void						undo_heuristic(Action const & action);
	
	// Condition
	void						update_condition(SequenceFormat const & seq, int player);
	void						undo_condition(Action const & action);

	// Printer
	void						print_index() const;
	void						print_board() const;
};

template<typename T>
void Board::update_heuristic_sequence(int index, int player)
{
	auto vector_one = get_sequence_vector<T>(index, true);
	auto vector_two = get_sequence_vector<T>(index, false);
	auto delta = calculate_delta(vector_one, vector_two, player);
	update_delta(player, delta);
}

template <typename T>
std::vector<Sequence> Board::get_sequence_vector(int index, bool inc) const
{
	std::vector<Sequence> seq_vector{};
	T i{index, width, inc};
	i.move();

	auto seq = get_next_sequence(i);
	while (seq.player != -1)
	{
		seq_vector.push_back(seq);
		seq = get_next_sequence(i);
	}

	return seq_vector;
}
