#include "minimax.hpp"
#include <limits>
#include <iostream>
#include <functional>

Minimax::Minimax(Board board, int depth, int ai) :
	board{board},
	max_depth{depth},
	ai{ai}
{}

int Minimax::operator()()
{
	if (board.is_first_turn())
		return 180;
	minimax(ai, max_depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	return best_action;
}

int Minimax::minimax(int player, int depth, int alpha, int beta)
{
	if (depth == 0 || board.is_end())
		return board.get_heuristic(ai);

	auto actions = board.next(player);

	int base_score = (player == ai)
		? std::numeric_limits<int>::min()
		: std::numeric_limits<int>::max();

	auto update_score = (player == ai)
		? std::function<void(int const, int const)>([&](int const new_score, int const action)
			{
				if (new_score > base_score)
				{
					base_score = new_score;
					if (depth == max_depth)
						best_action = action;
				}
				alpha = std::max(alpha, new_score);
			})
		: std::function<void(int const, int const)>([&](int const new_score, int const action)
			{
				(void)action;
				if (new_score < base_score)
					base_score = new_score;
				beta = std::min(beta, new_score);
			});

	for (auto action : actions)
	{
		board.play(action, player);
		auto new_score = minimax(player ^ 3, depth - 1, alpha, beta);
		board.undo();
		update_score(new_score, action);
		if (beta <= alpha) break;
	}
	return base_score;
}
