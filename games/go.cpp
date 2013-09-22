// Petter Strandmark 2013.

#include <list>
#include <sstream>
#include <vector>

#define UNICODE
#include <cinder/app/AppBasic.h>
#include <cinder/gl/TextureFont.h>

#include <mcts.h>

#include "go.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the AppBasic class
class GoApp: public AppBasic
{
  public:
	GoApp();
	void setup();
	
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void update();
	void draw();

	static const int M = 9;
	static const int N = 9;

	static const int board_x = 25;
	static const int board_y = 25;
	static const int board_width = 40;

	GoState<M, N> state;

	enum PlayerType {HUMAN, COMPUTER};
	PlayerType player1, player2;
	MCTS::ComputeOptions player1_options, player2_options;

	enum GameStatus {WAITING_FOR_USER, COMPUTER_THINKING, GAME_OVER, GAME_ERROR};
	GameStatus game_status;
	std::string error_string;

	// Compute move.
	std::future<GoState<M, N>::Move> computed_move;
	void next_player();
	void start_compute_move();
	void check_for_computed_move();

	// Drawing text.
	Font				mFont;
	gl::TextureFontRef	mTextureFont;
};

GoApp::GoApp()
{ }

void GoApp::setup()
{
	//state = GoState<M, N>(board);
	player1 = COMPUTER;
	player2 = COMPUTER;

	player1_options.max_iterations = -1;
	player1_options.max_time = 2.0;

	player2_options.max_iterations = -1;
	player2_options.max_time = 0.5;

	if (player1 == HUMAN) {
		game_status = WAITING_FOR_USER;
	}
	else {
		start_compute_move();
	}

	mFont = Font("Times New Roman", 24 );
	mTextureFont = gl::TextureFont::create( mFont );
}

void GoApp::start_compute_move()
{
	//game_status = WAITING_FOR_USER;
	//return;

	game_status = COMPUTER_THINKING;

	MCTS::ComputeOptions options;
	if (state.player_to_move == 1) {
		options = player1_options;
	}
	else {
		options = player2_options;
	}

	GoState<M, N> state_copy = state;
	computed_move = 
		std::async(std::launch::async,
			[state_copy, options]() 
			{ 
				//auto best_move = MCTS::compute_move(state_copy, options);
				//return best_move;

				auto moves = state_copy.get_moves();
				if (moves.size() == 1) {
					return moves[0];
				}

				auto tree = MCTS::compute_tree(state_copy, options, 1241 * std::time(0));
				typedef MCTS::Node<GoState<M, N>> Node;
				auto best_child = *std::max_element( tree->children.begin(), tree->children.end(), [](Node* lhs, Node* rhs) { return lhs->visits < rhs->visits; } );
				return best_child->move;
			});
}

void GoApp::check_for_computed_move()
{
	//return;

	if (game_status != COMPUTER_THINKING) {
		return;
	}

	auto status = computed_move.wait_for(std::chrono::seconds(0));
	if (status == std::future_status::ready) {
		try {
			auto move = computed_move.get();
			state.do_move(move);

			// Are there any more moves possible?
			if (state.get_moves().empty()) {
				game_status = GAME_OVER;
			}
			else {
				next_player();
			}

		} catch (std::exception& error) {
			game_status = GAME_ERROR;
			error_string = error.what();
		}
	}
}

void GoApp::next_player()
{
	if (state.player_to_move == 1) {
		if (player1 == HUMAN) {
			game_status = WAITING_FOR_USER;
		}
		else {
			start_compute_move();
		}
	}
	else if (state.player_to_move == 2) {
		if (player2 == HUMAN) {
			game_status = WAITING_FOR_USER;
		}
		else {
			start_compute_move();
		}
	}
}

void GoApp::mouseDown(MouseEvent event)
{
	if (game_status != WAITING_FOR_USER) {
		return;
	}

	int x = event.getX();
	int j = (x - board_x + board_width/2) / board_width;
	int y = event.getY();
	int i = (y - board_y + board_width/2) / board_width;

	try {
		if (state.is_move_possible(i, j)) {
			state.do_move(GoState<M, N>::ij_to_ind(i, j));

			// Are there any more moves possible?
			if (state.get_moves().empty()) {
				game_status = GAME_OVER;
			}
			else {
				next_player();
			}
		}
	} catch (std::exception& error) {
		game_status = GAME_ERROR;
		error_string = error.what();
	}
}

void GoApp::keyDown( KeyEvent event )
{
	if (event.getChar() == 'p') {
		if (game_status == WAITING_FOR_USER) {
			state.do_move(GoState<M, N>::pass);

			// Computer makes the next move.
			start_compute_move();
		}
	}
	else if (event.getChar() == 'r') {
		state = GoState<M, N>();
		setup();
	}
	else if (event.getChar() == 'd') {
		state.dump_board("board.txt");
		error_string = "Board dumped.";
		game_status = GAME_ERROR;
	}
}

void GoApp::update()
{
	check_for_computed_move();
}

void GoApp::draw()
{
	gl::clear( Color( 0.9f, 0.5f, 0.05f ) );
	
	gl::color( 0.0f, 0.0f, 0.0f );
	for (int i = 0; i < M; ++i) {
		int x  = board_x + i * board_width;
		int y1 = board_y;
		int y2 = board_y + (M-1) * board_width;
		gl::drawLine(Vec2f(x, y1), Vec2f(x, y2));

		int y  = board_y + i * board_width;
		int x1 = board_x;
		int x2 = board_x + (N-1) * board_width;
		gl::drawLine(Vec2f(x1, y), Vec2f(x2, y));
	}

	if (M == 9 && N == 9) {
		gl::color( 0.0f, 0.0f, 0.0f );
		gl::drawSolidCircle(Vec2f(board_x + 2 * board_width, board_y + 2 * board_width ), 3.0f);
		gl::drawSolidCircle(Vec2f(board_x + 6 * board_width, board_y + 6 * board_width ), 3.0f);
		gl::drawSolidCircle(Vec2f(board_x + 6 * board_width, board_y + 2 * board_width ), 3.0f);
		gl::drawSolidCircle(Vec2f(board_x + 2 * board_width, board_y + 6 * board_width ), 3.0f);
		gl::drawSolidCircle(Vec2f(board_x + 4 * board_width, board_y + 4 * board_width ), 3.0f);
	}

	for (int i = 0; i < M; ++i) {
	for (int j = 0; j < N; ++j) {
		Vec2f pos(board_x + j * board_width, board_y + i * board_width );

		if ( ! state.is_move_possible(i, j)) {
			gl::color( 1.0f, 0.0f, 0.0f );	
			gl::drawSolidCircle(pos, 4.0f );
		}

		if (state.get_pos(i, j) == GoState<M, N>::player1) {
			gl::color( 0.0f, 0.0f, 0.0f );
			gl::drawSolidCircle(pos, board_width / 2 - 1);
		}
		else if (state.get_pos(i, j) == GoState<M, N>::player2) {
			gl::color( 1.0f, 1.0f, 1.0f );
			gl::drawSolidCircle(pos, board_width / 2 - 1);
		}
	}}

	stringstream sout;
	if (game_status == WAITING_FOR_USER) {
		sout << "Make your move!" << endl;
	}
	else if (game_status == COMPUTER_THINKING) {
		sout << "Computing..." << endl;
	}
	else if (game_status == GAME_OVER) {
		sout << "Game over!" << endl;
	}
	else if (game_status == GAME_ERROR) {
		sout << "ERROR:\n" << error_string << endl;
	}

	sout << "Player 1 (black) score: " << state.get_player_score(1) << endl;
	sout << "Player 2 (white) score: " << state.get_player_score(2) << endl;

	//sout << "Board hash: " << state.previous_board_hash_value << " -> " << state.compute_hash_value() << endl;
	//for (auto move: state.get_moves()) {
	//	sout << move << " ";
	//}

	Rectf boundsRect(board_x, board_y + M * board_width + 20, 
	                 getWindowWidth() - 40, getWindowHeight() - 40 );
	gl::enableAlphaBlending();
	gl::color( ColorA( 0, 0, 0, 1.0f ) );
	mTextureFont->drawStringWrapped( sout.str(), boundsRect );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( GoApp, RendererGl )
