#include "node.hpp"

#include "globals.hpp"

#include <iostream>
#include <limits>

using namespace std;
using namespace procon26;

Node::Node()
 : _score(MAX_BLANK), _num_used_stone(MAX_STONE) {
}

Node::Node(Ground &ground, int pattern, int x, int y, int order, bool first_placement, Placement prev_sequence[])
 : _ground(ground), _pattern(pattern), _bx(x), _by(y), _order(order), _first_placement(first_placement), _num_used_stone(0) {
  _score = ground.calculateNumOpenBlock();

  for (int i = 0; i < order; i++) {
    _sequence[i] = prev_sequence[i];

    if (_sequence[i]._pattern != PASS_PATTERN)
      _num_used_stone++;
  }

  if (_pattern != UNPLACED_PATTERN && _pattern != PASS_PATTERN) {
    _ground.arrange(g_stones[_order][_pattern], _bx, _by);
    _score -= g_stones[_order][_pattern].getSize();
    _num_used_stone++;
  }

  _sequence[_order]._pattern = _pattern;
  _sequence[_order]._bx = _bx;
  _sequence[_order]._by = _by;
}

Node::~Node() {
}

Node* Node::create(Ground &ground, int pattern, int x, int y, int order, bool first_placement, Placement prev_seuqence[]) {
  Node *node =  new Node(ground, pattern, x, y, order, first_placement, prev_seuqence);
  node->setEvaluation(0);

  return node;
}

Node* Node::createCopy(Node &node) {
  Node *copy = new Node(node);

  return copy;
}

void Node::expandChildren(vector<Node*> &children) {
  // This node is the final stone
  if (_order == g_num_stone - 1) {
    return;
  }

  
  if (_first_placement) {
    Node *n = create(_ground, PASS_PATTERN, 0, 0, _order + 1, true, _sequence);
    children.push_back(n);
    for (int x = 1; x < Ground::WHOLE_BLOCK_WIDTH - BOARD_WIDTH - 1; x++) {
      for (int y = 1; y < Ground::WHOLE_BLOCK_HEIGHT - BOARD_HEIGHT - 1; y++) {
        for (int pi = 0; pi < NUM_PLACEMENT_PATTERN - 1; pi++) {
          if (!_ground.collideWith(g_stones[_order + 1][pi], x, y)
            && !g_duplicate_stone[_order + 1][pi]) {
            // The arrangement is valid
            Node *n = create(_ground, pi, x, y, _order + 1, false, _sequence);
            children.push_back(n);
          }
        }
      }
    }
  } else {
    Node *n = create(_ground, PASS_PATTERN, 0, 0, _order + 1, false, _sequence);
    children.push_back(n);
    for (int x = 1; x < Ground::WHOLE_BLOCK_WIDTH - BOARD_WIDTH - 1; x++) {
      for (int y = 1; y < Ground::WHOLE_BLOCK_HEIGHT - BOARD_HEIGHT - 1; y++) {
        for (int pi = 0; pi < NUM_PLACEMENT_PATTERN - 1; pi++) {
          if (!_ground.collideWith(g_stones[_order + 1][pi], x, y)
           && _ground.contactWithStones(g_stones[_order + 1][pi], x, y)
           && !g_duplicate_stone[_order + 1][pi]) {
            // The arrangement is valid
            Node *n = create(_ground, pi, x, y, _order + 1, false, _sequence);
            children.push_back(n);
          }
        }
      }
    }
  }
}
