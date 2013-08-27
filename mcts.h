#ifndef MCTS_HEADER_PETTER
#define MCTS_HEADER_PETTER
//
// Petter Strandmark 2013
// petter.strandmark@gmail.com
//
// Monte Carlo Tree Search for finite games.
//
// Originally based on Python code at
// http://mcts.ai/code/python.html
//
// This game engine can play any game defined by a state like this:
/*

class GameState
{
public:
	typedef int Move;
	static const Move no_move = ...

	void do_move(Move move);
	void do_random_move();
	bool has_moves() const;
	std::vector<Move> get_moves() const;

	// Returns a value in [0, 1].
	double get_result(int current_player_to_move) const;

	int player_to_move;

	// ...
private:
	// ...
};

*/
//
// See the examples for more details. Given a suitable State, the
// following function (tries to) compute the best move for the
// player to move.
//

namespace MCTS
{
template<typename State>
typename State::Move compute_move(const State& root_state,
                                  const int max_iterations = 10000,
								  bool verbose = false);
}
//
//
//

#include <cstdlib>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#ifdef USE_OPENMP
#include <omp.h>
#endif

namespace MCTS
{
using std::cerr;
using std::endl;
using std::vector;
using std::size_t;

std::mt19937_64 random_engine(std::mt19937_64::default_seed);

void check(bool expr, const char* message);
void assertion_failed(const char* expr, const char* file, int line);

#define attest(expr) if (!(expr)) { ::MCTS::assertion_failed(#expr, __FILE__, __LINE__); }
#ifndef NDEBUG
	#define dattest(expr) if (!(expr)) { ::MCTS::assertion_failed(#expr, __FILE__, __LINE__); }
#else
	#define dattest(expr) ((void)0)
#endif

//
// This class is used to build the game tree. The root is created by the users and
// the rest of the tree is created by add_node.
//
template<typename State>
class Node
{
public:
	typedef typename State::Move Move;

	Node(const State& state);
	~Node();

	bool has_untried_moves() const;
	Move get_untried_move() const;
	Node* best_child() const;

	bool has_children() const
	{
		return ! children.empty();
	}

	Node* select_child_UCT() const;
	Node* add_child(const Move& move, const State& state);
	void update(double result);

	std::string to_string() const;
	std::string tree_to_string(int max_depth = 1000000, int indent = 0) const;

	const Move move;
	Node* const parent;
	const int player_to_move;
	double wins;
	int visits;

	std::vector<Move> moves;
	mutable std::vector<Node*> children;

private:
	Node(const State& state, const Move& move, Node* parent);

	std::string indent_string(int indent) const;

	Node(const Node&);
	Node& operator = (const Node&);
};


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


template<typename State>
Node<State>::Node(const State& state) :
	move(State::no_move),
	parent(nullptr),
	player_to_move(state.player_to_move),
	wins(0),
	visits(0),
	moves(state.get_moves())
{ 
	for (auto child: children) {
		delete child;
	}
}

template<typename State>
Node<State>::Node(const State& state, const Move& move_, Node* parent_) :
	move(move_),
	parent(parent_),
	player_to_move(state.player_to_move),
	wins(0),
	visits(0),
	moves(state.get_moves())
{ }

template<typename State>
Node<State>::~Node()
{ }

template<typename State>
bool Node<State>::has_untried_moves() const
{
	return ! moves.empty();
}

template<typename State>
typename State::Move Node<State>::get_untried_move() const
{
	attest( ! moves.empty());
	std::uniform_int_distribution<std::size_t> moves_distribution(0, moves.size() - 1);
	return moves[moves_distribution(random_engine)];
}

template<typename State>
typename Node<State>* Node<State>::best_child() const
{
	attest(moves.empty());

	std::sort(children.begin(), children.end(),
		[](Node* a, Node* b) { return a->visits < b->visits; });
	
	return children.back();
}

template<typename State>
Node<State>* Node<State>::select_child_UCT() const
{
	attest( ! children.empty() );

	auto key = [&] (Node* other)
	{
		return double(other->wins) / double(other->visits) +
			std::sqrt(2.0 * std::log(this->visits) / other->visits);
	};

	return *std::max_element(children.begin(), children.end(),
		[&key](Node* a, Node* b) { return key(a) < key(b); });
}

template<typename State>
Node<State>* Node<State>::add_child(const Move& move, const State& state)
{
	auto node = new Node(state, move, this);
	children.push_back(node);
	attest( ! children.empty());

	auto itr = moves.begin();
	for (; itr != moves.end() && *itr != move; ++itr);
	attest(itr != moves.end());
	moves.erase(itr);
	return node;
}

template<typename State>
void Node<State>::update(double result)
{
	visits++;
	wins += result;
}

template<typename State>
std::string Node<State>::to_string() const
{
	std::stringstream sout;
	sout << "["
	     << "P" << 3 - player_to_move << " "
	     << "M:" << move << " "
	     << "W/V: " << wins << "/" << visits << " "
	     << "U: " << moves.size() << "]\n";
	return sout.str();
}

template<typename State>
std::string Node<State>::tree_to_string(int max_depth, int indent) const
{
	if (indent >= max_depth) {
		return "";
	}

	std::string s = indent_string(indent) + to_string();
	for (auto child: children) {
		s += child->tree_to_string(max_depth, indent + 1);
	}
	return s;
}

template<typename State>
std::string Node<State>::indent_string(int indent) const
{
	std::string s = "";
	for (int i = 1; i <= indent; ++i) {
		s += "| ";
	}
	return s;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


template<typename State>
typename State::Move compute_move(const State& root_state,
                                  const int max_iterations,
								  bool verbose)
{
	// Will support more players later.
	attest(root_state.player_to_move == 1 || root_state.player_to_move == 2);
	Node<State> root(root_state);

	#ifdef USE_OPENMP
	double start_time = ::omp_get_wtime();
	double print_time = start_time;
	#endif

	for (int iter = 1; iter <= max_iterations; ++iter) {
		auto node = &root;
		State state = root_state;

		// Select a path through the tree to a leaf node.
		while (!node->has_untried_moves() && node->has_children()) {
			node = node->select_child_UCT();
			state.do_move(node->move);
		}

		// If we are not already at the final state, expand the
		// tree with a new node and move there.
		if (node->has_untried_moves()) {
			auto move = node->get_untried_move();
			state.do_move(move);
			node = node->add_child(move, state);
		}

		// We now play randomly until the game ends.
		while (state.has_moves()) {
			state.do_random_move();
		}

		// We have now reached a final state. Backpropagate the result
		// up the tree to the root node.
		while (node != nullptr) {
			node->update(state.get_result(node->player_to_move));
			node = node->parent;
		}

		#ifdef USE_OPENMP
		double time = ::omp_get_wtime();
		if (time - print_time >= 1.0 || iter == max_iterations) {
			std::cerr << iter << " games played (" << double(iter) / (time - start_time) << " / second)." << endl;
			print_time = time;
		}
		#endif
	}

	// This is the move we are going to make.
	auto best_child = root.best_child();

	if (verbose) {
		//std::cerr << root.tree_to_string(4);
		//std::cerr << endl;
		std::cerr << root.tree_to_string(2);
		std::cerr << "Best move: " << best_child->move
		          << " (" << 100.0 * best_child->visits / max_iterations << "% visits)"
		          << " (" << 100.0 * best_child->wins / best_child->visits << "% wins)" << endl;
	}

	return best_child->move;
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


void check(bool expr, const char* message)
{
	if (!expr) {
		throw std::invalid_argument(message);
	}
}

void assertion_failed(const char* expr, const char* file_cstr, int line)
{
	using namespace std;

	// Extract the file name only.
	string file(file_cstr);
	auto pos = file.find_last_of("/\\");
	if (pos == string::npos) {
		pos = 0;
	}
	file = file.substr(pos + 1);  // Returns empty string if pos + 1 == length.

	stringstream sout;
	sout << "Assertion failed: " << expr << " in " << file << ":" << line << ".";
	throw runtime_error(sout.str().c_str());
}

}

#endif
