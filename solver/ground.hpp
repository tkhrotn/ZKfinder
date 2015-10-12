#ifndef __GROUND_HPP_
#define __GROUND_HPP_

#include "defines.hpp"

#include <string>

namespace procon26 {
  class Stone;

  /**
    Class for representing ground.
    */
  class Ground {
  public:
    static const int GROUND_BITBOARD_SIZE = 36;   //! Size of ground in BitBoard array representation
    static const int GROUND_BITBOARD_WIDTH = 6;   //! Width of ground in BitBoard array representation
    static const int GROUND_BITBOARD_HEIGHT = 6;  //! Height of ground in BitBoard array representation
    static const int OUTER_IDX[];         //! Indecies of BitBoard array corresponding to outside of ground
    static const int OUTER_IDX_SIZE = 20;     //! Size of outer indecies
    static const int INNER_IDX[];         //! Indecies of BitBoard array corresponding to inside of ground
    static const int INNER_IDX_SIZE = 16;     //! Size of inner indecies
    static const int INNER_IDX_LEFT[];        //! Left position of inner indecies
    static const int INNER_IDX_TOP[];       //! Top position of inner indecies
    static const int INNER_BLOCK_WIDTH = 32;    //! Width of inner in block
    static const int INNER_BLOCK_HEIGHT = 32;   //! Height of inner in block
    static const int WHOLE_BLOCK_WIDTH = 48;    //! Width of whole ground in block
    static const int WHOLE_BLOCK_HEIGHT = 48;   //! Height of whole ground in block
    static const char DEFAULT_COLOR[];
    static const char OBSTACLE_COLOR[];
    static const char STONE_COLOR[];

    Ground();

    /**
      Read initial ground information from string.
      @param problem_string string containing problem information.
      */
    void readProblemString(std::string problem_string);

    /**
      Print current ground to stdout.
      */
    void print(void);

    /**
      Check if a specified Stone arrangement collide with obstacle in the ground.
      @param stone_to_arrange Stone object to be arranged
      @param bx x-axis of the stone
      @param by y-axis of the stone
      */
    bool collideWith(Stone &stone_to_arrange, int bx, int by);

    /**
      Check if a specified Stone arrangement contact with stones in the ground.
      @param stone_to_arrange Stone object to be arranged
      @param bx x-axis of the stone
      @param by y-axis of the stone
      */
    bool contactWithStones(Stone &stone_to_arrange, int bx, int by);

    bool checkNotCollideAndContact(Stone &stone_to_arrange, int bx, int by);

    /**
      Arrange new stone to the ground.
      */
    void arrange(Stone &stone_to_arrange, int bx, int by);

    /**
      Calcualte the number of blocks contacting with an arranged stone.
      */
    int calculateNumContactingBlock();

    int calculateNumContactingBlockInNeumannNeighborhood();

    int calculateNumContactWithObstacle();

    /**
      Calculate the number of open blocks.
      */
    int calculateNumOpenBlock();

    int calculateNumBlockInNeumannNeighborhood(Stone &stone_to_arrange, int bx, int by);

    int calculateNumBlockInNeumannNeighborhoodWithWeight(Stone &stone_to_arrange, int bx, int by);

    int calculateNumOneContact();
    int calculateNumCornerContact();
    int calculateNumBothSideContact();
    int calculateNumThreeContact();
    int calculateNumFourContact();
    int calculateNumTwoClosed();
    int calculateNumLtypeClosed();
    int calculateNumItypeClosed();

    /**
      Check if specified position is obstacle or not.
      */
    bool isObstacle(int bx, int by);

    bool isContact(int bx, int by);

    int findTopmostContact();

    int findTopmostBlank();

    int findBottommostContact();

    int findBottommostBlank();

    int findLeftmostContact();

    int findLeftmostBlank();

    int findRightmostContact();

    int findRightmostBlank();

    BitBoard getOneContactBoard(int idx) {
      return _one_contact[idx];
    }

    BitBoard getCornerContactBoard(int idx) {
      return _corner_contact[idx];
    }

    BitBoard getBothSideContactBoard(int idx) {
      return _both_side_contact[idx];
    }

    BitBoard getThreeContactBoard(int idx) {
      return _three_contact[idx];
    }

    BitBoard getFourContactBoard(int idx) {
      return _four_contact[idx];
    }

  private:
    BitBoard _obstacle[GROUND_BITBOARD_SIZE] = {0};  //! BitBoard array representing existence of an obstacle block
    BitBoard _contact[GROUND_BITBOARD_SIZE] = {0};   //! BitBoard array representing contact with arranged blocks
    
    BitBoard _left_contact[GROUND_BITBOARD_SIZE] = {0};
    BitBoard _right_contact[GROUND_BITBOARD_SIZE] = {0};
    BitBoard _top_contact[GROUND_BITBOARD_SIZE] = {0};
    BitBoard _bottom_contact[GROUND_BITBOARD_SIZE] = {0};

    BitBoard _one_contact[GROUND_BITBOARD_SIZE] = {0};
    BitBoard _corner_contact[GROUND_BITBOARD_SIZE] = {0};
    BitBoard _both_side_contact[GROUND_BITBOARD_SIZE] = {0};
    BitBoard _three_contact[GROUND_BITBOARD_SIZE] = {0};
    BitBoard _four_contact[GROUND_BITBOARD_SIZE] = {0};

    BitBoard _two_closed[GROUND_BITBOARD_SIZE] = {0};
    BitBoard _ltype_closed[GROUND_BITBOARD_SIZE] = {0};
    BitBoard _itype_closed[GROUND_BITBOARD_SIZE] = {0};

    void updateContactAndClosedArray();

    static void arrangeContact(BitBoard array[], BitBoard borad_to_arrange, int bx, int by);
    static void andArray(BitBoard lhs[], BitBoard rhs[], BitBoard result[]);
    static void orArray(BitBoard lhs[], BitBoard rhs[], BitBoard result[]);
    static void shiftArray(BitBoard src[], BitBoard dst[], int x, int y);
    static void copy(BitBoard src[], BitBoard dst[]);
    static void clearArray(BitBoard array[]);
  };
}

#endif // __GROUND_HPP_
