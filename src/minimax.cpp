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
	// std::cout << "minimax depth " << depth << "\n";
	if (depth == 0 || board.is_end())
	{
		// std::cout << "return " << board.get_heuristic(ai) << std::endl;
		return board.get_heuristic(ai);
	}

	auto actions = board.next(player);

	int base_score = (player == ai)
		? std::numeric_limits<int>::min()
		: std::numeric_limits<int>::max();

	auto update_score = (player == ai)
		? std::function<void(int const, int const)>([&](int const new_score, int const action)
			{
				// std::cout << "function1\n";
				if (new_score > base_score)
				{
					base_score = new_score;
					// std::cout << "update max_score " << base_score << std::endl;
					if (depth == max_depth)
					{
						// std::cout << "update action " << best_action << std::endl;
						best_action = action;
					}
				}
				alpha = std::max(alpha, new_score);
			})
		: std::function<void(int const, int const)>([&](int const new_score, int const action)
			{
				// std::cout << "function 2\n";
				(void)action;
				if (new_score < base_score)
				{
					// std::cout << "update min_score " << new_score << std::endl;
					base_score = new_score;
				}
				beta = std::min(beta, new_score);
			});

	// std::cout << "valid: ";
	// for (auto action : actions)
	// {
	// 	std::cout << action/19+1 << "," << action%19+1 << " ";
	// }
	// std::cout << "\n";
	for (auto action : actions)
	{
		// std::cout << "action " << action/19+1 << "," << action%19+1 << "\n";
		board.play(action, player);
		auto new_score = minimax(player ^ 3, depth - 1, alpha, beta);
		board.undo();
		update_score(new_score, action);
		if (beta <= alpha) break;
	}
	return base_score;
}
