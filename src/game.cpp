#include "game.hpp"
#include "minimax.hpp"
#include <iostream>

Game::Game() :
	board{},
	current_player{1}
{}

void Game::operator()()
{
	while (!board.is_full())
	{
		std::cout << "Player " << current_player << "'s turn\n";
		int action;

		if (current_player == 1)
		{
			action = Minimax{board, current_player}();
		}
		else
		{
			while (true)
			{
				std::cout << "Input spot [1-9]: ";
				std::string str;
				getline(std::cin, str);
				try
				{
					action = std::stoi(str);
					--action;
				}
				catch (std::exception const & e)
				{
					std::cout << "Invalid syntax. Please try again.\n";
					continue;
				}
				if (action < 0 || action > 8)
					std::cout << "Input out of range. Please try again.\n";
				else if (!board.is_empty_spot(action))
					std::cout << "Spot is not empty. Please try again.\n";
				else
					break;
			}
		}

		board.play(action, current_player);
		board.print();

		if (board.is_win(current_player))
		{
			std::cout << "\n*************\n";
			std::cout << "Player " << current_player << " win!\n";
			std::cout << "*************\n";
			return;
		}

		current_player ^= 3;
	}

	std::cout << "\n*************\n";
	std::cout << "Draw!\n";
	std::cout << "*************\n";
}
