#include "stone.hpp"

#include <sstream>

using namespace std;
using namespace procon26;

Stone::Stone() : _bit_stone(0), _pass(false), _size(0) {
}

void Stone::readProblemString(string problem_string) {
  stringstream ss(problem_string);
	string tmp;

	for (int by = 0; by < BOARD_HEIGHT; by++) {
		string tmp;
		getline(ss, tmp);
		for (int bx = 0; bx < BOARD_WIDTH; bx++) {
			if (tmp[bx] != '0') {
				_bit_stone |= 1ull << (NUM_BLOCKS_IN_BITBOARD - (by*BOARD_HEIGHT + bx) - 1);
      }
    }
  }

  _reversed = false;
  _rotate = ZERO;

  _size = countBits(_bit_stone);
  _one = countOneContact(_bit_stone);
  _corner = countCornerContact(_bit_stone);
  _both_side = countBothSideContact(_bit_stone);
  _three = countThreeContact(_bit_stone);
  _four = countFourContact(_bit_stone);
  _inv_one = countOneContact(~_bit_stone);
  _inv_corner = countCornerContact(~_bit_stone);
  _inv_both_side = countBothSideContact(~_bit_stone);
  _inv_three = countThreeContact(~_bit_stone);
  _inv_four = countFourContact(~_bit_stone);

  setEdge();
}
