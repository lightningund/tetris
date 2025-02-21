#include "Rendering.hpp"

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
