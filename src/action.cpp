#include "action.hpp"

Action::Action():
	play{0},
	play_on_valid_spot{false},
	valids{},
	delta_heuristic{0},
	is_end{false}
{}
