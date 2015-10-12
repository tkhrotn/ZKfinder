#ifndef __PLACEMENT_HPP_
#define __PLACEMENT_HPP_

#include <string>
#include <fstream>

namespace procon26 {
  extern Stone g_stones[MAX_STONE][NUM_PLACEMENT_PATTERN];
  extern int g_num_stone;

  struct Placement {
    int _pattern;   //! Rotation and inversion pattern of Stone to be arranged
    int _bx;        //! x-axis of Stone arrangement
    int _by;        //! y-axis of Stone arrangement
    //bool _first_arrange; //! Flag if it is the first arrangement or not

    Placement() 
    : _pattern(UNPLACED_PATTERN) {
    }

    Placement(int pattern, int bx, int by)
    : _pattern(pattern), _bx(bx), _by(by) {
    }

    bool operator==(const Placement& rhs) const {
        return _pattern == rhs._pattern && _bx == rhs._bx && _by == rhs._by;
    }

    static bool isSameSequence(Placement lhs[], Placement rhs[], int order) {
        for (int i = 0; i < order; i++) {
            if (!(lhs[i] == rhs[i])) {
                return false;
            }
        }

        return true;
    }

    static void copySequence(Placement source[], Placement dest[]) {
      for (int i = 0; i < g_num_stone; i++) {
            dest[i] = source[i];
        }
    }

    static void writeSolutionFile(std::string filename, Placement sequence[]) {
        std::ofstream ofs;
        ofs.open(filename);

        for (int i = 0; i < g_num_stone; i++) {
            if (sequence[i]._pattern == PASS_PATTERN || sequence[i]._pattern == UNPLACED_PATTERN) {
                ofs << "\r" << std::endl;
            } else {
                ofs << sequence[i]._bx - BOARD_WIDTH << " "
                    << sequence[i]._by - BOARD_HEIGHT << " ";

                if (g_stones[i][sequence[i]._pattern].isReversed())
                    ofs << "T ";
                else
                    ofs << "H ";

                switch (g_stones[i][sequence[i]._pattern].getRotate()) {
                case ZERO:
                    ofs << "0\r" << std::endl;
                    break;
                case ONE:
                    ofs << "90\r" << std::endl;
                    break;
                case TWO:
                    ofs << "180\r" << std::endl;
                    break;
                case THREE:
                    ofs << "270\r" << std::endl;
                    break;
                }
            }
        }

        ofs.close();
    }
  };
}

#endif // __PLACEMENT_HPP_
