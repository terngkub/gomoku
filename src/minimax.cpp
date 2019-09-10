#include "minimax.hpp"
#include <limits>
#include <iostream>

Minimax::Minimax(Board board, int ai) :
	board{board},
	ai{ai}
{}

int Minimax::operator()(int depth)
{
	minimax(ai, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	return best_action;
}

int Minimax::minimax(int player, int depth, int alpha, int beta)
{
	if (depth == 0 || board.is_end())
	{
		int score = board.heuristic(ai, depth);
		return score;
	}

	if (player == ai)
	{
		int max_score = std::numeric_limits<int>::min();
		auto actions = board.next(player);
		for (auto action : actions)
		{
			board.play(action, player);
			auto new_score = minimax(player ^ 3, depth - 1, alpha, beta);
			board.redo(action, player);

			if (new_score > max_score)
			{
				max_score = new_score;
				best_action = action;
			}

			alpha = std::max(alpha, new_score);
			if (beta <= alpha) break;
		}
		return max_score;
	}

	int min_score = std::numeric_limits<int>::max();
	auto actions = board.next(player);
	for (auto action : actions)
	{
		board.play(action, player);
		auto new_score = minimax(player ^ 3, depth - 1, alpha, beta);
		board.redo(action, player);

		if (new_score < min_score)
			min_score = new_score;

		beta = std::min(beta, new_score);
		if (beta <= alpha) break;
	}
	return min_score;
}
