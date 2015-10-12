#ifndef __MAX_CONTACT_WITH_OBSTACLE_PLAYOUT_HPP_
#define __MAX_CONTACT_WITH_OBSTACLE_PLAYOUT_HPP_

#include "playout.hpp"

#include <random>
#include <utility>

namespace procon26 {
  class MaxContactWithObstaclePlayout : public Playout {
  private:
    std::mt19937 &_mt;

    double _contact_weight;
    double _four_contact_weight;
    double _two_closed_weight;
    double _ltype_closed_weight;
    double _itype_closed_weight;
    double _three_contact_weight;

  public:
    MaxContactWithObstaclePlayout(std::mt19937 &mt,
                                  double contact_weight,
                                  double four_contact_weight,
                                  double two_closed_weight,
                                  double ltype_closed_weight,
                                  double itype_closed_weight,
                                  double three_contact_weight);
    virtual ~MaxContactWithObstaclePlayout();

    virtual std::pair<int, int> run(Node &root, Placement result_sequence[]);

  };
}

#endif // __MAX_CONTACT_WITH_OBSTACLE_PLAYOUT_HPP_
