#ifndef __SEARCH_HPP_
#define __SEARCH_HPP_

#include "node.hpp"

namespace procon26 {
  /**
    Abstract class for search methods.
    */
  class Search {
  public:
    Search() {};
    virtual ~Search() {};

    virtual void run(Node &root) = 0;
  };
}

#endif // __SEARCH_HPP_
