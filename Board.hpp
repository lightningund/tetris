#pragma once

#include "Lib.hpp"
#include "Pieces.hpp"

struct Board {
	bool hit_floor{};
	bool hasHeld{};
	unsigned int held_piece{};
	unsigned int next_piece_ind{};
	unsigned int score{};
	unsigned int lines_cleared{};
	unsigned int drop_time{};
	unsigned int drop_timer{};

	MovingPiece curr_piece{};

	std::array<int, 7> nextPieces{};
	std::array<Row, NUM_CELLS_Y> rows{};

	void init();

	void newPiece();
	void clearRow(int rowNum);
	void shufflePieces();
	Cell& operator[](const Vec2i& pos);
	Cell& get_cell(Vec2i pos);
};
