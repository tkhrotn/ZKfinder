#include "utils.hpp"

#include <iostream>

using namespace std;
using namespace procon26;

const BitBoard procon26::UP_SHIFT_MASK[] = {
	0xFFFFFFFFFFFFFFFFull,
	0xFFFFFFFFFFFFFF00ull,
	0xFFFFFFFFFFFF0000ull,
	0xFFFFFFFFFF000000ull,
	0xFFFFFFFF00000000ull,
	0xFFFFFF0000000000ull,
	0xFFFF000000000000ull,
	0xFF00000000000000ull,
	0x0000000000000000ull
};

const BitBoard procon26::DOWN_SHIFT_MASK[] = {
	0xFFFFFFFFFFFFFFFFull,
	0x00FFFFFFFFFFFFFFull,
	0x0000FFFFFFFFFFFFull,
	0x000000FFFFFFFFFFull,
	0x00000000FFFFFFFFull,
	0x0000000000FFFFFFull,
	0x000000000000FFFFull,
	0x00000000000000FFull,
	0x0000000000000000ull
};

const BitBoard procon26::LEFT_SHIFT_MASK[] = {
	0xFFFFFFFFFFFFFFFFull,
	0xFEFEFEFEFEFEFEFEull,
	0xFCFCFCFCFCFCFCFCull,
	0xF8F8F8F8F8F8F8F8ull,
	0xF0F0F0F0F0F0F0F0ull,
	0xE0E0E0E0E0E0E0E0ull,
	0xC0C0C0C0C0C0C0C0ull,
	0x8080808080808080ull,
	0x0000000000000000ull
};

const BitBoard procon26::RIGHT_SHIFT_MASK[] = {
	0xFFFFFFFFFFFFFFFFull,
	0x7F7F7F7F7F7F7F7Full,
	0x3F3F3F3F3F3F3F3Full,
	0x1F1F1F1F1F1F1F1Full,
	0x0F0F0F0F0F0F0F0Full,
	0x0707070707070707ull,
	0x0303030303030303ull,
	0x0101010101010101ull,
	0x0000000000000000ull
};

const int procon26::ROTATION90_IDX[] = {
	7, 15, 23, 31, 39, 47, 55, 63,
	6, 14, 22, 30, 38, 46, 54, 62,
	5, 13, 21, 29, 37, 45, 53, 61,
	4, 12, 20, 28, 36, 44, 52, 60,
	3, 11, 19, 27, 35, 43, 51, 59,
	2, 10, 18, 26, 34, 42, 50, 58,
	1,  9, 17, 25, 33, 41, 49, 57,
	0,  8, 16, 24, 32, 40, 48, 56
};

const int procon26::ROTATION270_IDX[] = {
	56, 48, 40, 32, 24, 16,  8, 0,
	57, 49, 41, 33, 25, 17,  9, 1,
	58, 50, 42, 34, 26, 18, 10, 2,
	59, 51, 43, 35, 27, 19, 11, 3,
	60, 52, 44, 36, 28, 20, 12, 4,
	61, 53, 45, 37, 29, 21, 13, 5,
	62, 54, 46, 38, 30, 22, 14, 6,
	63, 55, 47, 39, 31, 23, 15, 7
};


void procon26::printBitBoard(BitBoard bitboard) {
	for (int h = 0; h < BOARD_HEIGHT; h++) {
		for (int w = 0; w < BOARD_WIDTH; w++) {
			BitBoard mask = (BitBoard)1 << (NUM_BLOCKS_IN_BITBOARD - (h*BOARD_HEIGHT + w) - 1);
			BitBoard b = bitboard & mask;

			if (b == (BitBoard)0) {
				cout << ".";
			} else {
				cout << "S";
			}
		}
		cout << endl;
	}
}
