#include "minimax.hpp"
#include <limits>
#include <iostream>
#include <functional>

struct ActionComparator
{
	bool operator()(std::pair<int, int> const & lhs, std::pair<int, int> const & rhs) const
	{
		return lhs.second > rhs.second;
	}
};

Minimax::Minimax(Board board, int depth, int ai) :
	board{board},
	max_depth{depth},
	ai{ai},
	visited_map{},
	complexity{0}
{}

int Minimax::operator()()
{
	if (board.is_first_turn())
		return board.get_size() / 2;
	minimax(ai, max_depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	std::cout << "complexity: " << complexity << "\n";
	return best_action;
}

int Minimax::minimax(int player, int depth, int alpha, int beta)
{
	++complexity;

	if (depth == 0 || board.is_end())
	{
		return board.get_heuristic(ai);
	}

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

	std::set<std::pair<int, int>, ActionComparator> actions;
	for (auto next : nexts)
	{
		board.play(next, player);
		auto h = board.get_heuristic(player);
		actions.insert({next, h});
		board.undo();
	}

	for (auto & action : actions)
	{
		board.play(action.first, player);
		int new_score;
		if (visited_map.find(board.get_indexes()) != visited_map.end())
		{
			new_score = visited_map[board.get_indexes()];
		}
		else
		{
			new_score = minimax(player ^ 3, depth - 1, alpha, beta);
			visited_map[board.get_indexes()] = new_score;
		}
		board.undo();
		update_score(new_score, action.first);
		if (beta <= alpha) break;
	}
	return base_score;
}
