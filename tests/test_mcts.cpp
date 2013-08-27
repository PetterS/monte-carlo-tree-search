// Petter Strandmark 2012.

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <mcts.h>

#include "games/nim.h"

using namespace std;

// Player 1 has two options:
//		1: Draw.
//		2: Nothing happens (Player 2's turn).
//
// Player 2 has five options:
//		  1: Player 1 wins.
//		2-5: Player X wins. (default: 2)
//
// If X == 1, player 1 should play 2 for a guaranteed win after the next move by player 2.
// If X == 2, player 1 should play 1 for an immediate draw.
class TestGame
{
public:
	typedef int Move;
	static const Move no_move = -1;

	TestGame(int X_ = 2)
		: player_to_move(1),
	      winner(-1),
		  X(X_)
	{ }

	void do_move(Move move)
	{
		if (player_to_move == 1) {
			attest(move >= 1 && move <= 2);

			if (move == 1) {
				winner = 0;
			}
			else {
			}
		}
		else if (player_to_move == 2) {
			attest(move >= 1 && move <= 5);

			if (move == 1) {
				winner = 1;
			}
			else {
				winner = X;
			}
		}

		player_to_move = 3 - player_to_move;
	}

	void do_random_move()
	{
		if (player_to_move == 1) {
			std::uniform_int_distribution<Move> moves(1, 2);
			do_move(moves(MCTS::random_engine));
		}
		else if (player_to_move == 2) {
			std::uniform_int_distribution<Move> moves(1, 5);
			do_move(moves(MCTS::random_engine));
		}
		
	}

	bool has_moves() const
	{
		return winner < 0;
	}

	std::vector<Move> get_moves() const
	{
		std::vector<Move> moves;
		if ( ! has_moves()) {
			return moves;
		}

		if (player_to_move == 1) {
			moves.push_back(1);
			moves.push_back(2);
		}
		else if (player_to_move == 2) {
			moves.push_back(1);
			moves.push_back(2);
			moves.push_back(3);
			moves.push_back(4);
			moves.push_back(5);
		}

		return moves;
	}

	double get_result(int current_player_to_move) const
	{
		attest(winner >= 0);

		if (winner == 0) {
			return 0.5;
		}

		if (winner == current_player_to_move) {
			return 0.0;
		}
		else {
			return 1.0;
		}
	}

	int player_to_move;
	int winner;
private:
	int X;
};

TEST_CASE("dummy1")
{
	TestGame state(1);
	auto move = MCTS::compute_move(state, 1000);
	CHECK(move == 2);
}

TEST_CASE("dummy2")
{
	TestGame state(2);
	auto move = MCTS::compute_move(state, 1000);
	CHECK(move == 1);
}

TEST_CASE("Nim_15")
{
	for (int chips = 4; chips <= 21; ++chips) {
		if (chips % 4 != 0) {
			NimState state(chips);
			auto move = MCTS::compute_move(state, 100000);
			CHECK(move == chips % 4);
		}
	}
}
