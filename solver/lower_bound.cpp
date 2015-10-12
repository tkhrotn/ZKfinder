#include "lower_bound.hpp"

#include "globals.hpp"

#include <iostream>
#include <limits>
#include <bitset>

using namespace std;
using namespace procon26;

int LowerBound::calculateBySubsetSumProblem(int num_open_block, int remain_order) {
  bitset<MAX_BLANK + 1> bs(1);

  // solve subset sum problem by dynamic programming
  for (int i = remain_order; i < g_num_stone; i++) {
    if (g_use_flag[i]) {
      bitset<MAX_BLANK + 1> bs_prev = bs;
      bs <<= g_stones[i][0].getSize();
      bs |= bs_prev;
    }
  }

  int lb = num_open_block;
  for (int i = num_open_block; i > 0; i--) {
    if (bs.test(i)) {
      lb = num_open_block - i;
      break;
    }
  }

  return lb;
}

int LowerBound::calculateByRemainOneBlock(Ground &ground, int remain_order) {
  int num_one_closed = ground.calculateNumFourContact();
  int num_two_closed = ground.calculateNumTwoClosed();
  int num_ltype_closed = ground.calculateNumLtypeClosed();
  int num_itype_closed = ground.calculateNumItypeClosed();

  // place to just fit closed
  int remain_ltype_closed = max(0, num_ltype_closed - g_remain_ltype_stone[remain_order]);
  int remain_itype_closed = max(0, num_itype_closed - g_remain_itype_stone[remain_order]);
  int remain_two_closed = max(0, num_two_closed - g_remain_two_stone[remain_order]);
  int remain_one_closed = max(0, num_one_closed - g_remain_one_stone[remain_order]);

  int remain_ltype_stone = max(0, g_remain_ltype_stone[remain_order] - remain_ltype_closed);
  int remain_itype_stone = max(0, g_remain_itype_stone[remain_order] - remain_itype_closed);
  int remain_two_stone = max(0, g_remain_two_stone[remain_order] - remain_two_closed);
  int remain_one_stone = max(0, g_remain_one_stone[remain_order] - remain_one_closed);

  // place two stone to ltype or itype closed
  int remain_itype_and_ltype = max(0, remain_ltype_closed + remain_itype_closed - remain_two_stone);
  remain_one_closed += remain_ltype_closed + remain_itype_closed - remain_itype_and_ltype;

  return max(0, remain_one_closed + 3 * remain_itype_and_ltype - remain_one_stone);
}
