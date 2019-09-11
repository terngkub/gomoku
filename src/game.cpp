#include "game.hpp"
#include "minimax.hpp"
#include <chrono>
#include <iostream>
#include <regex>

Game::Game(int size) :
	board{size},
	board_size{size},
	current_player{1}
{}

void Game::operator()(int depth)
{
	while (!board.is_end())
	{
		std::cout << "Player " << current_player << "'s turn\n";
		int action;
		if (current_player == 1)
		{
			auto beg = std::chrono::system_clock::now();
			action = Minimax{board, depth, current_player}();
			auto end = std::chrono::system_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - beg);
			std::cout << "AI took " << duration.count() << "ms\n";
		}
		else
			action = get_player_input();
		board.play(action, current_player);
		board.print();
		current_player ^= 3;
	}
	print_condition();
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
		if (!board.is_valid_spot(index))
		{
			std::cout << "Spot is not empty. Please try again.\n";
			continue;
		}

		break;
	}
	return index;
}

void Game::print_condition()
{
	auto condition = board.get_condition();

	std::cout << "\n*************\n";
	if		(condition == Condition::PlayerOneWin)	std::cout << "Player 1 win!\n";
	else if	(condition == Condition::PlayerTwoWin)	std::cout << "Player 2 win!\n";
	else											std::cout << "Draw!\n";
	std::cout << "*************\n";
}