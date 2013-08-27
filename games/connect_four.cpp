// Petter Strandmark 2013
// petter.strandmark@gmail.com

#include <iostream>
using namespace std;

#include <mcts.h>

#include "connect_four.h"

void main_program()
{
	using namespace std;

	bool human_player = true;

	ConnectFourState state;
	while (state.has_moves()) {
		cout << endl << "State: " << state << endl;

		ConnectFourState::Move move = ConnectFourState::no_move;
		if (state.player_to_move == 1) {
			move = MCTS::compute_move(state, 100000, true);
			state.do_move(move);
		}
		else {
			if (human_player) {
				while (true) {
					cout << "Input your move: ";
					move = ConnectFourState::no_move;
					cin >> move;
					try {
						state.do_move(move);
						break;
					}
					catch (std::exception& ) {
						cout << "Invalid move." << endl;
					}
				}
			}
			else {
				move = MCTS::compute_move(state, 1000, true);
				state.do_move(move);
			}
		}
	}

	cout << endl << "Final state: " << state << endl;

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

int main()
{
	try {
		main_program();
	}
	catch (std::runtime_error& error) {
		std::cerr << "ERROR: " << error.what() << std::endl;
		return 1;
	}
}