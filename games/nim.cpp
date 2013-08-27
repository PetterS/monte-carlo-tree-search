// Petter Strandmark 2013
// petter.strandmark@gmail.com

#include <algorithm>
#include <iostream>
using namespace std;

#include <mcts.h>

class NimState
{
public:
	typedef int Move;
	static const Move no_move = -1;

	NimState(int chips_ = 17)
		: player_to_move(1),
	      chips(chips_)
	{ }

	void do_move(Move move)
	{
		attest(move >= 1 && move <= 3);
		check_invariant();
	
		chips -= move;
		player_to_move = 3 - player_to_move;

		check_invariant();
	}

	void do_random_move()
	{
		attest(chips > 0);
		check_invariant();

		int max = std::min(3, chips);
		std::uniform_int_distribution<Move> moves(1, max);
		do_move(moves(MCTS::random_engine));

		check_invariant();
	}

	bool has_moves() const
	{
		check_invariant();
		return chips > 0;
	}

	std::vector<Move> get_moves() const
	{
		check_invariant();

		std::vector<Move> moves;
		for (Move move = 1; move <= std::min(3, chips); ++move) {
			moves.push_back(move);
		}
		return moves;
	}

	double get_result(int current_player_to_move) const
	{
		attest(chips == 0);
		check_invariant();

		if (player_to_move == current_player_to_move) {
			return 1.0;
		}
		else {
			return 0.0;
		}
	}

	int player_to_move;
private:

	void check_invariant() const
	{
		attest(chips >= 0);
		attest(player_to_move == 1 || player_to_move == 2);
	}

	int chips;
};

int main()
{
	using namespace std;

	NimState state(15);
	while (state.has_moves()) {
		cout << "State: " << state.player_to_move << endl;
		NimState::Move move;
		if (state.player_to_move == 1) {
			move = MCTS::compute_move(state, 100000);
		}
		else {
			move = MCTS::compute_move(state, 10);
		}

		cout << "Best move: " << move << endl;
		state.do_move(move);

		return 0;
	}
	if (state.get_result(1) == 1.0) {
		cout << "Player 1 wins!" << endl;
	}
	else if (state.get_result(2) == 1.0) {
		cout << "Player 1 wins!" << endl;
	}
	else {
		cout << "Nobody wins! (score: " << state.get_result(1) << " for player 1)" << endl;
	}
}
