#include "max_contact_with_obstacle_playout.hpp"
#include "node.hpp"
#include "placement.hpp"
#include "lower_bound.hpp"
#include "globals.hpp"

#include <vector>
#include <limits>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace procon26;

MaxContactWithObstaclePlayout::MaxContactWithObstaclePlayout(mt19937 &mt,
                                                             double contact_weight,
                                                             double four_contact_weight,
                                                             double two_closed_weight,
                                                             double ltype_closed_weight,
                                                             double itype_closed_weight,
                                                             double three_contact_weight)
 : Playout(),
   _mt(mt),
   _contact_weight(contact_weight),
   _four_contact_weight(four_contact_weight),
   _two_closed_weight(two_closed_weight),
   _ltype_closed_weight(ltype_closed_weight),
   _itype_closed_weight(itype_closed_weight),
   _three_contact_weight(three_contact_weight) {
}

MaxContactWithObstaclePlayout::~MaxContactWithObstaclePlayout() {

}

pair<int, int> MaxContactWithObstaclePlayout::run(Node &root, Placement result_sequence[]) {
  copyPredecessor(root, result_sequence);
  Node *result = &root;
  Ground ground = root.getGround();
  bool first_arrangement = root.isFirstPlacement();
  int num_used_stone = root.getNumUsedStone();

  for (int order = root.getOrder() + 1; order < MAX_STONE; order++) {
    vector<Placement> candidates;
    if (first_arrangement) {
      candidates.push_back(Placement(PASS_PATTERN, 0, 0));
      if (g_use_flag[order]) {
        for (int pi = 0; pi < NUM_PLACEMENT_PATTERN - 1; pi++) {
          for (int x = g_initial_leftmost - g_stones[order][pi].getRightmost(); x <= g_initial_rightmost - g_stones[order][pi].getLeftmost(); x++) {
            for (int y = g_initial_topmost - g_stones[order][pi].getBottommost(); y <= g_initial_bottommost - g_stones[order][pi].getTopmost(); y++) {
              if (!g_duplicate_stone[order][pi]
               && !ground.collideWith(g_stones[order][pi], x, y)) {
                // The placement is valid
                candidates.push_back(Placement(pi, x, y));
              }
            }
          }
        }
      }
    } else {
      candidates.push_back(Placement(PASS_PATTERN, 0, 0));

      if (g_use_flag[order]) {
        int leftmost = ground.findLeftmostContact();
        int rightmost = ground.findRightmostContact();
        int topmost = ground.findTopmostContact();
        int bottommost = ground.findBottommostContact();

        for (int pi = 0; pi < NUM_PLACEMENT_PATTERN - 1; pi++) {
          for (int x = leftmost - g_stones[order][pi].getRightmost(); x <= rightmost - g_stones[order][pi].getLeftmost(); x++) {
            for (int y = topmost - g_stones[order][pi].getBottommost(); y <= bottommost - g_stones[order][pi].getTopmost(); y++) {
              if (!g_duplicate_stone[order][pi]
               && ground.checkNotCollideAndContact(g_stones[order][pi], x, y)) {
                // The arrangement is valid
                candidates.push_back(Placement(pi, x, y));
              }
            }
          }
        }
      }
    }

    double max_eval = -1;
    Placement selected;
    if (!g_use_flag[order] || candidates.size() == 0 || LowerBound::calculateBySubsetSumProblem(ground.calculateNumOpenBlock() - g_stones[order][0].getSize(), order + 1) > min(g_best_so_far_score, g_upper_bound_score)
      || (min(g_upper_bound_score, g_best_so_far_score) == 0 && num_used_stone == g_best_so_far_num_used_stone)) {
      selected = candidates[0];
    } else if (!first_arrangement) {
      vector<Placement> max_candidates;
      for (auto it = candidates.begin(); it != candidates.end(); it++) {
        Placement p = (*it);

        int num_contact = ground.calculateNumBlockInNeumannNeighborhoodWithWeight(g_stones[order][p._pattern], p._bx, p._by);

        Ground g(ground);
        g.arrange(g_stones[order][p._pattern], p._bx, p._by);
        int four = g.calculateNumFourContact();
        int two = g.calculateNumTwoClosed();
        int ltype = g.calculateNumLtypeClosed();
        int itype = g.calculateNumItypeClosed();
        int three = g.calculateNumThreeContact();

        double eval = ::pow((double)num_contact + 1.0, _contact_weight)
                    * ::pow((double)four + 1.0, _four_contact_weight)
                    * ::pow((double)two + 1.0, _two_closed_weight)
                    * ::pow((double)ltype + 1.0, _ltype_closed_weight)
                    * ::pow((double)itype + 1.0, _itype_closed_weight)
                    * ::pow((double)three + 1.0, _three_contact_weight);

        if (LowerBound::calculateByRemainOneBlock(g, order + 1) > min(g_best_so_far_score, g_upper_bound_score))
          eval = -1;

        if (eval > max_eval) {
          max_candidates.clear();
          max_candidates.push_back(p);
          max_eval = eval;
        } else if (eval == max_eval) {
          max_candidates.push_back(p);
        }
      }

      // TODO: to fix bug in which eval can be nan.
      if (max_candidates.size() == 0) {
        selected = candidates[0];
      } else {
        uniform_int_distribution<int> dist(0, max_candidates.size() - 1);

        selected = max_candidates[dist(_mt)];        
      }
    } else {
      uniform_int_distribution<int> dist(0, candidates.size() - 1);

      selected = candidates[dist(_mt)];
    }

    ground.arrange(g_stones[order][selected._pattern], selected._bx, selected._by);
    result_sequence[order] = selected;

    if (selected._pattern != PASS_PATTERN) {
      first_arrangement = false;
      num_used_stone++;
    }
  }

  pair<int, int> snpair(ground.calculateNumOpenBlock(), num_used_stone);

  checkBest(snpair, result_sequence, ground, _name);

  return snpair;
}
