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

	template<typename RandomEngine>
	void do_random_move(RandomEngine* engine)
	{
		attest(chips > 0);
		check_invariant();

		int max = std::min(3, chips);
		std::uniform_int_distribution<Move> moves(1, max);
		do_move(moves(*engine));

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
