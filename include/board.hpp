#pragma once
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

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
	int play;
	bool play_on_valid_spot;
	std::set<int> valid_spots;
	SequenceAction seq_x;
	SequenceAction seq_y;
};

class Board
{
public:
	Board(int size);

	void play(int index, int player);
	void undo();
	int heuristic(int player, int depth_score) const;
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


	// ***** Sequence *******

	// store sequence length <head of sequence, length> used for detect
	std::unordered_map<int, int> seq_x;
	std::unordered_map<int, int> seq_y;

	// functions to find the head of sequence, combine with map to find sequence length
	int get_head_x(int index) const;
	int get_head_y(int index) const;

	// update sequence
	SequenceAction update_seq_x(int index);
	SequenceAction update_seq_y(int index);

	// undo sequence
	void undo_seq_x(SequenceAction const & seq_action);
	void undo_seq_y(SequenceAction const & seq_action);
};
