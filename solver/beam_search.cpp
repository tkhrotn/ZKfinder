#include "beam_search.hpp"
#include "node.hpp"
#include "lower_bound.hpp"
#include "globals.hpp"

#include <iostream>
#include <cstdlib>

using namespace procon26;
using namespace std;

BeamSearch::BeamSearch(int beam_width) : _beam_width(beam_width) {

}

BeamSearch::~BeamSearch() {

}


void BeamSearch::addQueueOrDelete(Node *node) {
  Node *back = _queue.back();

  if (_queue.size() == 0) {
    _queue.push_back(node);
  } else if (_queue.size() == _beam_width && node->getEvaluation() < back->getEvaluation()) {
    delete node;
  } else {
    if (_queue.size() == _beam_width) {
      delete back;
      _queue.pop_back();
    }

    for (auto it = _queue.begin(); it != _queue.end(); it++) {
      if (node->getEvaluation() > (*it)->getEvaluation()) {
        _queue.insert(it, node);
        return;
      }
    }

    // when specified node has worst evaluation in the queue
    _queue.push_back(node);
  }
}


void BeamSearch::run(Node &root) {
  vector <Node*> c;
  root.expandChildren(c);

  double local_best_evaluation = numeric_limits<double>::min();

  for (vector<Node*>::iterator cit = c.begin(); cit != c.end(); cit++) {
    addQueueOrDelete((*cit));
  }

  while (_queue.size() > 0) {
    Node *front = _queue.front();
    _queue.pop_front();

    //checkBest(pair<int, int>(front->getScore(), front->getNumUsedStone()), front->getSequence(), front->getGround());

    if (front->getEvaluation() > local_best_evaluation) {
      local_best_evaluation = front->getEvaluation();
    }

    // share best node with other search threads
    g_mutex.lock();
    Node best_so_far = getBestSoFarNode();
    if (front->getEvaluation() > best_so_far.getEvaluation()) {
      setBestSoFarNode(*front);
    } else {
      if (best_so_far.getEvaluation() > local_best_evaluation) {
        addQueueOrDelete(front->createCopy(best_so_far));
      }
    }
    g_mutex.unlock();

    vector<Node*> children;
    front->expandChildren(children);

    if (children.size() == 0) {
      delete front;
    } else {
      for (auto cit = children.begin(); cit != children.end(); cit++) {
        addQueueOrDelete((*cit));
      }

      delete front;
    }
  }

}
