#ifndef __CONTACT_WITH_OBSTACLE_NODE_HPP_
#define __CONTACT_WITH_OBSTACLE_NODE_HPP_

#include "node.hpp"
#include <random>

namespace procon26 {
  class ContactWithObstacleNode : public Node {
    typedef Node super;
    typedef std::pair<Placement, double> pipair;

  protected:
    std::mt19937 &_mt;

    int _pruning_threshold;
    double _contact_weight;
    double _four_contact_weight;
    double _two_closed_weight;
    double _ltype_closed_weight;
    double _itype_closed_weight;
    double _three_contact_weight;

    static bool greaterPair(const pipair& l, const pipair& r);
    
  public:
    ContactWithObstacleNode(std::mt19937 &mt);
    ContactWithObstacleNode(Ground &ground,
                            int pattern,
                            int x,
                            int y,
                            int order,
                            bool first_placement,
                            Placement prev_seuqence[],
                            std::mt19937 &mt,
                            int pruning_threshold,
                            double contact_weight,
                            double four_contact_weight,
                            double two_closed_weight,
                            double ltype_closed_weight,
                            double itype_closed_weight,
                            double three_contact_weight);
    virtual ~ContactWithObstacleNode();

    double evaluation(Node &node);

    virtual Node* create(Ground &ground, int pattern, int x, int y, int order, bool first_placement, Placement prev_seuqence[]);
    virtual Node* createCopy(Node &node);

    virtual void expandChildren(std::vector<Node*> &children);
  };
}

#endif // __CONTACT_WITH_OBSTACLE_NODE_HPP_
