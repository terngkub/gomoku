#pragma once
#include <set>

class Action
{
public:
    Action();

	int index;
	int player;
	bool play_on_valid_spot;
	std::set<int> valids;
	double delta_heuristic;
	bool is_end;
};