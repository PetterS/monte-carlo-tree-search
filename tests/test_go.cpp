// Petter Strandmark 2012.

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <mcts.h>

#include "games/go.h"

using namespace std;

TEST_CASE("go_game_over1")
{
	static const int M = 3;
	static const int N = 4;
	char board[M][N+1] = {".21.",
	                      "2211",
	                      ".21."};
	auto state = GoState<M, N>(board);
	CHECK(state.get_moves().size() == 0);
}

TEST_CASE("go_have_to_pass")
{
	static const int M = 3;
	static const int N = 3;
	char board[M][N+1] = {"21.",
	                      "211",
	                      ".1."};
	auto state = GoState<M, N>(board);

	state.player_to_move = 1;
	auto moves1 = state.get_moves();
	REQUIRE(moves1.size() == 1);
	CHECK(moves1[0] != (GoState<M, N>::pass));

	state.player_to_move = 2;
	auto moves2 = state.get_moves();
	REQUIRE(moves2.size() == 1);
	CHECK(moves2[0] == (GoState<M, N>::pass));
}

TEST_CASE("go_move_to_no_liberties")
{
	static const int M = 3;
	static const int N = 3;
	char board[M][N+1] = {
		"122",
		"112",
		"1.2"};
	auto state = GoState<M, N>(board);

	int i = 2;
	int j = 1;
	auto move = GoState<M, N>::ij_to_ind(i, j);
	REQUIRE(state.is_move_possible(i, j));
	state.do_move(move);
	REQUIRE(state.has_moves());
}

TEST_CASE("go_ko_rule")
{
	static const int M = 5;
	static const int N = 4;
	char board[M][N+1] = {
		"2.21",
		"2211",
		".211",
		"221.",
		".211"};
	auto state = GoState<M, N>(board);
	int i = 0;
	int j = 1;
	auto move = GoState<M, N>::ij_to_ind(i, j);
	REQUIRE(state.is_move_possible(i, j));
	state.do_move(move);
	REQUIRE(!state.has_moves());
}

TEST_CASE("go3")
{
	static const int M = 3;
	static const int N = 3;
	char board[M][N+1] = {"21.",
	                      "211",
	                      ".1."};
	auto state = GoState<M, N>(board);

	state.do_move(GoState<M, N>::ij_to_ind(2, 0));

	MCTS::ComputeOptions options;
	options.max_iterations = 100;
	options.max_time = 1.0;
	options.verbose = false;
	auto tree = MCTS::compute_tree(state, options, 1);
	REQUIRE(tree->has_children());
	REQUIRE(tree->children.size() == 2);
	std::set<GoState<M, N>::Move> move_set;
	move_set.insert(tree->children[0]->move);
	move_set.insert(tree->children[1]->move);
	REQUIRE(move_set.find(GoState<M, N>::ij_to_ind(0, 0)) != move_set.end());
	REQUIRE(move_set.find(GoState<M, N>::ij_to_ind(1, 0)) != move_set.end());
}

