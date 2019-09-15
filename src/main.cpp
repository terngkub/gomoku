#include "flag.hpp"
#include "game.hpp"

int
main (int argc, char const *argv[]) {

    int retcode = EXIT_SUCCESS;
    Flag flag = Flag{};
    auto d = flag.optInt("depth", 5, "choose depth for min-max algorithm");
    auto p1 = flag.optString("player1", "human", "choose between human or ai for player one");
    auto p2 = flag.optString("player2", "ai", "choose between human or ai for player two");
    flag.parse(argc, argv);

    Game{19}(4);
	return retcode;
}
