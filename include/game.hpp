#pragma once
#include "board.hpp"

class Game {
public:
                        Game(int, std::string const &, std::string const &);
    void                operator()(int);

private:
    enum type {
        AI,
        HUMAN
    };

    struct Player {
        int             number;
        enum type       type;
    };

    Board               board;
    int                 board_size;
    int                 _current_player;
    struct Player       _players[2];

    enum type           get_player_mode() const;
    int                 get_player_number() const;
    int                 get_player_input(int player) const;
    void                next_player();
    void                print_condition() const;
};
