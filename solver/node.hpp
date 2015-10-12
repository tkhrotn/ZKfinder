#ifndef __NODE_HPP_
#define __NODE_HPP_

#include "defines.hpp"
#include "ground.hpp"
#include "stone.hpp"
#include "placement.hpp"

#include <vector>
#include <string>

namespace procon26 {
  /**
    Basic node class for any tree-search algorithms.
    */
  class Node {
  protected:
    Ground _ground; //! Current ground status
    int _pattern;   //! Rotation and inversion pattern of Stone to be arranged
    int _bx;        //! x-axis of Stone arrangement
    int _by;        //! y-axis of Stone arrangement
    int _order;     //! Order of stone arrangement
    bool _first_placement; //! Flag if it is the first placement or not
    int _score;     //! Score (num. of blanks)
    int _num_used_stone;  //! Num. of used stones 
    double _evaluation;  //! Evaluation of the node

    Placement _sequence[MAX_STONE];

    std::string _name;

  public:
    Node();
    Node(Ground &ground, int pattern, int x, int y, int order, bool first_placement, Placement prev_seuqence[]);
    virtual ~Node();

    virtual Node* create(Ground &ground, int pattern, int x, int y, int order, bool first_placement, Placement prev_seuqence[]);
    virtual Node* createCopy(Node &node);

    /**
      Expand child nodes.
      */
    virtual void expandChildren(std::vector<Node*> &children);

    int getOrder() {
      return _order;
    }

    int getScore() {
      return _score;
    }

    int getNumUsedStone() {
      return _num_used_stone;
    }

    Ground& getGround() {
      return _ground;
    }

    int getPattern() {
      return _pattern;
    }

    int getX() {
      return _bx;
    }

    int getY() {
      return _by;
    }

    bool isFirstPlacement() {
      return _first_placement;
    }

    bool isTerminal() {
      return _order >= g_num_stone - 1;
    }

    Placement* getSequence() {
      return _sequence;
    }

    double getEvaluation() {
      return _evaluation;
    }

    void setEvaluation(double evaluation) {
      _evaluation = evaluation;
    }

    void setName(std::string name) {
      _name = name;
    }

    std::string getName() {
      return _name;
    }

    bool operator<(const Node& rhs) const {
      return _evaluation < rhs._evaluation;
    }

    static bool compare(const Node *lhs, const Node *rhs) {
      return *lhs < *rhs;
    }
  };
}

#endif // __NODE_HPP_
