// Petter Strandmark 2013
// petter.strandmark@gmail.com

#include <algorithm>
#include <iostream>
using namespace std;

#include <mcts.h>

class ConnectFourState
{
public:
	typedef int Move;
	static const Move no_move = -1;

	static const char player_markers[3]; 

	ConnectFourState(int num_rows_ = 6, int num_cols_ = 7)
		: player_to_move(1),
	      num_rows(num_rows_),
	      num_cols(num_cols_)
	{ 
		board.resize(num_rows, vector<char>(num_cols, player_markers[0]));
	}

	void do_move(Move move)
	{
		attest(0 <= move && move < num_cols);
		attest(board[0][move] == player_markers[0]);
		check_invariant();

		int row = num_rows - 1;
		while (board[row][move] != player_markers[0]) row--;
		board[row][move] = player_markers[player_to_move];

		player_to_move = 3 - player_to_move;
	}

	void do_random_move()
	{
		attest(has_moves());
		check_invariant();
		std::uniform_int_distribution<Move> moves(0, num_cols - 1);

		while (true) {
			auto move = moves(MCTS::random_engine);
			if (board[0][move] == player_markers[0]) {
				do_move(move);
				return;
			}
		}
	}

	bool has_moves() const
	{
		check_invariant();

		char winner = get_winner();
		if (winner != player_markers[0]) {
			return false;
		}

		for (int col = 0; col < num_cols; ++col) {
			if (board[0][col] == player_markers[0]) {
				return true;
			}
		}
		return false;
	}

	std::vector<Move> get_moves() const
	{
		check_invariant();

		std::vector<Move> moves;
		if (get_winner() != player_markers[0]) {
			return moves;
		}

		moves.reserve(num_cols);

		for (int col = 0; col < num_cols; ++col) {
			if (board[0][col] == player_markers[0]) {
				moves.push_back(col);
			}
		}
		return moves;
	}

	char get_winner() const
	{
		// X X X X
		for (int row = 0; row < num_rows; ++row) {
			for (int col = 0; col < num_cols - 3; ++col) {
				if (board[row][col] == player_markers[0]) {
					continue;
				}

				bool is_win = true;
				for (int i = 1; i < 4; ++i) {
					if (board[row][col + i] != board[row][col]) {
						is_win = false;
						break;
					}
				}
				if (is_win) {
					return board[row][col];
				}
			}
		}

		// X
		// X
		// X
		// X
		for (int row = 0; row < num_rows - 3; ++row) {
			for (int col = 0; col < num_cols; ++col) {
				if (board[row][col] == player_markers[0]) {
					continue;
				}

				bool is_win = true;
				for (int i = 1; i < 4; ++i) {
					if (board[row + i][col] != board[row][col]) {
						is_win = false;
						break;
					}
				}
				if (is_win) {
					return board[row][col];
				}
			}
		}

		// X
		//  X
		//   X
		//    X
		for (int row = 0; row < num_rows - 3; ++row) {
			for (int col = 0; col < num_cols - 3; ++col) {
				if (board[row][col] == player_markers[0]) {
					continue;
				}

				bool is_win = true;
				for (int i = 1; i < 4; ++i) {
					if (board[row + i][col + i] != board[row][col]) {
						is_win = false;
						break;
					}
				}
				if (is_win) {
					return board[row][col];
				}
			}
		}

		//    X
		//   X
		//  X
		// X
		for (int row = 3; row < num_rows; ++row) {
			for (int col = 0; col < num_cols - 3; ++col) {
				if (board[row][col] == player_markers[0]) {
					continue;
				}

				bool is_win = true;
				for (int i = 1; i < 4; ++i) {
					if (board[row - i][col + i] != board[row][col]) {
						is_win = false;
						break;
					}
				}
				if (is_win) {
					return board[row][col];
				}
			}
		}

		return player_markers[0];
	}

	double get_result(int current_player_to_move) const
	{
		attest( ! has_moves());
		check_invariant();

		auto winner = get_winner();
		if (winner == player_markers[0]) {
			return 0.5;
		}

		if (winner == player_markers[current_player_to_move]) {
			return 0.0;
		}
		else {
			return 1.0;
		}
	}

	void print(ostream& out) const
	{
		out << endl;
		out << " ";
		for (int col = 0; col < num_cols - 1; ++col) {
			out << col << ' ';
		}
		out << num_cols - 1 << endl;
		for (int row = 0; row < num_rows; ++row) {
			out << "|";
			for (int col = 0; col < num_cols - 1; ++col) {
				out << board[row][col] << ' ';
			}
			out << board[row][num_cols - 1] << "|" << endl;
		}
		out << "+";
		for (int col = 0; col < num_cols - 1; ++col) {
			out << "--";
		}
		out << "-+" << endl;
		out << player_markers[player_to_move] << " to move " << endl << endl;
	}

	int player_to_move;
private:

	void check_invariant() const
	{
		attest(player_to_move == 1 || player_to_move == 2);
	}

	int num_rows, num_cols;
	vector<vector<char>> board; 
};

ostream& operator << (ostream& out, const ConnectFourState& state)
{
	state.print(out);
	return out;
}

const char ConnectFourState::player_markers[3] = {'.', 'X', 'O'}; 
