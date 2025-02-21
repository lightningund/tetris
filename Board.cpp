#include "Board.hpp"

void Board::init() {
	drop_time = 30;
	drop_timer = 0;
	hit_floor = false;
	score = 0;
	has_held = false;
	shuffle_pieces();
	new_piece();

	for (Row& row : rows) row.reset();
}

// Assign the current piece to the next piece and generates a new random next piece
void Board::new_piece() {
	curr_piece = MovingPiece{next_pieces[next_piece_ind]};
	curr_piece.rot = 0;
	curr_piece.pos = Vec2i{NUM_CELLS_X / 2, 0};
	next_piece_ind++;
	if (next_piece_ind == 7) shuffle_pieces();
}

// Clear a row and shifts down all other rows above it
void Board::clear_row(int rowNum) {
	for (int j = rowNum; j > 1; j--) rows[j] = rows[j - 1];
	rows[0].reset();
}

void Board::shuffle_pieces() {
	next_piece_ind = 0;
	unsigned short pieceNumbers[] = {0, 1, 2, 3, 4, 5, 6};
	for (int i = 0; i < 7; i++) {
		int randInd = (int)(rand() % 7);
		std::swap(pieceNumbers[i], pieceNumbers[randInd]);
	}

	for (int i = 0; i < 7; i++) next_pieces[i] = pieceNumbers[i];
}

Cell& Board::operator[](const Vec2i& pos) {
	return rows[pos.y].cells[pos.x];
}

Cell& Board::get_cell(Vec2i pos) {
	return rows[pos.y].cells[pos.x];
}
