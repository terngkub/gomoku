#include "game.hpp"
#include "minimax.hpp"
#include <chrono>
#include <iostream>
#include <regex>

Game::Game(int board_size, std::string const &p1, std::string const &p2) :
    board{board_size},
    board_size{board_size},
    _current_player{0} {

    for (int k = 0; k < 2; ++k) {
        auto player = (!k ? p1 : p2);
        if (player == "ai") {
            _players[k] = Player{
                .number = k,
                .type = AI
            };
        } else if (player == "human") {
            _players[k] = Player{
                .number = k,
                .type = HUMAN
            };
        } else {
            std::cout << "error: " << player << " isn't recognized as a valid player option" << std::endl;
            std::cout << "Available player options are:" << std::endl;
            std::cout << " - human" << std::endl;
            std::cout << " - ai" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void
Game::operator()(int depth) {

    while (!board.is_end()) {
		std::cout << "Player " << get_player_number() << "'s turn" << std::endl;

		int action;
		if (get_player_mode() == AI) {
			auto beg = std::chrono::system_clock::now();
			action = Minimax{board, depth, get_player_number()}();
			auto end = std::chrono::system_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - beg);
			std::cout << "AI took " << duration.count() << "ms\n";
		} else {
            action = get_player_input();
        }

		board.play(action, get_player_number());
		board.print();
		next_player();
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

Game::type
Game::get_player_mode() const {

    return _players[_current_player].type;
}

int
Game::get_player_number() const {

    // Number starts from zero, but we consider players are 1 and 2, so we add one to the return
    return _players[_current_player].number + 1;
}

void
Game::next_player() {

    _current_player = static_cast<int>(~static_cast<unsigned>(_current_player) & 0x1u);
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
