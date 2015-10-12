#ifndef __DEFINES_HPP_
#define __DEFINES_HPP_

typedef unsigned long long BitBoard;

#define NUM_BLOCKS_IN_BITBOARD 64
#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8
#define MAX_STONE 256
#define NUM_PLACEMENT_PATTERN 9
#define PASS_PATTERN 8
#define UNPLACED_PATTERN -1
#define MAX_BLANK 1024

enum Rotate {
	ZERO, ONE, TWO, THREE
};

#endif // __DEFINES_HPP_
