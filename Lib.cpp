#include "Lib.hpp"
#include "Pieces.hpp"
#include <utility>

MovingPiece::MovingPiece() : MovingPiece{0} {}

MovingPiece::MovingPiece(const int type) : type{type} {}

const FullPiece& MovingPiece::get_full() const {
	return pieces[type];
}

void Cell::reset() {
	col = BLACK;
	prev_col = BLACK;
	on = false;
}

void Row::reset() {
	filled = false;
	flash_timer = -1;
	for (Cell& cell : cells) cell.reset();
}

void Row::update() {
	if (filled || flash_timer != -1) return;
	for (Cell& cell : cells) {
		if (!cell.on) return;
	}

	filled = true;
	flash_timer = FLASH_FRAMES;
}

Piece get_piece(const FullPiece& full_piece, const int rot) {
	Piece base = full_piece.base;
	Vec2f center = full_piece.center;

	int s = (rot == 3) ? -1 : rot % 2;
	int c = (rot == 2) ? -1 : (rot + 1) % 2;

	for (int i = 0; i < 4; ++i) {
		Vec2f part{base[i]};
		part -= center;
		float xnew = part.x * c - part.y * s;
		float ynew = part.x * s + part.y * c;
		base[i] = Vec2i{center + Vec2f{xnew, ynew}};
	}

	return base;
}

Piece get_piece(const int type, const int rot) {
	return get_piece(pieces[type], rot);
}

Piece get_piece(const MovingPiece& moving_piece) {
	return get_piece(moving_piece.get_full(), moving_piece.rot);
}

// Creates a MovingPiece from a type, rotation, x, and y
MovingPiece make_moving_piece(const int type, const int rot, const int x, const int y) {
	MovingPiece piece{type};

	piece.pos = Vec2i{x, y};
	piece.rot = rot;

	return piece;
}
