#include "minimax.hpp"
#include <limits>
#include <iostream>
#include <functional>

Minimax::Minimax(int depth, int ai) :
	max_depth{depth},
	ai{ai},
	complexity{0},
	visited{}
{}

int Minimax::operator()(Board & board)
{
	minimax(board, ai, max_depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	std::cout << "Complexity: " << complexity << "\n";
	return best_action;
}

int Minimax::minimax(Board & board, int player, int depth, int alpha, int beta)
{
	++complexity;

	if (depth == 0 || board.is_end(player))
		return board.get_heuristic(ai);

	auto nexts = board.get_nexts(player);

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
					{
						best_action = action;
					}
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

	auto cmp = [&player](auto const & lhs, auto const & rhs){ return lhs.second.get_heuristic(player) > rhs.second.get_heuristic(player); };
	std::set<std::pair<int, Board>, decltype(cmp)> actions{cmp};
	for (auto next : nexts)
	{
		auto b = board.play(next, player);
		actions.insert({next, std::move(b)});
	}

	for (auto action : actions)
	{
		int new_score;
		if (visited.find(action.second.get_bitset()) != visited.end())
		{
			new_score = visited[action.second.get_bitset()];
		}
		else
		{
			new_score = minimax(action.second, player ^ 3, depth - 1, alpha, beta);
			visited[action.second.get_bitset()] = new_score;
		}
		update_score(new_score, action.first);
		if (beta <= alpha) break;
	}
	return base_score;
}
