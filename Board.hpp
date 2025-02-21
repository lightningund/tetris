#pragma once

#include "Lib.hpp"
#include "Pieces.hpp"

struct Board {
	bool hit_floor{};
	bool has_held{};
	unsigned int held_piece{};
	unsigned int next_piece_ind{};
	unsigned int score{};
	unsigned int lines_cleared{};

	MovingPiece curr_piece{};

	std::array<int, 7> next_pieces{};
	std::array<Row, NUM_CELLS_Y> rows{};

	void init();

	void new_piece();
	void clear_row(int rowNum);
	void shuffle_pieces();
	Cell& operator[](const Vec2i& pos);
	const Cell& operator[](const Vec2i& pos) const;
	bool can_drop(MovingPiece& piece) const;
	MovingPiece find_drop(const MovingPiece& piece) const;
	void drop_piece();
	void swap_held();

	bool check_piece(const MovingPiece& piece) const;
	bool check_piece() const;
	bool set_piece(const MovingPiece& phantom);
	bool move_piece(const Vec2i& delta);
	bool rotate_piece(int amnt);

	private:
	Cell& cell_at(const Vec2i& pos);
	const Cell& cell_at(const Vec2i& pos) const;
};
