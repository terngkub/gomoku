#include "game.hpp"
#include "minimax.hpp"
#include <iostream>
#include <regex>

Game::Game(int size) :
	board{size},
	board_size{size},
	current_player{1}
{}

void Game::operator()(int depth)
{
	while (!board.is_full())
	{
		std::cout << "Player " << current_player << "'s turn\n";
		int action = (current_player == 1)
			? Minimax{board, current_player}(depth)
			: get_player_input();
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

int Game::get_player_input()
{
	int index = 0;
	while (true)
	{
		std::cout << "Input coordinate [1-" << board_size << "] [1-" << board_size << "]: ";
		std::string str;
		getline(std::cin, str);

		std::smatch matches;
		static const std::regex pattern{R"(^\s*([1-9]|1[0-9])\s+([1-9]|1[0-9])\s*$)"};
		if (!std::regex_search(str, matches, pattern))
		{
			std::cout << "Invalid syntax. Please try again.\n";
			continue;
		}

		int x = std::stoi(matches.str(1)) - 1;
		int y = std::stoi(matches.str(2)) - 1;
		if (x < 0 || x > board_size - 1 || y < 0 || y > board_size - 1)
		{
			std::cout << x << " " << y << "\n";
			std::cout << "Input out of range. Please try again.\n";
			continue;
		}

		index = y * board_size + x;
		if (!board.is_empty_spot(index))
		{
			std::cout << "Spot is not empty. Please try again.\n";
			continue;
		}

		break;
	}
	std::cout << index << "\n";
	return index;
}