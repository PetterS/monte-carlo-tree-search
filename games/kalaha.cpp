// Petter Strandmark 2014
// petter.strandmark@gmail.com
// 
// See
// Solving Kalah, http://www.fdg.unimaas.nl/educ/donkers/pdf/kalah.pdf
//

#include <iostream>
using namespace std;

#include <mcts.h>

#include "kalaha.h"

void main_program()
{
	using namespace std;

	// Set this to true to play against the computer.
	bool human_player = true;

	MCTS::ComputeOptions player1_options, player2_options;
	player1_options.max_iterations = -1;
	player1_options.max_time = 1.0;
	player1_options.verbose = true;
	player2_options.max_iterations =  -1;
	player2_options.max_time = 0.5;
	player2_options.verbose = true;

	typedef KalahaState<6> State;
	State state(3);

	stringstream move_string;

	while (state.has_moves()) {
		cout << endl << move_string.str() << endl;
		cout << state << endl;

		State::Move move = State::no_move;
		if (state.player_to_move == 1) {
			move = MCTS::compute_move(state, player1_options);
			state.do_move(move);
		}
		else {
			if (human_player) {
				while (true) {
					cout << "Input your move: ";
					move = State::no_move;
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
				move = MCTS::compute_move(state, player2_options);
				state.do_move(move);
			}
		}

		move_string << move;

		// Forced passing simplifies the fact that a player may
		// move again in some circumstances.
		if (state.player_must_pass) {
			state.do_move(State::pass_move);
			cout << endl << "Player " << state.player_to_move << " goes again.";
		}
		else {
			move_string << "-";
		}
	}

	cout << endl << "Final state:\n" << state << endl;

	if (state.get_result(2) == 1.0) {
		cout << "Player 1 wins!" << endl;
	}
	else if (state.get_result(1) == 1.0) {
		cout << "Player 2 wins!" << endl;
	}
	else {
		cout << "Nobody wins!" << endl;
	}

	cout << move_string.str() << endl;
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