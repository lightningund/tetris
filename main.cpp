#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include "Lib.hpp"
#include "Pieces.hpp"
#include "Board.hpp"

void update(Board& board, sf::RenderWindow& wind, std::map<sf_key, Key>& keys);
void render(const Board& board, sf::RenderWindow& wind);

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

bool can_drop(const Board& board, MovingPiece& piece) {
	Piece active_piece{get_piece(piece)};
	Vec2i pos{piece.pos};
	pos.y += 1;

	for (Vec2i part : active_piece) {
		Vec2i check = pos + part;
		if (check.y >= NUM_CELLS_Y || board.rows[check.y].cells[check.x].on)
			return true;
	}

	return false;
}

MovingPiece find_drop(const Board& board, const MovingPiece& piece) {
	MovingPiece phantom{piece};

	while (!can_drop(board, phantom)) {
		phantom.pos.y++;
	}

	return phantom;
}

void drop_piece(Board& board) {
	MovingPiece dropped_piece = find_drop(board, board.curr_piece);
	Vec2i pos = dropped_piece.pos;
	Piece active_piece = get_piece(dropped_piece);
	for (Vec2i part : active_piece) {
		Cell& cell = board.get_cell(pos + part);
		cell.on = true;
		cell.col = dropped_piece.get_full().color;
	}

	board.new_piece();
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
		if (!move_piece(board, Vec2i{0, 1})) drop_piece(board);

	if (check_key(keys[TURN_RIGHT], true)) // Rotate
		rotate_piece(board, 1);
	if (check_key(keys[TURN_180], true)) // Rotate
		rotate_piece(board, 2);
	if (check_key(keys[TURN_LEFT], true)) // Rotate
		rotate_piece(board, 3);
	if (check_key(keys[SWAP_HELD], true)) // Swap for held piece
		swap_held(board);
	if (check_key(keys[KEY_DROP], true)) // Quick Drop
		drop_piece(board);
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

void draw_ui_piece(bool isHeld, const Board& board, sf::RenderWindow& wind) {
	unsigned int xoff = isHeld ? PIECE_SIZE : (WIDTH - 5 * PIECE_SIZE);

	sf::RectangleShape bg{Vec2f{PIECE_SIZE * 4, PIECE_SIZE * 4}};
	bg.setPosition(static_cast<float>(xoff), PIECE_SIZE);
	bg.setFillColor(BLACK);
	bg.setOutlineColor(WHITE);
	bg.setOutlineThickness(1);
	wind.draw(bg);

	if ((isHeld && board.has_held) || !isHeld) {
		int sel_ind = isHeld ? board.held_piece : board.next_pieces[board.next_piece_ind];
		FullPiece sel_piece{pieces[sel_ind]};
		const sf::Color draw_col{sel_piece.color};
		sf::RectangleShape piece_part{Vec2f{PIECE_SIZE, PIECE_SIZE}};
		Piece active_piece{sel_piece.base};

		for (Vec2i part : active_piece) {
			piece_part.setPosition(static_cast<float>(xoff + (part.x * PIECE_SIZE)), static_cast<float>(PIECE_SIZE + (part.y * PIECE_SIZE)));
			piece_part.setOutlineThickness(0);
			piece_part.setFillColor(draw_col);
			wind.draw(piece_part);
		}
	}
}

void draw_next(const Board& board, sf::RenderWindow& wind) {
	draw_ui_piece(false, board, wind);
}

void draw_held(const Board& board, sf::RenderWindow& wind) {
	draw_ui_piece(true, board, wind);
}

void render_ui(const Board& board, sf::RenderWindow& wind) {
	draw_held(board, wind);
	draw_next(board, wind);
}

void draw_piece(const MovingPiece& piece, const sf::Color& col, sf::RenderWindow& wind) {
	Vec2i pos = piece.pos;
	Piece active_piece = get_piece(piece);

	for (Vec2i& part : active_piece) {
		sf::RectangleShape pp{Vec2f{CELL_SIZE, CELL_SIZE}};
		pp.setPosition(XOFF + ((pos.x + part.x) * CELL_SIZE), float(pos.y + part.y) * CELL_SIZE);
		pp.setFillColor(col);
		wind.draw(pp);
	}
}

void draw_piece(const MovingPiece& piece, sf::RenderWindow& wind) {
	draw_piece(piece, piece.get_full().color, wind);
}

void draw_piece(const Board& board, sf::RenderWindow& wind) {
	draw_piece(board.curr_piece, wind);
}

void pseudo_drop(const MovingPiece& piece, const Board& board, sf::RenderWindow& wind) {
	sf::Color col = piece.get_full().color;
	col.a = 100;

	MovingPiece dropped{find_drop(board, piece)};

	draw_piece(dropped, col, wind);
}

void pseudo_drop(const Board& board, sf::RenderWindow& wind) {
	pseudo_drop(board.curr_piece, board, wind);
}

void render_board(const Board& board, sf::RenderWindow& wind) {
	// Draw the black background
	sf::RectangleShape rect{Vec2f{NUM_CELLS_X * CELL_SIZE, HEIGHT}};
	rect.setPosition(XOFF, 0);
	rect.setFillColor(BLACK);
	wind.draw(rect);

	for (int i = 0; i < board.rows.size(); i++) {
		const Row& row = board.rows[i];
		for (int j = 0; j < row.cells.size(); j++) {
			const Cell& cell = row.cells[j];
			if (!cell.on) continue;

			// This needs to be somewhere else
			// If the cell is on but black revert it to it's previous color
			//if (cell.col == BLACK) cell.col = cell.prev_col;

			sf::RectangleShape cell_rect{Vec2f{CELL_SIZE, CELL_SIZE}};
			cell_rect.setPosition(j * CELL_SIZE + XOFF, i * float(CELL_SIZE));
			cell_rect.setFillColor(cell.col);
			wind.draw(cell_rect);
		}
	}

	draw_piece(board, wind);
	pseudo_drop(board, wind);
}

void render(const Board& board, sf::RenderWindow& wind) {
	render_ui(board, wind);
	render_board(board, wind);
}
