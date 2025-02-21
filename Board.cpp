#include "Board.hpp"

void Board::init() {
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
	for (int j = rowNum; j > 1; --j) {
		rows[j] = rows[j - 1];
	}

	rows[0].reset();
}

void Board::shuffle_pieces() {
	next_piece_ind = 0;
	unsigned short pieceNumbers[] = {0, 1, 2, 3, 4, 5, 6};
	for (int i = 0; i < 7; ++i) {
		int randInd = (int)(rand() % 7);
		std::swap(pieceNumbers[i], pieceNumbers[randInd]);
	}

	for (int i = 0; i < 7; ++i) {
		next_pieces[i] = pieceNumbers[i];
	}
}

Cell& Board::cell_at(const Vec2i& pos) {
	return rows[pos.y].cells[pos.x];
}

const Cell& Board::cell_at(const Vec2i& pos) const {
	return rows[pos.y].cells[pos.x];
}

Cell& Board::operator[](const Vec2i& pos) {
	return cell_at(pos);
}

const Cell& Board::operator[](const Vec2i& pos) const {
	return cell_at(pos);
}

bool Board::can_drop(MovingPiece& piece) const {
	Piece active_piece{get_piece(piece)};
	Vec2i pos{piece.pos};
	pos.y += 1;

	for (Vec2i part : active_piece) {
		Vec2i check = pos + part;
		if (check.y >= NUM_CELLS_Y || cell_at(check).on) {
			return true;
		}
	}

	return false;
}

MovingPiece Board::find_drop(const MovingPiece& piece) const {
	MovingPiece phantom{piece};

	while (!can_drop(phantom)) {
		phantom.pos.y++;
	}

	return phantom;
}

void Board::drop_piece() {
	MovingPiece dropped_piece = find_drop(curr_piece);
	Vec2i pos = dropped_piece.pos;
	Piece active_piece = get_piece(dropped_piece);
	for (Vec2i part : active_piece) {
		Cell& cell = cell_at(pos + part);
		cell.on = true;
		cell.col = dropped_piece.get_full().color;
	}

	new_piece();
}

void Board::swap_held() {
	if (!has_held) {
		has_held = true;
		held_piece = curr_piece.type;
		new_piece();
	} else {
		int tempPiece = held_piece;
		held_piece = curr_piece.type;
		curr_piece = MovingPiece{tempPiece};
		curr_piece.pos = Vec2i{NUM_CELLS_X / 2, 0};
	}
}

bool Board::check_piece(const MovingPiece& piece) const {
	Piece active_piece = get_piece(piece);

	for (Vec2i part : active_piece) {
		Vec2i check = piece.pos + part;
		if (check.x >= NUM_CELLS_X || check.x < 0) return false;
		if (check.y >= NUM_CELLS_Y || check.y < 0) return false;

		if (cell_at(check).on) return false;
	}

	return true;
}

bool Board::check_piece() const {
	return check_piece(curr_piece);
}

bool Board::set_piece(const MovingPiece& phantom) {
	if (!check_piece(phantom)) return false;

	curr_piece = MovingPiece{phantom};
	return true;
}

bool Board::move_piece(const Vec2i& delta) {
	MovingPiece phantom = curr_piece;
	phantom.pos += delta;
	return set_piece(phantom);
}

bool Board::rotate_piece(int amnt) {
	MovingPiece phantom = curr_piece;
	phantom.rot += amnt;
	phantom.rot %= 4;
	return set_piece(phantom);
}
