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

// Check if the key is down and is ready to be processed again
static bool check_key(const Key& key, const bool once_key) {
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

static void key_pressed(std::map<sf_key, Key>& keys, const sf_key& key) {
	if (keys.contains(key) && keys[key].down) return;

	keys[key] = Key{true, 0};
}

static void key_released(std::map<sf_key, Key>& keys, const sf_key& key) {
	keys[key].down = false;
}

static void check_events(sf::RenderWindow& wind, std::map<sf_key, Key>& keys) {
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

static void check_inputs(Board& board, std::map<sf_key, Key>& keys) {
	if (check_key(keys[LEFT], false))
		board.move_piece(Vec2i{-1, 0});
	if (check_key(keys[RIGHT], false))
		board.move_piece(Vec2i{1, 0});
	if (check_key(keys[DOWN], false))
		if (!board.move_piece(Vec2i{0, 1})) {
			board.drop_piece();
		}

	if (check_key(keys[TURN_RIGHT], true)) // Rotate
		board.rotate_piece(1);
	if (check_key(keys[TURN_180], true)) // Rotate
		board.rotate_piece(2);
	if (check_key(keys[TURN_LEFT], true)) // Rotate
		board.rotate_piece(3);
	if (check_key(keys[SWAP_HELD], true)) // Swap for held piece
		board.swap_held();
	if (check_key(keys[KEY_DROP], true)) // Quick Drop
		board.drop_piece();
}

static void clear_lines(Board& board) {
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

static void update(Board& board, sf::RenderWindow& wind, std::map<sf_key, Key>& keys) {
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
