// Petter Strandmark 2013
// petter.strandmark@gmail.com

#include <algorithm>
#include <iostream>
using namespace std;

#include <mcts.h>

#include "nim.h"

int main()
{
	using namespace std;

	NimState state(15);
	while (state.has_moves()) {
		cout << "State: " << state.player_to_move << endl;
		NimState::Move move;
		if (state.player_to_move == 1) {
			move = MCTS::compute_move(state, 100000, true);
		}
		else {
			move = MCTS::compute_move(state, 100000, true);
		}

		cout << "Best move: " << move << endl;
		state.do_move(move);
	}
	if (state.get_result(2) == 1.0) {
		cout << "Player 1 wins!" << endl;
	}
	else if (state.get_result(1) == 1.0) {
		cout << "Player 2 wins!" << endl;
	}
	else {
		cout << "Nobody wins! (score: " << state.get_result(1) << " for player 1)" << endl;
	}
}
