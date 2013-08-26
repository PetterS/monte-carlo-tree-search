// Petter Strandmark 2013
// petter.strandmark@gmail.com
//

#ifndef MCTS_HEADER_PETTER
#define MCTS_HEADER_PETTER

#include <iostream>
#include <string>
#include <vector>
using std::vector;
using std::size_t;

namespace MCTS
{	

template<typename State>
class Node
{
public:
	typedef typename State::Move Move;

	Node();
	~Node();

	Node* select_child() const;
	Node* add_child();
	void update(double result);

	std::string to_string() const;
	std::string tree_to_string() const;

private:
	std::string indent_string(int indent) const;
	std::string children_to_string() const;
};

}

#endif
