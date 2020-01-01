// Petter Strandmark 2013
// petter.strandmark@gmail.com

#include <algorithm>
#include <iostream>
#include <utility>

#include <mcts.h>

template<unsigned int M, unsigned int N>
class GoState
{
public:

	// Mutable because is_move_possible temporary modifies
	// the board.
	mutable unsigned char board[M][N];
	unsigned int previous_board_hash_value;
	std::set<unsigned int> all_hash_values;
	

public:
	static const unsigned char empty = 0;
	static const unsigned char player1 = 1;
	static const unsigned char player2 = 2;

	mutable int depth;
	int player_to_move;
	typedef int Move;
	static const Move no_move;
	static const Move pass;

	static int ij_to_ind(int i, int j)
	{
		attest(i >= 0 && j >= 0 && i < M && j < N);
		return N*i + j;
	}

	static std::pair<int, int> ind_to_ij(int ind)
	{ 
		attest(ind >= 0 && ind < M * N);
		return std::make_pair(ind / N, ind % N); 
	}


	GoState():
		previous_board_hash_value(0),
		depth(0),
		player_to_move(1)
	{ 
		all_hash_values.insert(compute_hash_value());

		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j) {
				board[i][j] =  empty;
			}
		}
	}

	GoState(char board[M][N+1]):
		previous_board_hash_value(0),
		depth(0),
		player_to_move(1)
	{
		GoState<M, N> state;
		for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			if (board[i][j] == '1') {
				set_pos(i, j, 1);
			}
			else if (board[i][j] == '2') {
				set_pos(i, j, 2);
			}
			else {
				set_pos(i, j, empty);
			}
		}}
	}

	virtual ~GoState() { }

	virtual unsigned char get_pos(int i, int j) const
	{
		attest(ij_to_ind(i, j) >= 0);
		return board[i][j];
	}

	virtual void set_pos(int i, int j, unsigned char player)
	{
		attest(ij_to_ind(i, j) >= 0);
		board[i][j] = player;
	}

	virtual unsigned int compute_hash_value() const
	{
		unsigned int value = 0;
		for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			value = 65537 * value + board[i][j];
		}}
		return value;
	}

	virtual bool is_move_possible(int i, int j) const
	{
		return is_move_possible(i, j, player_to_move);
	}

	virtual bool is_move_possible(const int i, const int j, const int player) const
	{
		const int opponent = 3 - player;

		if (0 <= i && i < M && 0 <= j && j < N) {
			if (board[i][j] != empty) {
				return false;
			}

			// See if it is possible to move into this empty place.
			std::set<std::pair<int, int>> pieces;
			board[i][j] = player;

			bool possible = false;
			if (is_alive(i, j, &pieces)) {
				// This stone is immediately alive.
				possible = true;
			}
			else {
				// This stone may have killed opponents so
				// that it is alive.

				if (i > 0 && board[i - 1][j] == opponent && !is_alive(i - 1, j, &pieces)) {
					possible = true;
				}
				else if (i < M - 1 && board[i + 1][j] == opponent && !is_alive(i + 1, j, &pieces)) {
					possible = true;
				}
				else if (j > 0 && board[i][j - 1] == opponent && !is_alive(i, j - 1, &pieces)) {
					possible = true;
				}
				else if (j < N - 1 && board[i][j + 1] == opponent && !is_alive(i, j + 1, &pieces)) {
					possible = true;
				}
			}

			if (possible) {
				// Ko rule tests.
				if (compute_hash_value() == previous_board_hash_value) {			
					possible = false;
				}
				else if (all_hash_values.find(compute_hash_value()) != all_hash_values.end()) {
					possible = false;
				}
			}

			if (possible) {
				// Not possible to play in one's own eye.
				if (is_eye(i, j, player)) {
					possible = false;
				}
			}

			board[i][j] = empty;
			return possible;
		}
		else {
			// Not a valid position.
			return false;
		}
	}

	virtual bool is_eye(int i, int j, int player) const
	{
		bool eye = true;
		if (i > 0 && board[i - 1][j] != player) eye = false;
		if (i < M - 1 && board[i + 1][j] != player) eye = false;
		if (j > 0 && board[i][j - 1] != player) eye = false;
		if (j < N - 1 && board[i][j + 1] != player) eye = false;
		return eye;
	}

	virtual void do_move(Move move)
	{
		depth++;

		int opponent = 3 - player_to_move;

		if (move == pass) {
			player_to_move = opponent;
			return;
		}

		int i,j;
		std::tie(i, j) = ind_to_ij(move);
		attest(is_move_possible(i, j));

		board[i][j] = player_to_move;

		// We save the hash values before all captures as this is way easier
		// to check.
		previous_board_hash_value = compute_hash_value();
		all_hash_values.insert(previous_board_hash_value);

		// Check for the killing of any opposing stones.
		if (i > 0 && board[i - 1][j] == opponent) {
			check_alive(i - 1, j);
		}
		if (i < M - 1 && board[i + 1][j] == opponent) {
			check_alive(i + 1, j);
		}
		if (j > 0 && board[i][j - 1] == opponent) {
			check_alive(i, j - 1);
		}
		if (j < N - 1 && board[i][j + 1] == opponent) {
			check_alive(i, j + 1);
		}

		std::set<std::pair<int, int>> pieces;
		// Now the played stone must be alive.
		attest(board[i][j] == player_to_move);
		attest(is_alive(i, j, &pieces));

		// Next player
		player_to_move = opponent;
	}

	virtual bool is_alive(int i_start, int j_start, std::set<std::pair<int, int>>* pieces) const
	{
		if (board[i_start][j_start] == empty) {
			// No piece here, so alive
			return true;
		}

		std::vector<std::pair<int, int>> stack;
		pieces->clear();
		stack.emplace_back(i_start, j_start);
		int player = board[i_start][j_start];

		while (!stack.empty()) {
			auto back = stack.back();
			stack.pop_back();
			if (pieces->find(back) != pieces->end()) {
				// Already checked.
				continue;
			}
			int i,j;
			std::tie(i, j) = back;
			
			if (board[i][j] == player) {

				// Does not work in older GCC.
				//pieces->emplace(i, j);
				pieces->insert(std::make_pair(i, j));

				if (i > 0) {
					stack.emplace_back(i - 1, j);
				}
				if (i < M - 1) {
					stack.emplace_back(i + 1, j);
				}
				if (j > 0) {
					stack.emplace_back(i, j - 1);
				}
				if (j < N - 1) {
					stack.emplace_back(i, j + 1);
				}
			}
			else if (board[i][j] == empty) {
				// Alive.
				return true;
			}
		}
		return false;
	}

	virtual void check_alive(int i, int j)
	{
		std::set<std::pair<int, int>> pieces;
		if (!is_alive(i, j, &pieces)) {
			// Remove the dead pieces.
			for (auto& ij: pieces) {
				int i, j;
				std::tie(i, j) = ij;
				board[i][j] = empty;
			}
		}
	}

	template<typename RandomEngine>
	void do_random_move(RandomEngine* engine)
	{
		auto moves = get_moves();
		attest(! moves.empty());
		std::uniform_int_distribution<std::size_t> move_ind(0, moves.size() - 1);
		auto move = moves[move_ind(*engine)];
		do_move(move);
	}

	virtual bool has_moves() const
	{
		// TODO: make faster.
		return ! get_moves().empty();
	}

	virtual std::vector<Move> get_moves() const
	{
		std::vector<Move> moves;
		if (depth > 1000) {
			attest(false);
			return moves;
		}

		bool opponent_has_move = false;
		for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			if (is_move_possible(i, j, player_to_move)) {
				moves.push_back(ij_to_ind(i, j));
			}

			if (!opponent_has_move && is_move_possible(i, j, 3 - player_to_move)) {
				opponent_has_move = true;
			}
		}}

		if (moves.empty() && opponent_has_move) {
			moves.push_back(pass);
		}

		return moves;
	}

	virtual int get_player_score(int player) const
	{
		int score = 0;
		for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			if (board[i][j] == empty) {
				if (is_eye(i, j, player)) {
					score++;
				}
			}
			if (board[i][j] == player) {
				score++;
			}
		}}
		return score;
	}

	virtual double get_result(int current_player_to_move) const
	{
		int score1 = get_player_score(1);
		int score2 = get_player_score(2);

		if (score1 == score2) {
			return 0.5;
		}
		int winner = 0;
		if (score1 > score2) {
			winner = 1;
		}
		else {
			winner = 2;
		}

		if (winner == current_player_to_move) {
			return 0.0;
		}
		else {
			return 1.0;
		}
	}

	virtual void dump_board(const char* file_name) const
	{
		std::ofstream fout(file_name);
		fout << "static const int M = " << M << ";" << std::endl;
		fout << "static const int N = " << N << ";" << std::endl;
		fout << "char board[M][N+1] = {" << std::endl;
		for (int i = 0; i < M; ++i) {
			fout << "\t\"";
			for (int j = 0; j < N; ++j) {
				if (board[i][j] == empty) fout << ".";
				else fout << int(board[i][j]);
			}
			fout << "\"," << std::endl;
		}
		fout << "};" << std::endl;
	}
};

template<unsigned int M, unsigned int N>
const typename GoState<M, N>::Move GoState<M, N>::no_move = -2;

template<unsigned int M, unsigned int N>
const typename GoState<M, N>::Move GoState<M, N>::pass = -1;
