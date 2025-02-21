#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include "Lib.hpp"
#include "Pieces.hpp"
#include "Board.hpp"
#include "Rendering.hpp"

void update(Board& board, sf::RenderWindow& wind, std::map<sf_key, Key>& keys);

int main() {
	Board board;

	std::map<sf_key, Key> keys;

	sf::RenderWindow wind{sf::VideoMode(WIDTH, HEIGHT), "Tetris"};
	wind.setFramerateLimit(60);

	srand((unsigned int)time(0));

	board.init();

	while (wind.isOpen()) { //Main Loop
		wind.clear(GREY);
		update(board, wind, keys);
		render(board, wind);
		wind.display();
	}

	return 0;
}

bool check_piece(const Board& board, const MovingPiece& piece) {
	Piece active_piece = get_piece(piece);

	for (Vec2i part : active_piece) {
		int checkX = piece.pos.x + part.x;
		if (checkX >= NUM_CELLS_X || checkX < 0) return false;

		int checkY = piece.pos.y + part.y;
		if (checkY >= NUM_CELLS_Y || checkY < 0) return false;

		if (board.rows[checkY].cells[checkX].on) return false;
	}

	return true;
}

bool check_piece(const Board& board) {
	return check_piece(board, board.curr_piece);
}

bool set_piece(Board& board, const MovingPiece& phantom) {
	if (!check_piece(board, phantom)) return false;

	board.curr_piece = MovingPiece{phantom};
	return true;
}

bool move_piece(Board& board, const Vec2i& delta) {
	MovingPiece phantom = board.curr_piece;
	phantom.pos += delta;
	return set_piece(board, phantom);
}

bool rotate_piece(Board& board, int amnt) {
	MovingPiece phantom = board.curr_piece;
	phantom.rot += amnt;
	phantom.rot %= 4;
	return set_piece(board, phantom);
}

// Check if the key is down and is ready to be processed again
bool check_key(const Key& key, const bool once_key) {
	if (key.down) {
		if (key.held_time == 0) {
			return true;
		}
		if (!once_key) {
			if (key.held_time - DAR > 0) {
				if ((key.held_time - DAR) % ARR == 0) {
					return true;
				}
			}
		}
	}

	return false;
}

void key_pressed(std::map<sf_key, Key>& keys, const sf_key& key) {
	if (keys.contains(key) && keys[key].down) return;

	keys[key] = Key{true, 0};
}

void key_released(std::map<sf_key, Key>& keys, const sf_key& key) {
	keys[key].down = false;
}

void check_events(sf::RenderWindow& wind, std::map<sf_key, Key>& keys) {
	// Loop Through all the events that happened this frame
	sf::Event event;
	while (wind.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			wind.close();
		else if (event.type == sf::Event::KeyPressed)
			key_pressed(keys, event.key.code);
		else if (event.type == sf::Event::KeyReleased)
			key_released(keys, event.key.code);
	}
}

void swap_held(Board& board) {
	if (!board.has_held) {
		board.has_held = true;
		board.held_piece = board.curr_piece.type;
		board.new_piece();
	} else {
		int tempPiece = board.held_piece;
		board.held_piece = board.curr_piece.type;
		board.curr_piece = MovingPiece{tempPiece};
		board.curr_piece.pos = Vec2i{NUM_CELLS_X / 2, 0};
	}
}

void check_inputs(Board& board, std::map<sf_key, Key>& keys) {
	if (check_key(keys[LEFT], false))
		move_piece(board, Vec2i{-1, 0});
	if (check_key(keys[RIGHT], false))
		move_piece(board, Vec2i{1, 0});
	if (check_key(keys[DOWN], false))
		if (!move_piece(board, Vec2i{0, 1})) board.drop_piece();

	if (check_key(keys[TURN_RIGHT], true)) // Rotate
		rotate_piece(board, 1);
	if (check_key(keys[TURN_180], true)) // Rotate
		rotate_piece(board, 2);
	if (check_key(keys[TURN_LEFT], true)) // Rotate
		rotate_piece(board, 3);
	if (check_key(keys[SWAP_HELD], true)) // Swap for held piece
		swap_held(board);
	if (check_key(keys[KEY_DROP], true)) // Quick Drop
		board.drop_piece();
}

void clear_lines(Board& board) {
	int lines = 0;
	for (int i = 0; i < board.rows.size(); i++) {
		Row& row = board.rows[i];
		row.update();
		if (row.filled) {
			if (row.flash_timer == 0) {
				lines++;
				row.flash_timer = -1;
				board.clear_row(i);
			} else {
				uint8_t flash_col = ((row.flash_timer >> 2) % 2) * 255;
				row.flash_timer--;

				for (Cell& cell : row.cells) {
					cell.col = sf::Color{flash_col, flash_col, flash_col};
				}
			}
		}
	}

	board.score += lines * lines;

	if (lines != 0) std::cout << board.score << "\n";
}

void update(Board& board, sf::RenderWindow& wind, std::map<sf_key, Key>& keys) {
	check_events(wind, keys);
	check_inputs(board, keys);

	clear_lines(board);

	for (auto& [code, val] : keys) {
		if (val.down) {
			if (use_default_key_events) {
				val.down = false;
				val.held_time = 0;
			} else val.held_time++;
		}
	}
}
