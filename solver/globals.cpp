#include "globals.hpp"

#include "node.hpp"
#include "lower_bound.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <unistd.h>
#include <utility>
#include <algorithm>
#include <limits>
#include <random>

using namespace std;
using namespace procon26;

namespace procon26 {
  Ground g_initial_ground;
  int g_num_blanks;
  int g_initial_leftmost, g_initial_rightmost, g_initial_topmost, g_initial_bottommost;

  int g_num_stone;
  Stone g_stones[MAX_STONE][NUM_PLACEMENT_PATTERN];
  bool g_duplicate_stone[MAX_STONE][NUM_PLACEMENT_PATTERN] = {false};
  int g_remain_one_stone[MAX_STONE] = {0};
  int g_remain_two_stone[MAX_STONE] = {0};
  int g_remain_ltype_stone[MAX_STONE] = {0};
  int g_remain_itype_stone[MAX_STONE] = {0};
  bool g_use_flag[MAX_STONE];
  int g_permissible_used_stone = 0;

  int g_best_so_far_score = MAX_BLANK;
  int g_best_so_far_num_used_stone = MAX_STONE;
  Placement g_best_so_far_sequence[MAX_STONE];
  Ground g_best_so_far_ground;
  bool g_zero_blank_found = false;

  Node g_best_so_far_node;

  mutex g_mutex;

  chrono::time_point<chrono::system_clock> g_start_time, g_found_best_time;

  bool g_selected_stone_mode = false;

  int g_upper_bound_score = numeric_limits<int>::max();

  int g_seed1, g_seed2, g_seed3;

  int g_termination_time = 30;

  typedef std::pair<int, int> ipair;
  bool lessPair(const ipair& l, const ipair& r) {
    return l.second > r.second;
  }

  void initializeApplication(int argc, char* argv[]) {
    g_mutex.lock();

    g_start_time = chrono::system_clock::now();
    
    string ground_string, line;
    for (int gi = 0; gi < Ground::INNER_BLOCK_HEIGHT; gi++) {
      cin >> line;
      ground_string += line + '\n';
    }

    g_initial_ground.readProblemString(ground_string);
    g_initial_ground.print();
    g_num_blanks = g_initial_ground.calculateNumOpenBlock();
    cout << "Num. blank :" << g_num_blanks << endl;

    g_initial_leftmost = g_initial_ground.findLeftmostBlank();
    g_initial_rightmost = g_initial_ground.findRightmostBlank();
    g_initial_topmost = g_initial_ground.findTopmostBlank();
    g_initial_bottommost = g_initial_ground.findBottommostBlank();

    cout << "L:" << g_initial_leftmost << " "
         << "R:" << g_initial_rightmost << " "
         << "T:" << g_initial_topmost << " "
         << "B:" << g_initial_bottommost << endl;

    cin >> g_num_stone;
    cout << "Num. stones: " << g_num_stone << endl;

    for (int si = 0; si < g_num_stone; si++) {
      string stone_string;
      for (int bi = 0; bi < BOARD_HEIGHT; bi++) {
        cin >> line;
        stone_string += line + '\n';
      }
      g_stones[si][0].readProblemString(stone_string);

      for (int pi = 1; pi < NUM_PLACEMENT_PATTERN; pi++) {
        g_stones[si][pi] = g_stones[si][0];
      }
      g_stones[si][1].rotate90();
      g_stones[si][2].rotate180();
      g_stones[si][3].rotate270();
      g_stones[si][4].reverse();
      g_stones[si][5].reverse(); g_stones[si][5].rotate90();
      g_stones[si][6].reverse(); g_stones[si][6].rotate180();
      g_stones[si][7].reverse(); g_stones[si][7].rotate270();
      g_stones[si][8].setPass(true);

      for (int pi1 = 0; pi1 < NUM_PLACEMENT_PATTERN - 2; pi1++) {
        for (int pi2 = pi1 + 1; pi2 < NUM_PLACEMENT_PATTERN - 1; pi2++) {
          if (isDuplicate(g_stones[si][pi1].getBitBoard(), g_stones[si][pi2].getBitBoard())) {
            g_duplicate_stone[si][pi2] = true;
          }
        }
      }
    }


    // print stones
    /*for (int si = 0; si < g_num_stone; si++) {
      cout << endl << "Order " << si << endl;
      for (int pi = 0; pi < NUM_PLACEMENT_PATTERN; pi++) {
        if (g_stones[si][pi].isPassed()) {
          cout << "Placement passed.";
        } else {
          if (g_duplicate_stone[si][pi])
            cout << "**Duplicate**" << endl;

          printBitBoard(g_stones[si][pi].getBitBoard());
          cout << "L:" << g_stones[si][pi].getLeftmost() << " "
               << "R:" << g_stones[si][pi].getRightmost() << " "
               << "T:" << g_stones[si][pi].getTopmost() << " "
               << "B:" << g_stones[si][pi].getBottommost() << endl;
        }
        cout << endl;
      }
    }*/

    random_device rd;
    g_seed1 = rd();
    g_seed2 = rd();
    g_seed3 = rd();

    int result;
    while ((result = getopt(argc, argv, "a:b:c:sp:t:u:")) != -1 ) {
      switch (result) {
      case 'a':
        g_seed1 = atoi(optarg);
        break;
      case 'b':
        g_seed2 = atoi(optarg);
        break;
      case 'c':
        g_seed3 = atoi(optarg);
        break;
      case 's':
        g_selected_stone_mode = true;
        break;
      case 'p':
        g_permissible_used_stone = atoi(optarg);
        break;
      case 't':
        g_termination_time = atoi(optarg);
        break;
      case 'u':
        g_upper_bound_score = atoi(optarg);
        break;
      }
    }

    for (int i = 0; i < g_num_stone; i++) {
      g_use_flag[i] = true;
    }

    if (g_selected_stone_mode) {
      for (int i = 0; i < g_num_stone; i++) {
        g_use_flag[i] = false;
      }
      vector<ipair> stone_sizes;
      for (int i = 0; i < g_num_stone; i++) {
        stone_sizes.push_back(ipair(i, g_stones[i][0].getSize()));
      }
      sort(stone_sizes.begin(), stone_sizes.end(), lessPair);

      for (int i = 0; i < g_num_stone; i++) { 
        g_use_flag[stone_sizes[i].first] = true;
        if (LowerBound::calculateBySubsetSumProblem(g_num_blanks, 0) == 0 && i + 1 > g_permissible_used_stone)
          break;
      }
    }

    int num_one_stone = 0;
    int num_two_stone = 0;
    int num_ltype_stone = 0;
    int num_itype_stone = 0;
    for (int si = 0; si < g_num_stone; si++) {
      if (g_use_flag[si]) {
        if (g_stones[si][0].getSize() == 1)
          num_one_stone++;
        else if (g_stones[si][0].getSize() == 2)
          num_two_stone++;
        else if (isLtype(g_stones[si][0].getBitBoard()))
          num_ltype_stone++;
        else if (isItype(g_stones[si][0].getBitBoard()))
          num_itype_stone++;
      }
    }

    for (int si = 0; si < g_num_stone; si++) {
      g_remain_one_stone[si] = num_one_stone;
      g_remain_two_stone[si] = num_two_stone;
      g_remain_ltype_stone[si] = num_ltype_stone;
      g_remain_itype_stone[si] = num_itype_stone;

      if (g_use_flag[si]) {
        if (g_stones[si][0].getSize() == 1)
          num_one_stone--;
        else if (g_stones[si][0].getSize() == 2)
          num_two_stone--;
        else if (isLtype(g_stones[si][0].getBitBoard()))
          num_ltype_stone--;
        else if (isItype(g_stones[si][0].getBitBoard()))
          num_itype_stone--;
      }
    }

    ofstream ofs("problem_stats.txt");
    ofs << "NumBlanks: " << g_num_blanks << endl;
    ofs << "Edge(LRTB): " << g_initial_leftmost << " "
                          << g_initial_rightmost << " "
                          << g_initial_topmost << " "
                          << g_initial_bottommost << endl;
    ofs << "NumStones: " << g_num_stone << endl;
    ofs.close();

    g_mutex.unlock();
  }

  void finalizeApplication() {
    g_found_best_time = chrono::system_clock::now();
    auto elapse = g_found_best_time - g_start_time;
    double seconds = (double)chrono::duration_cast<chrono::milliseconds>(elapse).count() / 1000.0;

    cout << "Termination condition satisfied. Elapse: "
         << fixed << setprecision(2) << seconds << "(s)" << endl;
    exit(0);
  }

  void checkBest(pair<int, int> snpair, Placement sequence[], Ground &ground, string name) {
    g_mutex.lock();

    if (snpair.first < g_best_so_far_score
      || (snpair.first == g_best_so_far_score && snpair.second < g_best_so_far_num_used_stone)) {
      g_best_so_far_score = snpair.first;
      g_best_so_far_num_used_stone = snpair.second;
      Placement::copySequence(sequence, g_best_so_far_sequence);
      g_best_so_far_ground = ground;

      if (g_best_so_far_score == 0) {
        if (!g_zero_blank_found) {
          cout << "\x1b[4m!!!! ZERO BLANK SOLUTION FOUND !!!!\x1b[0m" << endl;
          g_zero_blank_found = true;
        } else {
          cout << "\x1b[4m!!!! ZERO BLANK SOLUTION FOUND !!!!\x1b[0m Fewer stones solution found!" << endl;
        }
      } else {
        cout << "Update best!" << endl;
      }
      cout << "(by " << name << ")" << endl;

      g_found_best_time = chrono::system_clock::now();
      auto elapse = g_found_best_time - g_start_time;
      double seconds = (double)chrono::duration_cast<chrono::milliseconds>(elapse).count() / 1000.0;

      cout << "Score:" << setw(4) << g_best_so_far_score
           << "  Used stone:" << setw(4) << g_best_so_far_num_used_stone
           << "  Elapse: " << fixed << setw(5) << setprecision(1) << seconds << "(s)";

      if (g_selected_stone_mode) {
        cout << "  \x1b[44m[SELECTED STONE MODE]\x1b[49m" << endl;
      } else {
        cout << endl;
      }
      g_best_so_far_ground.print();
      cout << endl;

      Placement::writeSolutionFile("solution.txt", sequence);

      ofstream ofs("score.txt");
      ofs << g_best_so_far_score << " " << g_best_so_far_num_used_stone << endl;
      ofs.close();
    }

    g_mutex.unlock();
  }

  void setBestSoFarNode(Node &best_so_far_node) {
    g_best_so_far_node = best_so_far_node;
  }
  
  Node getBestSoFarNode() {
    Node retval = g_best_so_far_node;

    return retval;
  }
}



