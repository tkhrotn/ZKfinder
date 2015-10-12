#ifndef __GLOBALS_HPP_
#define __GLOBALS_HPP_

#include "defines.hpp"
#include "stone.hpp"
#include "placement.hpp"
#include "ground.hpp"
#include "node.hpp"

#include <random>
#include <utility>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>

namespace procon26 {
  extern Ground g_initial_ground;
  extern int g_num_blanks;
  extern int g_initial_leftmost, g_initial_rightmost, g_initial_topmost, g_initial_bottommost;

  extern int g_num_stone;
  extern Stone g_stones[MAX_STONE][NUM_PLACEMENT_PATTERN];
  extern bool g_duplicate_stone[MAX_STONE][NUM_PLACEMENT_PATTERN];
  extern int g_remain_one_stone[MAX_STONE];
  extern int g_remain_two_stone[MAX_STONE];
  extern int g_remain_ltype_stone[MAX_STONE];
  extern int g_remain_itype_stone[MAX_STONE];
  extern bool g_use_flag[MAX_STONE];

  extern int g_best_so_far_score;
  extern int g_best_so_far_num_used_stone;
  extern Placement g_best_so_far_sequence[MAX_STONE];
  extern Ground g_best_so_far_ground;

  extern std::mutex g_mutex;

  extern std::chrono::time_point<std::chrono::system_clock> g_start_time, g_found_best_time;

  extern bool g_selected_stone_mode;

  extern int g_upper_bound_score;

  extern int g_seed1, g_seed2, g_seed3;

  extern int g_termination_time;

  void initializeApplication(int argc, char* argv[]);
  void finalizeApplication();
  void checkBest(std::pair<int, int> snpair, Placement sequence[], Ground &ground, std::string name);
  void setBestSoFarNode(Node &best_so_far_node);
  Node getBestSoFarNode();
}

#endif // __GLOBALS_HPP_
