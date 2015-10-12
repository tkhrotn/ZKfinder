#ifndef __LOWERBOUND_HPP_
#define __LOWERBOUND_HPP_

#include <vector>

#include "defines.hpp"
#include "stone.hpp"
#include "ground.hpp"

namespace procon26 {

  /**
    Class for calculating lower bound of score.
    */
  class LowerBound {
  public:
    static int calculateBySubsetSumProblem(int num_open_block, int remain_order);
    static int calculateByRemainOneBlock(Ground &ground, int remain_order);
  };
}

#endif // __LOWERBOUND_HPP_
