/**
	Utilities for operating BitBoard variables.
*/
#ifndef __UTILS_HPP_
#define __UTILS_HPP_

#include "defines.hpp"

namespace procon26 {
	extern const BitBoard UP_SHIFT_MASK[];
	extern const BitBoard DOWN_SHIFT_MASK[];
	extern const BitBoard LEFT_SHIFT_MASK[];
	extern const BitBoard RIGHT_SHIFT_MASK[];
	extern const int ROTATION90_IDX[];
	extern const int ROTATION270_IDX[];

	void printBitBoard(BitBoard bitboard);

	inline BitBoard shiftLeft(BitBoard bitboard, int shift) {
		return (bitboard << shift) & LEFT_SHIFT_MASK[shift];
	}

	inline BitBoard shiftRight(BitBoard bitboard, int shift) {
		return (bitboard >> shift) & RIGHT_SHIFT_MASK[shift];
	}

	inline BitBoard shiftUp(BitBoard bitboard, int shift) {
		// Speed-up of: return bitboard << (shift*BOARD_HEIGHT) & UP_SHIFT_MASK[shift];
		return (bitboard << (shift << 3)) & UP_SHIFT_MASK[shift];
	}

	inline BitBoard shiftDown(BitBoard bitboard, int shift) {
		// Speed-up of: return bitboard >> (shift*BOARD_HEIGHT) & DOWN_SHIFT_MASK[shift];
		return (bitboard >> (shift << 3)) & DOWN_SHIFT_MASK[shift];
	}


	inline BitBoard reverseHorizontal(BitBoard bitboard) {
		bitboard = ((bitboard & 0x5555555555555555ull) << 1) | ((bitboard & 0xAAAAAAAAAAAAAAAAull) >> 1);
		bitboard = ((bitboard & 0x3333333333333333ull) << 2) | ((bitboard & 0xCCCCCCCCCCCCCCCCull) >> 2);
		return ((bitboard & 0x0F0F0F0F0F0F0F0Full) << 4) | ((bitboard & 0xF0F0F0F0F0F0F0F0ull) >> 4);
	}

	inline BitBoard reverseVertical(BitBoard bitboard) {
		bitboard = ((bitboard & 0x00FF00FF00FF00FFull) << 8) | ((bitboard & 0xFF00FF00FF00FF00ull) >> 8);
		bitboard = ((bitboard & 0x0000FFFF0000FFFFull) << 16) | ((bitboard & 0xFFFF0000FFFF0000ull) >> 16);
		return ((bitboard & 0x00000000FFFFFFFFull) << 32) | ((bitboard & 0xFFFFFFFF00000000ull) >> 32);
	}

	inline BitBoard rotate90(BitBoard bitboard) {
		BitBoard rotated = 0ull;

		for (int i = 0; i < NUM_BLOCKS_IN_BITBOARD; i++) {
			rotated |= ((bitboard >> ROTATION90_IDX[i]) & 1) << (NUM_BLOCKS_IN_BITBOARD - i - 1);
		}

		return rotated;
	}

	inline BitBoard rotate180(BitBoard bitboard) {
		return reverseVertical(reverseHorizontal(bitboard));
	}

	inline BitBoard rotate270(BitBoard bitboard) {
		BitBoard rotated = 0ull;

		for (int i = 0; i < NUM_BLOCKS_IN_BITBOARD; i++) {
			rotated |= ((bitboard >> ROTATION270_IDX[i]) & 1) << (NUM_BLOCKS_IN_BITBOARD - i - 1);
		}

		return rotated;
	}

	inline int countBits(BitBoard bitboard) {
		// Requires SSE4.2
		return __builtin_popcountll(bitboard);
	}

	inline BitBoard setOneBit(int sx, int sy) {
		// Speed-up of: return (0x8000000000000000ull >> sx) >> (sy * BOARD_WIDTH);
		return (0x8000000000000000ull >> sx) >> (sy << 3);
	}

	inline bool isDuplicate(BitBoard lhs, BitBoard rhs) {
		int count = countBits(lhs);

		if (count != countBits(rhs))
			return false;

		for (int x = 0; x < BOARD_WIDTH; x++) {
			for (int y = 0; y < BOARD_HEIGHT; y++) {
				BitBoard upleft = shiftUp(shiftLeft(lhs, x), y);
				BitBoard downleft = shiftDown(shiftLeft(lhs, x), y);
				BitBoard upright = shiftUp(shiftRight(lhs, x), y);
				BitBoard downright = shiftDown(shiftRight(lhs, x), y);

				if (count == countBits(upleft & rhs)
				 || count == countBits(downleft & rhs)
				 || count == countBits(upright & rhs)
				 || count == countBits(downright & rhs))
					return true;
			}
		}

		return false;
	}

	inline int countOneContact(BitBoard bitboard) {
		BitBoard l = shiftRight(bitboard, 1) & ~bitboard;
		BitBoard r = shiftLeft(bitboard, 1) & ~bitboard;
		BitBoard t = shiftDown(bitboard, 1) & ~bitboard;
		BitBoard b = shiftUp(bitboard, 1) & ~bitboard;

		BitBoard inner =  r & ~l & ~t & ~b 
            		   | ~r &  l & ~t & ~b
              		 | ~r & ~l &  t & ~b
              		 | ~r & ~l & ~t &  b;


		return countBits(inner)
		     + countBits(0x0101010101010101ull & bitboard)
		     + countBits(0x8080808080808080ull & bitboard)
		     + countBits(0xFF00000000000000ull & bitboard)
		     + countBits(0x00000000000000FFull & bitboard);
	}

	inline int countCornerContact(BitBoard bitboard) {
		BitBoard l = shiftRight(bitboard, 1) & ~bitboard;
		BitBoard r = shiftLeft(bitboard, 1) & ~bitboard;
		BitBoard t = shiftDown(bitboard, 1) & ~bitboard;
		BitBoard b = shiftUp(bitboard, 1) & ~bitboard;

		BitBoard corner =  r & ~l &  t & ~b
                    |  r & ~l & ~t &  b
                    | ~r &  l &  t & ~b
                    | ~r &  l & ~t &  b;

    return countBits(corner);
	}

	inline int countBothSideContact(BitBoard bitboard) {
		BitBoard l = shiftRight(bitboard, 1) & ~bitboard;
		BitBoard r = shiftLeft(bitboard, 1) & ~bitboard;
		BitBoard t = shiftDown(bitboard, 1) & ~bitboard;
		BitBoard b = shiftUp(bitboard, 1) & ~bitboard;

		BitBoard both_side =  r &  l & ~t & ~b
                       | ~r & ~l &  t &  b;

    return countBits(both_side);
	}

	inline int countThreeContact(BitBoard bitboard) {
		BitBoard l = shiftRight(bitboard, 1) & ~bitboard;
		BitBoard r = shiftLeft(bitboard, 1) & ~bitboard;
		BitBoard t = shiftDown(bitboard, 1) & ~bitboard;
		BitBoard b = shiftUp(bitboard, 1) & ~bitboard;

    BitBoard three =  r &  l &  t & ~b
                   |  r &  l & ~t &  b
                   |  r & ~l &  t &  b
                   | ~r &  l &  t &  b;

    return countBits(three);
	}

	inline int countFourContact(BitBoard bitboard) {
		BitBoard l = shiftRight(bitboard, 1) & ~bitboard;
		BitBoard r = shiftLeft(bitboard, 1) & ~bitboard;
		BitBoard t = shiftDown(bitboard, 1) & ~bitboard;
		BitBoard b = shiftUp(bitboard, 1) & ~bitboard;

    BitBoard four = r & l & t & b;

    return countBits(four);
	}

	inline int findTopmost(BitBoard bitboard) {
		int topleft_idx = __builtin_clzll(bitboard);
    return topleft_idx >> 3;
	}

	inline int findBottommost(BitBoard bitboard) {
		int bottomright_idx = __builtin_ctzll(bitboard);
    return BOARD_HEIGHT - 1 - (bottomright_idx >> 3);
	}

	inline int findLeftmost(BitBoard bitboard) {
		if (bitboard & 0x8080808080808080ull) {
      return 0;
    } else if (bitboard & 0x4040404040404040ull) {
      return 1;
    } else if (bitboard & 0x2020202020202020ull) {
      return 2;
    } else if (bitboard & 0x1010101010101010ull) {
      return 3;
    } else if (bitboard & 0x0808080808080808ull) {
      return 4;
    } else if (bitboard & 0x0404040404040404ull) {
      return 5;
    } else if (bitboard & 0x0202020202020202ull) {
      return 6;
    } else if (bitboard & 0x0101010101010101ull) {
      return 7;
    }
	}

	inline int findRightmost(BitBoard bitboard) {
    if (bitboard & 0x0101010101010101ull) {
      return 7;
    } else if (bitboard & 0x0202020202020202ull) {
      return 6;
    } else if (bitboard & 0x0404040404040404ull) {
      return 5;
    } else if (bitboard & 0x0808080808080808ull) {
      return 4;
    } else if (bitboard & 0x1010101010101010ull) {
      return 3;
    } else if (bitboard & 0x2020202020202020ull) {
      return 2;
    } else if (bitboard & 0x4040404040404040ull) {
      return 1;
    } else if (bitboard & 0x8080808080808080ull) {
      return 0;
    } 
	}

  inline bool isLtype(BitBoard bitboard) {
    if (countBits(bitboard) != 3) 
      return false;

    BitBoard r = shiftRight(bitboard, 1);
    BitBoard l = shiftLeft(bitboard, 1);
    BitBoard u = shiftUp(bitboard, 1);
    BitBoard d = shiftDown(bitboard, 1);

    return (bitboard & r & u) || (bitboard & r & d) || (bitboard & l & u) || (bitboard & l & d);
  }

  inline bool isItype(BitBoard bitboard) {
    if (countBits(bitboard) != 3)
      return false;

    return !isLtype(bitboard);
  }
}

#endif // __UTILS_HPP_
