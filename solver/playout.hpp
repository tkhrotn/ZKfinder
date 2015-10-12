#ifndef __PLAYOUT_HPP_
#define __PLAYOUT_HPP_

#include "node.hpp"
#include "placement.hpp"

#include <utility>
#include <string>

namespace procon26 {
  /**
    Abstract class for implementing playout classes.
    */
  class Playout {
  protected:
    std::string _name;

    void copyPredecessor(Node &root, Placement result_sequence[]) {
      for (int i = 0; i <= root.getOrder(); i++) {
        result_sequence[i] = root.getSequence()[i];
      }
    }

  public:
    Playout() {
    };

    virtual ~Playout() {
    };

    void setName(std::string name) {
      _name = name;
    }

    std::string getName() {
      return _name;
    }

    /**
      Abstract method for implementing playout procedures.
      @param root Root node which is a start point of a playout.
      @param result_sequence Play sequence of the result of the playout.
      @return Pair of the score and the number of stones obtained by the playout.
      */
    virtual std::pair<int, int> run(Node &root, Placement result_sequence[]) = 0;
  };
}

#endif // __PLAYOUT_HPP_
