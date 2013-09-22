// Petter Strandmark 2013
// petter.strandmark@gmail.com

#include <algorithm>
#include <iostream>
#include <utility>

#include <mcts.h>

template<unsigned int M, unsigned int N>
class Go5RowState:
	public GoState<M, N>
{
private:
	int last_row, last_col;

	mutable std::vector<Move> scratch;
public:

	Go5RowState():
		last_row(-1),
		last_col(-1)
	{ }

	virtual void do_move(Move move)
	{
		GoState::do_move(move);
		
		/*
		if (move == pass) {
			player_to_move = 3 - player_to_move;
			return;
		}

		int i,j;
		std::tie(i, j) = ind_to_ij(move);
		board[i][j] = player_to_move;
		player_to_move = 3 - player_to_move;
		*/

		if (move >= 0) {
			std::tie(last_row, last_col) = ind_to_ij(move);
		}
		else {
			last_row = last_col = -1;
		}
	}

	virtual unsigned char get_winner() const
	{
		if (last_row < 0) {
			return empty;
		}

		// We only need to check around the last piece played.
		auto piece = board[last_row][last_col];

		// X X X X
		int left = 0, right = 0;
		for (int col = last_col - 1; col >= 0 && board[last_row][col] == piece; --col) left++;
		for (int col = last_col + 1; col < N  && board[last_row][col] == piece; ++col) right++;
		if (left + 1 + right >= 5) {
			return piece;
		}

		// X
		// X
		// X
		// X
		int up = 0, down = 0;
		for (int row = last_row - 1; row >= 0 && board[row][last_col] == piece; --row) up++;
		for (int row = last_row + 1; row < M  && board[row][last_col] == piece; ++row) down++;
		if (up + 1 + down >= 5) {
			return piece;
		}

		return empty;
	}

	/*
	virtual bool has_moves() const
	{
		if (get_winner() != empty) {
			return false;
		}

		for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			if (board[i][j] == empty) {
				return true;
			}
		}}
		return false;
	}
	*/

	/*
	template<typename RandomEngine>
	void do_random_move(RandomEngine* engine)
	{
		get_moves_internal();
		std::uniform_int_distribution<std::size_t> move_ind(0, scratch.size() - 1);
		auto move = scratch[move_ind(*engine)];
		do_move(move);
	}

	void get_moves_internal() const
	{
		scratch.clear();
		if (get_winner() != empty) {
			return;
		}

		//return GoState::get_moves();

		for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			if (board[i][j] == empty) {
				scratch.emplace_back(ij_to_ind(i, j));
			}
		}}
	}
	*/

	virtual std::vector<Move> get_moves() const
	{	
		//get_moves_internal();
		//return scratch;

		if (get_winner() != empty) {
			return std::vector<Move>();
		}
		return GoState::get_moves();
	}

	virtual double get_result(int current_player_to_move) const
	{
		auto winner = get_winner();
		if (winner == empty) {
			return 0.5;
		}

		if (winner == current_player_to_move) {
			return 0.0;
		}
		else {
			return 1.0;
		}
	}
};

