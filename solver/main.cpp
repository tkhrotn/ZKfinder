#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <thread>
#include <limits>
#include <chrono>

#include "defines.hpp"
#include "utils.hpp"
#include "ground.hpp"
#include "stone.hpp"
#include "node.hpp"
#include "lower_bound.hpp"
#include "max_contact_with_obstacle_playout.hpp"
#include "beam_search.hpp"
#include "contact_with_obstacle_node.hpp"
#include "globals.hpp"

using namespace std;
using namespace procon26;

void worker1() {
    mt19937 mt;
    mt.seed(g_seed1);

  while (true) {
    Placement prev[MAX_STONE];
    ContactWithObstacleNode node(g_initial_ground, UNPLACED_PATTERN, 1, 1, -1, true, prev, mt, 5, 1, -2, -2, -2, -2, 0);
    node.setName("FewerIsolated");
    BeamSearch bs(50000);
    bs.run(node);    
  }
}

void worker2() {
  mt19937 mt;
  mt.seed(g_seed2);

  while (true) {
    Placement prev[MAX_STONE];
    ContactWithObstacleNode node(g_initial_ground, UNPLACED_PATTERN, 1, 1, -1, true, prev, mt, 5, 1, -0.1, -0.1, -0.1, -0.1, 0);
    node.setName("CloseContact");
    BeamSearch bs(50000);
    bs.run(node);    
  }
}


void worker3() {
  mt19937 mt;
  mt.seed(g_seed3);

  while (true) {
    Placement prev[MAX_STONE];
    ContactWithObstacleNode node(g_initial_ground, UNPLACED_PATTERN, 1, 1, -1, true, prev, mt, numeric_limits<int>::max(), 1, -0.1, -0.1, -0.1, -0.1, 0);
    node.setName("ExtremeBreadth");
    BeamSearch bs(50000);
    bs.run(node);    
  }
}


int main(int argc, char* argv[])
{
  initializeApplication(argc, argv);

  thread th1(worker1);
  thread th2(worker2);
  thread th3(worker3);

  while (true) {
    this_thread::sleep_for(chrono::seconds(1));

    auto now = chrono::system_clock::now();
    auto elapse = now - g_found_best_time;
    double seconds = (double)chrono::duration_cast<chrono::milliseconds>(elapse).count() / 1000.0;
    if (seconds > g_termination_time) {
      finalizeApplication();
    }
  }
  
  //// NEVER REACHED ////
  th1.join();
  th2.join();
  th3.join();

  return EXIT_SUCCESS;
}
