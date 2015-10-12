#include "ground.hpp"

#include "utils.hpp"
#include "stone.hpp"
#include "globals.hpp"

#include <iostream>
#include <sstream>

using namespace procon26;
using namespace std;

const int Ground::GROUND_BITBOARD_SIZE;
const int Ground::GROUND_BITBOARD_WIDTH;
const int Ground::GROUND_BITBOARD_HEIGHT;
const int Ground::OUTER_IDX[] = {0, 1, 2, 3, 4, 5, 6, 11, 12, 17, 18, 23, 24, 29, 30, 31, 32, 33, 34, 35};
const int Ground::OUTER_IDX_SIZE;
const int Ground::INNER_IDX[] = {7, 8, 9, 10, 13, 14, 15, 16, 19, 20, 21, 22, 25, 26, 27, 28};
const int Ground::INNER_IDX_SIZE;
const int Ground::INNER_IDX_LEFT[] = {8, 16, 24, 32, 8, 16, 24, 32, 8, 16, 24, 32, 8, 16, 24, 32};
const int Ground::INNER_IDX_TOP[] = {8, 8, 8, 8, 16, 16, 16, 16, 24, 24, 24, 24, 32, 32, 32, 32};
const int Ground::INNER_BLOCK_WIDTH;
const int Ground::INNER_BLOCK_HEIGHT;
const int Ground::WHOLE_BLOCK_WIDTH;
const int Ground::WHOLE_BLOCK_HEIGHT;

const char Ground::DEFAULT_COLOR[] = "\x1b[49m";
const char Ground::OBSTACLE_COLOR[] = "\x1b[47m"; 
const char Ground::STONE_COLOR[] = "\x1b[46m";

Ground::Ground() {
  for (int oi = 0; oi < OUTER_IDX_SIZE; oi++) {
    _obstacle[OUTER_IDX[oi]] = 0xFFFFFFFFFFFFFFFFull;
  }
}


void Ground::readProblemString(string problem_string) {
  stringstream ss(problem_string);
  string tmp;

  for (int gy = 1; gy < GROUND_BITBOARD_HEIGHT - 1; gy++) {
    for (int by = 0; by < BOARD_HEIGHT; by++) {
      string tmp;
      getline(ss, tmp);
      for (int gx = 1; gx < GROUND_BITBOARD_WIDTH - 1; gx++) {
        for (int bx = 0; bx < BOARD_WIDTH; bx++) {
          if (tmp[(gx - 1)*BOARD_WIDTH + bx] != '0') {
            BitBoard mask = 1ull << (NUM_BLOCKS_IN_BITBOARD - (by*BOARD_WIDTH + bx) - 1);
            _obstacle[gy*GROUND_BITBOARD_HEIGHT + gx] |= mask;
          }
        }
      }
    }
  }

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    arrangeContact(_left_contact, _obstacle[INNER_IDX[ii]], INNER_IDX_LEFT[ii] + 1, INNER_IDX_TOP[ii]);
    arrangeContact(_right_contact, _obstacle[INNER_IDX[ii]], INNER_IDX_LEFT[ii] - 1, INNER_IDX_TOP[ii]);
    arrangeContact(_top_contact, _obstacle[INNER_IDX[ii]], INNER_IDX_LEFT[ii], INNER_IDX_TOP[ii] + 1);
    arrangeContact(_bottom_contact, _obstacle[INNER_IDX[ii]], INNER_IDX_LEFT[ii], INNER_IDX_TOP[ii] - 1);
  }
  // mark ourter blocks as contacting
  _left_contact[7]  |= 0x8080808080808080ull;
  _left_contact[13] |= 0x8080808080808080ull;
  _left_contact[19] |= 0x8080808080808080ull;
  _left_contact[25] |= 0x8080808080808080ull;
  _right_contact[10] |= 0x0101010101010101ull;
  _right_contact[16] |= 0x0101010101010101ull;
  _right_contact[22] |= 0x0101010101010101ull;
  _right_contact[28] |= 0x0101010101010101ull;
  _top_contact[7]  |= 0xFF00000000000000ull;
  _top_contact[8]  |= 0xFF00000000000000ull;
  _top_contact[9]  |= 0xFF00000000000000ull;
  _top_contact[10] |= 0xFF00000000000000ull;
  _bottom_contact[25] |= 0x00000000000000FFull;
  _bottom_contact[26] |= 0x00000000000000FFull;
  _bottom_contact[27] |= 0x00000000000000FFull;
  _bottom_contact[28] |= 0x00000000000000FFull;

  updateContactAndClosedArray();
}

void Ground::updateContactAndClosedArray() {
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    _contact[gi] &= ~_obstacle[gi];
    _left_contact[gi] &= ~_obstacle[gi];
    _right_contact[gi] &= ~_obstacle[gi];
    _top_contact[gi] &= ~_obstacle[gi];
    _bottom_contact[gi] &= ~_obstacle[gi];
  }

  for (int oi = 0; oi < OUTER_IDX_SIZE; oi++) {
    _contact[OUTER_IDX[oi]] = 0ull;
    _left_contact[OUTER_IDX[oi]] = 0ull;
    _right_contact[OUTER_IDX[oi]] = 0ull;
    _top_contact[OUTER_IDX[oi]] = 0ull;
    _bottom_contact[OUTER_IDX[oi]] = 0ull;
  }

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    BitBoard r = _right_contact[INNER_IDX[ii]];
    BitBoard l = _left_contact[INNER_IDX[ii]];
    BitBoard t = _top_contact[INNER_IDX[ii]];
    BitBoard b = _bottom_contact[INNER_IDX[ii]];

    _one_contact[INNER_IDX[ii]] =  r & ~l & ~t & ~b 
                                | ~r &  l & ~t & ~b
                                | ~r & ~l &  t & ~b
                                | ~r & ~l & ~t &  b;

    _corner_contact[INNER_IDX[ii]] =  r & ~l &  t & ~b
                                   |  r & ~l & ~t &  b
                                   | ~r &  l &  t & ~b
                                   | ~r &  l & ~t &  b;

    _both_side_contact[INNER_IDX[ii]] =  r &  l & ~t & ~b
                                      | ~r & ~l &  t &  b;

    _three_contact[INNER_IDX[ii]] =  r &  l &  t & ~b
                                  |  r &  l & ~t &  b
                                  |  r & ~l &  t &  b
                                  | ~r &  l &  t &  b;

    _four_contact[INNER_IDX[ii]] = r & l & t & b;
  }

  BitBoard array[GROUND_BITBOARD_SIZE];

  BitBoard sr[GROUND_BITBOARD_SIZE], sl[GROUND_BITBOARD_SIZE], su[GROUND_BITBOARD_SIZE], sd[GROUND_BITBOARD_SIZE];
  shiftArray(_three_contact, sr, 1, 0);
  shiftArray(_three_contact, sl, -1, 0);
  shiftArray(_three_contact, su, 0, -1);
  shiftArray(_three_contact, sd, 0, 1);

  BitBoard asr[GROUND_BITBOARD_SIZE], asl[GROUND_BITBOARD_SIZE], asu[GROUND_BITBOARD_SIZE], asd[GROUND_BITBOARD_SIZE];
  andArray(_three_contact, sr, asr);
  andArray(_three_contact, sl, asl);
  andArray(_three_contact, su, asu);
  andArray(_three_contact, sd, asd);

  clearArray(_two_closed);
  orArray(_two_closed, asr, _two_closed);
  orArray(_two_closed, asl, _two_closed);
  orArray(_two_closed, asu, _two_closed);
  orArray(_two_closed, asd, _two_closed);

  andArray(_corner_contact, sr, asr);
  andArray(_corner_contact, sl, asl);
  andArray(_corner_contact, su, asu);
  andArray(_corner_contact, sd, asd);

  BitBoard ru[GROUND_BITBOARD_SIZE], rd[GROUND_BITBOARD_SIZE], lu[GROUND_BITBOARD_SIZE], ld[GROUND_BITBOARD_SIZE];
  andArray(asr, asu, ru);
  andArray(asr, asd, rd);
  andArray(asl, asu, lu);
  andArray(asl, asd, ld);

  clearArray(_ltype_closed);
  orArray(_ltype_closed, ru, _ltype_closed);
  orArray(_ltype_closed, rd, _ltype_closed);
  orArray(_ltype_closed, lu, _ltype_closed);
  orArray(_ltype_closed, ld, _ltype_closed);

  andArray(_both_side_contact, sr, asr);
  andArray(_both_side_contact, sl, asl);
  andArray(_both_side_contact, su, asu);
  andArray(_both_side_contact, sd, asd);

  BitBoard rl[GROUND_BITBOARD_SIZE], ud[GROUND_BITBOARD_SIZE];
  andArray(asr, asl, rl);
  andArray(asu, asd, ud);

  clearArray(_itype_closed);
  orArray(_itype_closed, rl, _itype_closed);
  orArray(_itype_closed, ud, _itype_closed);
}

void Ground::print(void) {
  cout << OBSTACLE_COLOR;
  for (int i = 0; i < INNER_BLOCK_WIDTH + 2; i++) {
    cout << "  ";
  }
  cout << DEFAULT_COLOR;
  cout << endl;
  for (int gy = 1; gy < GROUND_BITBOARD_HEIGHT-1; gy++) {
    for (int by = 0; by < BOARD_HEIGHT; by++) {
      cout << OBSTACLE_COLOR << "  " << DEFAULT_COLOR;
      for (int gx = 1; gx < GROUND_BITBOARD_WIDTH-1; gx++) {
        for (int bx = 0; bx < BOARD_WIDTH; bx++) {
          BitBoard mask = 1ull << (NUM_BLOCKS_IN_BITBOARD - (by*BOARD_WIDTH + bx) - 1);

          BitBoard initial = g_initial_ground._obstacle[gy * GROUND_BITBOARD_HEIGHT + gx] & mask;
          BitBoard o = _obstacle[gy * GROUND_BITBOARD_HEIGHT + gx] & mask;
          BitBoard c = _contact[gy * GROUND_BITBOARD_HEIGHT + gx] & mask;

          if (o != 0ull) {
            if (initial != 0ull)
              cout << OBSTACLE_COLOR;
            else
              cout << STONE_COLOR;

            cout << "  ";
            cout << DEFAULT_COLOR;
          } else {
            cout << "  ";
          }
        }
      }
      cout << OBSTACLE_COLOR << "  " << DEFAULT_COLOR << endl;
    }
  }
  cout << OBSTACLE_COLOR;
  for (int i = 0; i < INNER_BLOCK_WIDTH + 2; i++) {
    cout << "  ";
  }
  cout << DEFAULT_COLOR;
  cout << endl;
}


bool Ground::collideWith(Stone &stone_to_arrange, int bx, int by) {
  // speed-up: int gx = bx / BOARD_WIDTH, gy = by / BOARD_HEIGHT;
  int gx = bx >> 3, gy = by >> 3;

  // speed-up: int remx = bx % BOARD_WIDTH, remy = by % BOARD_HEIGHT;
  int remx = bx & 7, remy = by & 7;

  BitBoard arrange = stone_to_arrange.getBitBoard();

  BitBoard current = shiftDown(shiftRight(arrange, remx), remy);
  BitBoard right = shiftDown(shiftLeft(arrange, BOARD_WIDTH - remx), remy);
  BitBoard bottom = shiftUp(shiftRight(arrange, remx), BOARD_HEIGHT - remy);
  BitBoard right_bottom = shiftUp(shiftLeft(arrange, BOARD_WIDTH - remx), BOARD_HEIGHT - remy);

  return (_obstacle[gy * GROUND_BITBOARD_WIDTH + gx] & current)
      || (_obstacle[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
      || (_obstacle[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
      || (_obstacle[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom);
}


bool Ground::contactWithStones(Stone &stone_to_arrange, int bx, int by) {
  // speed-up: int gx = bx / BOARD_WIDTH, gy = by / BOARD_HEIGHT;
  int gx = bx >> 3, gy = by >> 3;

  // speed-up: int remx = bx % BOARD_WIDTH, remy = by % BOARD_HEIGHT;
  int remx = bx & 7, remy = by & 7;

  BitBoard arrange = stone_to_arrange.getBitBoard();

  BitBoard current = shiftDown(shiftRight(arrange, remx), remy);
  BitBoard right = shiftDown(shiftLeft(arrange, BOARD_WIDTH - remx), remy);
  BitBoard bottom = shiftUp(shiftRight(arrange, remx), BOARD_HEIGHT - remy);
  BitBoard right_bottom = shiftUp(shiftLeft(arrange, BOARD_WIDTH - remx), BOARD_HEIGHT - remy);

  return (_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
      || (_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
      || (_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
      || (_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom);
}


bool Ground::checkNotCollideAndContact(Stone &stone_to_arrange, int bx, int by) {
  // speed-up: int gx = bx / BOARD_WIDTH, gy = by / BOARD_HEIGHT;
  int gx = bx >> 3, gy = by >> 3;

  // speed-up: int remx = bx % BOARD_WIDTH, remy = by % BOARD_HEIGHT;
  int remx = bx & 7, remy = by & 7;

  BitBoard arrange = stone_to_arrange.getBitBoard();

  BitBoard current = shiftDown(shiftRight(arrange, remx), remy);
  BitBoard right = shiftDown(shiftLeft(arrange, BOARD_WIDTH - remx), remy);
  BitBoard bottom = shiftUp(shiftRight(arrange, remx), BOARD_HEIGHT - remy);
  BitBoard right_bottom = shiftUp(shiftLeft(arrange, BOARD_WIDTH - remx), BOARD_HEIGHT - remy);

  int c_idx = gy * GROUND_BITBOARD_WIDTH + gx;
  int r_idx = gy * GROUND_BITBOARD_WIDTH + gx + 1;
  int b_idx = (gy + 1) * GROUND_BITBOARD_WIDTH + gx;
  int rb_idx = (gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1;

  bool collide = (_obstacle[c_idx] & current)
      || (_obstacle[r_idx] & right)
      || (_obstacle[b_idx] & bottom)
      || (_obstacle[rb_idx] & right_bottom);

  bool contact = (_contact[c_idx] & current)
      || (_contact[r_idx] & right)
      || (_contact[b_idx] & bottom)
      || (_contact[rb_idx] & right_bottom);

  return !collide && contact;
}


int Ground::calculateNumBlockInNeumannNeighborhood(Stone &stone_to_arrange, int bx, int by) {
  // speed-up: int gx = bx / BOARD_WIDTH, gy = by / BOARD_HEIGHT;
  int gx = bx >> 3, gy = by >> 3;

  // speed-up: int remx = bx % BOARD_WIDTH, remy = by % BOARD_HEIGHT;
  int remx = bx & 7ull, remy = by & 7ull;

  BitBoard arrange = stone_to_arrange.getBitBoard();

  BitBoard current = shiftDown(shiftRight(arrange, remx), remy);
  BitBoard right = shiftDown(shiftLeft(arrange, BOARD_WIDTH - remx), remy);
  BitBoard bottom = shiftUp(shiftRight(arrange, remx), BOARD_HEIGHT - remy);
  BitBoard right_bottom = shiftUp(shiftLeft(arrange, BOARD_WIDTH - remx), BOARD_HEIGHT - remy);

  return   countBits(_left_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
       + countBits(_left_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
       + countBits(_left_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
       + countBits(_left_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom)
       + countBits(_right_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
       + countBits(_right_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
       + countBits(_right_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
       + countBits(_right_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom)
       + countBits(_top_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
       + countBits(_top_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
       + countBits(_top_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
       + countBits(_top_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom)
       + countBits(_bottom_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
       + countBits(_bottom_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
       + countBits(_bottom_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
       + countBits(_bottom_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom);
}


int Ground::calculateNumBlockInNeumannNeighborhoodWithWeight(Stone &stone_to_arrange, int bx, int by) {
  // speed-up: int gx = bx / BOARD_WIDTH, gy = by / BOARD_HEIGHT;
  int gx = bx >> 3, gy = by >> 3;

  // speed-up: int remx = bx % BOARD_WIDTH, remy = by % BOARD_HEIGHT;
  int remx = bx & 7ull, remy = by & 7ull;

  BitBoard arrange = stone_to_arrange.getBitBoard();

  BitBoard current = shiftDown(shiftRight(arrange, remx), remy);
  BitBoard right = shiftDown(shiftLeft(arrange, BOARD_WIDTH - remx), remy);
  BitBoard bottom = shiftUp(shiftRight(arrange, remx), BOARD_HEIGHT - remy);
  BitBoard right_bottom = shiftUp(shiftLeft(arrange, BOARD_WIDTH - remx), BOARD_HEIGHT - remy);

  int one = countBits(_one_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
       + countBits(_one_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
       + countBits(_one_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
       + countBits(_one_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom);
  int corner = countBits(_corner_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
       + countBits(_corner_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
       + countBits(_corner_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
       + countBits(_corner_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom);
  int both_side = countBits(_both_side_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
       + countBits(_both_side_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
       + countBits(_both_side_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
       + countBits(_both_side_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom);
  int three = countBits(_three_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
       + countBits(_three_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
       + countBits(_three_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
       + countBits(_three_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom);
  int four = countBits(_four_contact[gy * GROUND_BITBOARD_WIDTH + gx] & current)
       + countBits(_four_contact[gy * GROUND_BITBOARD_WIDTH + gx + 1] & right)
       + countBits(_four_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] & bottom)
       + countBits(_four_contact[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] & right_bottom);

  return 1 * one + 2 * corner + 2 * both_side + 3 * three + 4 * four;
}


int Ground::calculateNumOneContact() {
  int num_one = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_one += countBits(_one_contact[gi]);
  }

  return num_one;
}


int Ground::calculateNumCornerContact() {
  int num_corner = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_corner += countBits(_corner_contact[gi]);
  }

  return num_corner;
}


int Ground::calculateNumBothSideContact() {
  int num_both_side = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_both_side += countBits(_both_side_contact[gi]);
  }

  return num_both_side;
}


int Ground::calculateNumThreeContact() {
  int num_three = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_three += countBits(_three_contact[gi]);
  }

  return num_three;
}


int Ground::calculateNumFourContact() {
  int num_four = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_four += countBits(_four_contact[gi]);
  }

  return num_four;
}


int Ground::calculateNumTwoClosed() {
  int num_two = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_two += countBits(_two_closed[gi]);
  }

  return num_two >> 1; // Speed-up: return num_two / 2;
}


int Ground::calculateNumLtypeClosed() {
  int num_ltype = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_ltype += countBits(_ltype_closed[gi]);
  }

  return num_ltype;
}


int Ground::calculateNumItypeClosed() {
  int num_itype = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_itype += countBits(_itype_closed[gi]);
  }

  return num_itype;
}


inline void Ground::andArray(BitBoard lhs[], BitBoard rhs[], BitBoard result[]) {
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    result[gi] = lhs[gi] & rhs[gi];
  }  
}


inline void Ground::orArray(BitBoard lhs[], BitBoard rhs[], BitBoard result[]) {
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    result[gi] = lhs[gi] | rhs[gi];
  }  
}


inline void Ground::shiftArray(BitBoard src[], BitBoard dst[], int x, int y) {
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    dst[gi] = 0ull;
  }

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    arrangeContact(dst, src[INNER_IDX[ii]], INNER_IDX_LEFT[ii] + x, INNER_IDX_TOP[ii] + y);
  }
}

inline void Ground::copy(BitBoard src[], BitBoard dst[]) {
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    dst[gi] = src[gi];
  }
}


inline void Ground::clearArray(BitBoard array[]) {
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    array[gi] = 0ull;
  }  
}


inline void Ground::arrangeContact(BitBoard array[], BitBoard arrange, int bx, int by) {
  // speed-up: int gx = bx / BOARD_WIDTH, gy = by / BOARD_HEIGHT;
  int gx = bx >> 3, gy = by >> 3;

  // speed-up: int remx = bx % BOARD_WIDTH, remy = by % BOARD_HEIGHT;
  int remx = bx & 7ull, remy = by & 7ull;

  BitBoard current = shiftDown(shiftRight(arrange, remx), remy);
  BitBoard right = shiftDown(shiftLeft(arrange, BOARD_WIDTH - remx), remy);
  BitBoard bottom = shiftUp(shiftRight(arrange, remx), BOARD_HEIGHT - remy);
  BitBoard right_bottom = shiftUp(shiftLeft(arrange, BOARD_WIDTH - remx), BOARD_HEIGHT - remy);

  array[gy * GROUND_BITBOARD_WIDTH + gx] |= current;
  array[gy * GROUND_BITBOARD_WIDTH + gx + 1] |= right;
  array[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] |= bottom;
  array[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] |= right_bottom;
}


void Ground::arrange(Stone &stone_to_arrange, int bx, int by) {
  if (stone_to_arrange.isPassed())
    return;

  // speed-up: int gx = bx / BOARD_WIDTH, gy = by / BOARD_HEIGHT;
  int gx = bx >> 3, gy = by >> 3;

  // speed-up: int remx = bx % BOARD_WIDTH, remy = by % BOARD_HEIGHT;
  int remx = bx & 7ull, remy = by & 7ull;

  BitBoard arrange = stone_to_arrange.getBitBoard();

  BitBoard current = shiftDown(shiftRight(arrange, remx), remy);
  BitBoard right = shiftDown(shiftLeft(arrange, BOARD_WIDTH - remx), remy);
  BitBoard bottom = shiftUp(shiftRight(arrange, remx), BOARD_HEIGHT - remy);
  BitBoard right_bottom = shiftUp(shiftLeft(arrange, BOARD_WIDTH - remx), BOARD_HEIGHT - remy);

  _obstacle[gy * GROUND_BITBOARD_WIDTH + gx] |= current;
  _obstacle[gy * GROUND_BITBOARD_WIDTH + gx + 1] |= right;
  _obstacle[(gy + 1) * GROUND_BITBOARD_WIDTH + gx] |= bottom;
  _obstacle[(gy + 1) * GROUND_BITBOARD_WIDTH + gx + 1] |= right_bottom;

  arrangeContact(_contact, stone_to_arrange.getBitBoard(), bx - 1, by);
  arrangeContact(_contact, stone_to_arrange.getBitBoard(), bx, by - 1);
  arrangeContact(_contact, stone_to_arrange.getBitBoard(), bx + 1, by);
  arrangeContact(_contact, stone_to_arrange.getBitBoard(), bx, by + 1);

  arrangeContact(_right_contact, stone_to_arrange.getBitBoard(), bx - 1, by);
  arrangeContact(_bottom_contact, stone_to_arrange.getBitBoard(), bx, by - 1);
  arrangeContact(_left_contact, stone_to_arrange.getBitBoard(), bx + 1, by);
  arrangeContact(_top_contact, stone_to_arrange.getBitBoard(), bx, by + 1);

  updateContactAndClosedArray();
}


int Ground::calculateNumContactingBlock() {
  int num_contacting_block = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_contacting_block += countBits(_contact[gi]);
  }

  return num_contacting_block;
}


int Ground::calculateNumContactingBlockInNeumannNeighborhood() {
  int num_contacting_block = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_contacting_block += countBits(_contact[gi] & _left_contact[gi]);
    num_contacting_block += countBits(_contact[gi] & _right_contact[gi]);
    num_contacting_block += countBits(_contact[gi] & _top_contact[gi]);
    num_contacting_block += countBits(_contact[gi] & _bottom_contact[gi]);
  }

  return num_contacting_block;
}


int Ground::calculateNumContactWithObstacle() {
  int num_contacting_block = 0;
  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_contacting_block += countBits(_left_contact[gi]);
    num_contacting_block += countBits(_right_contact[gi]);
    num_contacting_block += countBits(_top_contact[gi]);
    num_contacting_block += countBits(_bottom_contact[gi]);
  }

  return num_contacting_block;
}


int Ground::calculateNumOpenBlock() {
  int num_open_block = BOARD_WIDTH * BOARD_HEIGHT * GROUND_BITBOARD_SIZE;

  for (int gi = 0; gi < GROUND_BITBOARD_SIZE; gi++) {
    num_open_block -= countBits(_obstacle[gi]);
  }

  return num_open_block;
}


bool Ground::isObstacle(int bx, int by) {
  // speed-up: int gx = bx / BOARD_WIDTH, gy = by / BOARD_HEIGHT;
  int gx = bx >> 3, gy = by >> 3;

  // speed-up: int remx = bx % BOARD_WIDTH, remy = by % BOARD_HEIGHT;
  int remx = bx & 7ull, remy = by & 7ull;

  return _obstacle[gy * GROUND_BITBOARD_WIDTH + gx] & setOneBit(remx, remy);
}


bool Ground::isContact(int bx, int by) {
  // speed-up: int gx = bx / BOARD_WIDTH, gy = by / BOARD_HEIGHT;
  int gx = bx >> 3, gy = by >> 3;

  // speed-up: int remx = bx % BOARD_WIDTH, remy = by % BOARD_HEIGHT;
  int remx = bx & 7ull, remy = by & 7ull;

  return _contact[gy * GROUND_BITBOARD_WIDTH + gx] & setOneBit(remx, remy);
}


int Ground::findTopmostContact(void) {
  int top = 1024;

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    if (_contact[INNER_IDX[ii]]) {
      int iy = INNER_IDX_TOP[ii];
      int y = findTopmost(_contact[INNER_IDX[ii]]) + iy;

      if (y < top) {
        top = y;
      }
    }
  }

  return top;
}


int Ground::findTopmostBlank() {
  int top = 1024;

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    if (~_obstacle[INNER_IDX[ii]]) {
      int iy = INNER_IDX_TOP[ii];
      int y = findTopmost(~_obstacle[INNER_IDX[ii]]) + iy;

      if (y < top) {
        top = y;
      }
    }
  }

  return top;
}


int Ground::findBottommostContact(void) {
  int bottom = 0;

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    if (_contact[INNER_IDX[ii]]) {
      int iy = INNER_IDX_TOP[ii];
      int y = findBottommost(_contact[INNER_IDX[ii]]) + iy;

      if (y > bottom) {
        bottom = y;
      }
    }
  }

  return bottom;
}


int Ground::findBottommostBlank() {
  int bottom = 0;

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    if (~_obstacle[INNER_IDX[ii]]) {
      int iy = INNER_IDX_TOP[ii];
      int y = findBottommost(~_obstacle[INNER_IDX[ii]]) + iy;

      if (y > bottom) {
        bottom = y;
      }
    }
  }

  return bottom;
}


int Ground::findLeftmostContact(void) {
  int left = 1024;

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    if (_contact[INNER_IDX[ii]]) {
      int ix = INNER_IDX_LEFT[ii];
      int x = findLeftmost(_contact[INNER_IDX[ii]]) + ix;

      if (x < left) {
        left = x;
      }
    }
  }

  return left;
}


int Ground::findLeftmostBlank() {
  int left = 1024;

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    if (~_obstacle[INNER_IDX[ii]]) {
      int ix = INNER_IDX_LEFT[ii];
      int x = findLeftmost(~_obstacle[INNER_IDX[ii]]) + ix;

      if (x < left) {
        left = x;
      }
    }
  }

  return left;
}


int Ground::findRightmostContact(void) {
  int right = 0;

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    if (_contact[INNER_IDX[ii]]) {
      int ix = INNER_IDX_LEFT[ii];
      int x = findRightmost(_contact[INNER_IDX[ii]]) + ix;

      if (x > right) {
        right = x;
      }
    }
  }

  return right;
}


int Ground::findRightmostBlank() {
  int right = 0;

  for (int ii = 0; ii < INNER_IDX_SIZE; ii++) {
    if (~_obstacle[INNER_IDX[ii]]) {
      int ix = INNER_IDX_LEFT[ii];
      int x = findRightmost(~_obstacle[INNER_IDX[ii]]) + ix;

      if (x > right) {
        right = x;
      }
    }
  }

  return right;
}
