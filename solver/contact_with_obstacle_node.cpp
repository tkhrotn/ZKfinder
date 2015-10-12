#include "contact_with_obstacle_node.hpp"
#include "max_contact_with_obstacle_playout.hpp"
#include "placement.hpp"
#include "globals.hpp"
#include "lower_bound.hpp"
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace procon26;

ContactWithObstacleNode::ContactWithObstacleNode(mt19937 &mt)
 : Node(), _mt(mt) {
 }

ContactWithObstacleNode::ContactWithObstacleNode(Ground &ground,
                                                 int pattern,
                                                 int x,
                                                 int y,
                                                 int order,
                                                 bool first_placement,
                                                 Placement prev_sequence[],
                                                 mt19937 &mt,
                                                 int pruning_threshold,
                                                 double contact_weight,
                                                 double four_contact_weight,
                                                 double two_closed_weight,
                                                 double ltype_closed_weight,
                                                 double itype_closed_weight,
                                                 double three_contact_weight)
 : Node(ground, pattern, x, y, order, first_placement, prev_sequence), _mt(mt),
  _pruning_threshold(pruning_threshold),
  _contact_weight(contact_weight),
  _four_contact_weight(four_contact_weight),
  _two_closed_weight(two_closed_weight),
  _ltype_closed_weight(_ltype_closed_weight),
  _itype_closed_weight(itype_closed_weight),
  _three_contact_weight(three_contact_weight) {
}

ContactWithObstacleNode::~ContactWithObstacleNode() {
}

double ContactWithObstacleNode::evaluation(Node &node) {
  double worst = g_num_blanks * 10000 + MAX_STONE;
  double eval = worst;

  Placement sequence[MAX_STONE];
  MaxContactWithObstaclePlayout mcwop(_mt, _contact_weight, _four_contact_weight, _two_closed_weight, _ltype_closed_weight, _itype_closed_weight, _three_contact_weight);
  mcwop.setName(_name);
  for (int i = 0; i < 1; i++) {
    pair<int, int> snpair = mcwop.run(node, sequence);

    eval = min((double)snpair.first * 10000 + (double)snpair.second, eval);
  }

  return worst - eval;
}

Node* ContactWithObstacleNode::create(Ground &ground, int pattern, int x, int y, int order, bool first_placement, Placement prev_seuqence[]) {
  Node *node = new ContactWithObstacleNode(ground,
                                           pattern,
                                           x,
                                           y,
                                           order,
                                           first_placement,
                                           prev_seuqence,
                                           _mt,
                                           _pruning_threshold,
                                           _contact_weight,
                                           _four_contact_weight,
                                           _two_closed_weight,
                                           _ltype_closed_weight,
                                           _itype_closed_weight,
                                           _three_contact_weight);
  node->setEvaluation(evaluation(*node));
  node->setName(_name);

  return node;
}


Node* ContactWithObstacleNode::createCopy(Node &node) {
  ContactWithObstacleNode *copy = new ContactWithObstacleNode(_mt);

  copy->_ground = node.getGround();
  copy->_pattern = node.getPattern();
  copy->_bx = node.getX();
  copy->_by = node.getY();
  copy->_order = node.getOrder();
  copy->_first_placement = node.isFirstPlacement();
  copy->_score = node.getScore();
  copy->_num_used_stone = node.getNumUsedStone();
  copy->_evaluation = node.getEvaluation();
  Placement::copySequence(node.getSequence(), copy->_sequence);
  copy->_pruning_threshold = _pruning_threshold;
  copy->_contact_weight = _contact_weight;
  copy->_four_contact_weight = _four_contact_weight;
  copy->_two_closed_weight = _two_closed_weight;
  copy->_ltype_closed_weight = _ltype_closed_weight;
  copy->_itype_closed_weight = _itype_closed_weight;
  copy->_three_contact_weight = _three_contact_weight;
  copy->setName(_name);

  return copy;
}


bool ContactWithObstacleNode::greaterPair(const pipair& l, const pipair& r) {
  return l.second > r.second;
}

void ContactWithObstacleNode::expandChildren(vector<Node*> &children) {
  // This node is the final stone
  if (_order == g_num_stone - 1) {
    return;
  }

  vector<pipair> candidates;
  
  if (_first_placement) {
    Node *n = create(_ground, PASS_PATTERN, 0, 0, _order + 1, true, _sequence);
    children.push_back(n);

    if (g_use_flag[_order + 1]) {
      for (int x = 1; x < Ground::WHOLE_BLOCK_WIDTH - BOARD_WIDTH - 1; x++) {
        for (int y = 1; y < Ground::WHOLE_BLOCK_HEIGHT - BOARD_HEIGHT - 1; y++) {
          for (int pi = 0; pi < NUM_PLACEMENT_PATTERN - 1; pi++) {
            if (!g_duplicate_stone[_order + 1][pi]
             && !_ground.collideWith(g_stones[_order + 1][pi], x, y)) {
              // The arrangement is valid
              double num_contact = _ground.calculateNumBlockInNeumannNeighborhoodWithWeight(g_stones[_order + 1][pi], x, y);

              Ground g(_ground);
              g.arrange(g_stones[_order + 1][pi], x, y);
              int open = g.calculateNumOpenBlock();
              int four = g.calculateNumFourContact();
              int two = g.calculateNumTwoClosed();
              int ltype = g.calculateNumLtypeClosed();
              int itype = g.calculateNumItypeClosed();
              int three = g.calculateNumThreeContact();

              double eval = ::pow(num_contact + 1, _contact_weight)
                          * ::pow(four + 1, _four_contact_weight)
                          * ::pow(two + 1, _two_closed_weight)
                          * ::pow(ltype + 1, _ltype_closed_weight)
                          * ::pow(itype + 1, _itype_closed_weight)
                          * ::pow(three + 1, _three_contact_weight);

              if (max(LowerBound::calculateBySubsetSumProblem(open, _order + 1),
                      LowerBound::calculateByRemainOneBlock(g, _order + 1)) > min(g_best_so_far_score, g_upper_bound_score))
                eval = -1;

              candidates.push_back(pipair(Placement(pi, x, y), eval));
            }
          }
        }
      }
    }
  } else {
    Node *n = create(_ground, PASS_PATTERN, 0, 0, _order + 1, false, _sequence);
    children.push_back(n);
    if (g_use_flag[_order + 1]) {
      for (int x = 1; x < Ground::WHOLE_BLOCK_WIDTH - BOARD_WIDTH - 1; x++) {
        for (int y = 1; y < Ground::WHOLE_BLOCK_HEIGHT - BOARD_HEIGHT - 1; y++) {
          for (int pi = 0; pi < NUM_PLACEMENT_PATTERN - 1; pi++) {
            if (!g_duplicate_stone[_order + 1][pi]
             && _ground.checkNotCollideAndContact(g_stones[_order + 1][pi], x, y)) {
              // The arrangement is valid
              double num_contact = _ground.calculateNumBlockInNeumannNeighborhoodWithWeight(g_stones[_order + 1][pi], x, y);

              Ground g(_ground);
              g.arrange(g_stones[_order + 1][pi], x, y);
              int open = g.calculateNumOpenBlock();
              int four = g.calculateNumFourContact();
              int two = g.calculateNumTwoClosed();
              int ltype = g.calculateNumLtypeClosed();
              int itype = g.calculateNumItypeClosed();
              int three = g.calculateNumThreeContact();

              double eval = ::pow(num_contact + 1, _contact_weight)
                          * ::pow(four + 1, _four_contact_weight)
                          * ::pow(two + 1, _two_closed_weight)
                          * ::pow(ltype + 1, _ltype_closed_weight)
                          * ::pow(itype + 1, _itype_closed_weight)
                          * ::pow(three + 1, _three_contact_weight);

              if (max(LowerBound::calculateBySubsetSumProblem(open, _order + 1),
                      LowerBound::calculateByRemainOneBlock(g, _order + 1)) > min(g_best_so_far_score, g_upper_bound_score))
                eval = -1;

              candidates.push_back(pipair(Placement(pi, x, y), eval));
            }
          }
        }
      }
    }
  }

  sort(candidates.begin(), candidates.end(), ContactWithObstacleNode::greaterPair);

  for (int i = 0; i < min((int)candidates.size(), _pruning_threshold); i++) {
    Node *n = create(_ground, candidates[i].first._pattern, candidates[i].first._bx, candidates[i].first._by, _order + 1, false, _sequence);
    children.push_back(n);
  }
}
