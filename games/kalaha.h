// Petter Strandmark 2014
// petter.strandmark@gmail.com

#include <algorithm>
#include <iostream>
using namespace std;

#include <mcts.h>

template<short num_bins>
class KalahaState
{
public:
	typedef short Move;
	static const Move no_move   = -100;
	static const Move pass_move = -1;

	bool player_must_pass = false;
	int player_to_move = 1;

	KalahaState(short start_seeds_ = 3)
		: start_seeds(start_seeds_)
	{
		for (auto& seed: player1_bins) {
			seed = start_seeds;
		}
		for (auto& seed: player2_bins) {
			seed = start_seeds;
		}
	}

	void do_move(Move move)
	{
		if (player_must_pass) {
			attest(move == pass_move);
			player_must_pass = false;
			player_to_move = 3 - player_to_move;
			return;
		}

		attest(0 <= move && move < num_bins);
		short* upper_bins = player_to_move == 1 ? player2_bins : player1_bins;
		short* lower_bins = player_to_move == 1 ? player1_bins : player2_bins;
		attest(lower_bins[move] > 0);
		auto seeds = lower_bins[move];
		lower_bins[move] = 0;

		short* lower_store = player_to_move == 1 ? &player1_store : &player2_store;

		while (seeds > 0) {
			// Move along lower bins.
			for (auto m = move + 1; m < num_bins && seeds > 0; ++m) {
				lower_bins[m] += 1;
				seeds -= 1;

				if (seeds == 0) {
					// Land in an empty square with opposite markers?
					auto opposite = num_bins - 1 - m;
					if (lower_bins[m] == 1 /* && upper_bins[opposite] > 0 */) { // Allow null capture?
						// Capture everything
						*lower_store += lower_bins[m] + upper_bins[opposite];
						lower_bins[m] = 0;
						upper_bins[opposite] = 0;
					}
				}
			}

			if (seeds == 0) {
				break;
			}

			// Put a seed in the store.
			*lower_store += 1;
			seeds -= 1;
			if (seeds == 0) {
				// Last seed in store; gets an extra turn.
				player_must_pass = true;
				break;
			}

			// Move along upper bins.
			for (short m = 0; m < num_bins && seeds > 0; ++m) {
				upper_bins[m] += 1;
				seeds -= 1;
			}
		}

		player_to_move = 3 - player_to_move;
	}

	template<typename RandomEngine>
	void do_random_move(RandomEngine* engine)
	{
		if (player_must_pass) {
			do_move(pass_move);
			return;
		}

		std::uniform_int_distribution<Move> moves(0, num_bins - 1);
		const short* bins = player_to_move == 1 ? player1_bins : player2_bins;

		while (true) {
			auto move = moves(*engine);
			if (bins[move] > 0) {
				do_move(move);
				return;
			}
		}
	}

	bool has_moves() const
	{
		if (player_must_pass) {
			return true;
		}

		const short* bins = player_to_move == 1 ? player1_bins : player2_bins;
		for (short i = 0; i < num_bins; ++i) {
			if (bins[i] > 0) {
				return true;
			}
		}
		return false;
	}

	std::vector<Move> get_moves() const
	{
		std::vector<Move> moves;

		if (player_must_pass) {
			moves.push_back(pass_move);
			return moves;
		}

		const short* bins = player_to_move == 1 ? player1_bins : player2_bins;
		for (short i = 0; i < num_bins; ++i) {
			if (bins[i] > 0) {
				moves.push_back(i);
			}
		}
		return moves;
	}

	double get_result(int current_player_to_move) const
	{
		short player1_sum = player1_store;
		for (auto seed: player1_bins) {
			player1_sum += seed;
		}

		short player2_sum = player2_store;
		for (auto seed: player2_bins) {
			player2_sum += seed;
		}

		if (player1_sum == player2_sum) {
			return 0.5;
		}

		if (player1_sum > player2_sum) {
			return current_player_to_move == 1 ? 0.0 : 1.0;
		}
		else {
			return current_player_to_move == 1 ? 1.0 : 0.0;
		}
	}

	void print(ostream& out) const
	{
		using namespace std;
		check_invariant();
		out << "Player " << player_to_move << " to move." << endl;

		auto print_bins = [&out](const short bins[num_bins], bool reverse)
		{
			out << "|    ";
			if (reverse) {
				for (short i = num_bins -1; i >= 0; --i) {
					out << setw(4) << right << bins[i];
				}
			}
			else {
				for (short i = 0; i < num_bins; ++i) {
					out << setw(4) << right << bins[i];
				}
			}
			out << "      |" << endl;
		};

		auto print_stores = [&out](short left_store, short right_store)
		{
			out << '|' 
			    << setw(4) << right << left_store
			    << "                        "
			    << setw(4) << right << right_store
			    << "  |" << endl;
		};

		auto print_moves = [&out](const short bins[num_bins])
		{
			out << "Moves:  ";
			for (short i = 0; i < num_bins; ++i) {
				if (bins[i] > 0) {
					out << i;
				}
				else {
					out << ' ';
				}
				out << setw(4) << right;
			}
			out << endl;
		};

		out << "+----------------------------------+" << endl;
		if (player_to_move == 1) {
			print_bins(player2_bins, true);
			print_stores(player2_store, player1_store);
			print_bins(player1_bins, false);
			out << "+----------------------------------+" << endl;
			print_moves(player1_bins);
		}
		else {
			print_bins(player1_bins, true);
			print_stores(player1_store, player2_store);
			print_bins(player2_bins, false);
			out << "+----------------------------------+" << endl;
			print_moves(player2_bins);
		}
	}

private:

	void check_invariant() const
	{
		short player1_sum = player1_store;
		for (auto seed: player1_bins) {
			player1_sum += seed;
		}

		short player2_sum = player2_store;
		for (auto seed: player2_bins) {
			player2_sum += seed;
		}
		attest(player1_sum + player2_sum == (num_bins * 2) * start_seeds);
	}

	short player1_bins[num_bins];
	short player2_bins[num_bins];
	short player1_store = 0;
	short player2_store = 0;

	short start_seeds;
};

template<short n>
ostream& operator << (ostream& out, const KalahaState<n>& state)
{
	state.print(out);
	return out;
}
