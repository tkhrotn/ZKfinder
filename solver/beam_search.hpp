#ifndef __BEAM_SEARCH_HPP_
#define __BEAM_SEARCH_HPP_

#include "search.hpp"

#include <list>

namespace procon26 {
  class BeamSearch : public Search {
  private:
    std::list<Node*> _queue;
    int _beam_width;

  public:
    BeamSearch(int beam_width);
    virtual ~BeamSearch();

    void addQueueOrDelete(Node *node);
    virtual void run(Node &root);
  };
}

#endif // __BEAM_SEARCH_HPP_
