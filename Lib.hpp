#pragma once

#include <SFML/Graphics.hpp>
#include <array>

using sf_key = sf::Keyboard::Key;

const sf::Color BLACK{0, 0, 0, 255};
const sf::Color GREY{128, 128, 128, 255};
const sf::Color WHITE{255, 255, 255, 255};
const sf::Color RED{255, 0, 0, 255};
const sf::Color GREEN{0, 255, 0, 255};
const sf::Color BLUE{0, 0, 255, 255};
const sf::Color YELLOW{255, 255, 0, 255};
const sf::Color PURPLE{255, 0, 255, 255};
const sf::Color CYAN{0, 255, 255, 255};
const sf::Color ORANGE{255, 128, 0, 255};

constexpr auto WIDTH{800};
constexpr auto HEIGHT{800};
constexpr auto NUM_CELLS_X{10};
constexpr auto NUM_CELLS_Y{25};
constexpr auto CELL_SIZE{HEIGHT / NUM_CELLS_Y};
constexpr auto PIECE_SIZE{25};
constexpr auto FLASH_FRAMES{20};
constexpr auto FRAMERATE{60};
constexpr auto UPDATEFRAMES{30};

constexpr auto ARR{3};
constexpr auto DAR{3};

constexpr bool real{false};
constexpr bool use_default_key_events{false};

constexpr float XOFF = ((WIDTH - (NUM_CELLS_X * CELL_SIZE)) / 2);

constexpr sf_key TURN_LEFT{sf::Keyboard::Q};
constexpr sf_key TURN_RIGHT{sf::Keyboard::E};
constexpr sf_key TURN_180{sf::Keyboard::W};
constexpr sf_key LEFT{sf::Keyboard::A};
constexpr sf_key RIGHT{sf::Keyboard::D};
constexpr sf_key DOWN{sf::Keyboard::S};
constexpr sf_key SWAP_HELD{sf::Keyboard::H};
constexpr sf_key KEY_DROP{sf::Keyboard::Space};

using Vec2f = sf::Vector2f;
using Vec2i = sf::Vector2i;

// Tetris piece, just 4 coordinates
using Piece = std::array<Vec2i, 4>;

// Tetris piece that can rotate
struct FullPiece {
	// The base piece
	Piece base;
	// The point to rotate around
	Vec2f center;
	// The color of the piece
	sf::Color color;
};

// Tetris piece that can exist on the board
struct MovingPiece {
	// The type of the piece
	int type{0};
	// The current rotation
	int rot{0};
	// The current position
	Vec2i pos{};

	MovingPiece();
	MovingPiece(const int type);

	const FullPiece& get_full() const;
};

struct Cell {
	bool on{false};
	sf::Color col{BLACK};
	sf::Color prev_col{BLACK};

	void reset();
};

struct Row {
	bool filled;
	int flash_timer;
	std::array<Cell, NUM_CELLS_X> cells{};

	void reset();
	void update();
};

struct Key {
	bool down;
	int held_time;
};

Piece get_piece_from_full_piece_and_rot(const FullPiece& full_piece, const int rot);
Piece get_piece_from_type_and_rot(const int type, const int rot);
Piece get_piece_from_moving_piece(const MovingPiece& moving_piece);
MovingPiece make_moving_piece(const int type, const int rot, const int x, const int y);
