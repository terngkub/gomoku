#pragma once
#include <set>
#include <vector>
#include <unordered_map>
#include <list>

enum Condition
{
	playing = 0,
	one_win = 1,
	two_win = 2,
	draw = 3
};

struct SequenceAction
{
	bool is_combined;
	int added_index;
	int deleted_index;
};

struct Action
{
	int play;
	std::vector<int> valid;
	SequenceAction vertical_action;
	SequenceAction horizontal_action;
	SequenceAction downward_diagonal_action;
	SequenceAction upward_diagonal_action;
};

class Board
{
public:
	Board(int size);

	void play(int action, int player);
	void redo(int action, int player);
	int heuristic(int player, int depth_score) const;
	std::set<int> next(int player) const;
	void print() const;
	bool is_empty_spot(int spot) const;

	bool is_end() const;
	bool is_full() const;
	bool is_win(int player) const;
	int detect_horizontal(int player, int index) const;
	int detect_vertical(int player, int index) const;

private:
	int size;
	std::vector<int> spots;

	// ***** Additional ******

	// store played and valid move
	std::set<int> played;
	std::set<int> valid;

	// store sequence length <head of sequence, length> used for detect
	std::unordered_map<int, int> vertical_sequence;
	std::unordered_map<int, int> horizontal_sequence;
	std::unordered_map<int, int> downward_diagonal_sequence;
	std::unordered_map<int, int> upward_diagonal_sequence;

	// functions to find the head of sequence, combine with map to find sequence length
	int get_vertical_head(int index) const;
	int get_horizontal_head(int index) const;
	int get_downward_diagonal_head(int index) const;
	int get_upward_diagonal_head(int index) const;

	// enum condition, used to quickly determine state of the game
	Condition condition;

	// history
	std::list<Action> history;
};

// note
// head = get_horizontal_head(index)
// check if there is a next head of sequence or not

// if not
// normal case
// map[index] += 1;

// if yes
// combine case
// tmp_count = map.at(index + 1)
// map.remove(index+1)
// map[head] += 1 + tmp_count;

// how to store this sequence?
// for normal case
// * incremented head
// for combine case
// * delete head and count
// * incremented head