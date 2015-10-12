/**
*/
#ifndef __STONE_HPP_
#define __STONE_HPP_

#include "defines.hpp"
#include "utils.hpp"

#include <string>

namespace procon26 {
	/**
		Class for representing a stone./
		*/
	class Stone {
	private:
		BitBoard _bit_stone;	//! Bitboard representation of the stone
		bool _reversed;				//! Flag if the stone is reversed or not
		Rotate _rotate;				//! Angle of rotation of the stone
		bool _pass;						//! Flag if arrangement of the stone is passed or not
		int _size;				//! Size of the stone
		int _one;
		int _corner;
		int _both_side;
		int _three;
		int _four;

		int _inv_one;
		int _inv_corner;
		int _inv_both_side;
		int _inv_three;
		int _inv_four;

		int _leftmost;
		int _rightmost;
		int _topmost;
		int _bottommost;

	public:
		Stone();

		void setPass(bool pass) {
			_pass = pass;
		}

		bool isPassed() {
			return _pass;
		}

		/**
			Read stone information from string.
			*/
		void readProblemString(std::string problem_string);

		/**
			Get bitboard representation of the stone.
			*/
		BitBoard getBitBoard(void) {
			return _bit_stone;
		}

		int getSize(void) {
			return _size;
		}

		int getNumOneContact(void) {
			return _one;
		}

		int getNumCornerContact(void) {
			return _corner;
		}

		int getNumBothSideContact(void) {
			return _both_side;
		}

		int getNumThreeContact(void) {
			return _three;
		}

		int getNumFourContact(void) {
			return _four;
		}

		int getNumInvOneContact(void) {
			return _inv_one;
		}

		int getNumInvCornerContact(void) {
			return _inv_corner;
		}

		int getNumInvBothSideContact(void) {
			return _inv_both_side;
		}

		int getNumInvThreeContact(void) {
			return _inv_three;
		}

		int getNumInvFourContact(void) {
			return _inv_four;
		}

		Rotate getRotate(void) {
			return _rotate;
		}

		bool isReversed(void) {
			return _reversed;
		}

		/**
			Reverse stone.
			*/
		void reverse(void) {
			_bit_stone = reverseHorizontal(_bit_stone);
			_reversed = true;

			setEdge();
		}

		/**
			Rotate 90 degrees.
			*/
		void rotate90(void) {
			_bit_stone = procon26::rotate90(_bit_stone);
			_rotate = ONE;

			setEdge();
		}

		/**
			Rotate 180 degrees.
			*/
		void rotate180(void) {
			_bit_stone = procon26::rotate180(_bit_stone);
			_rotate = TWO;

			setEdge();
		}

		/**
			Rotate 270 degrees.
			*/
		void rotate270(void) {
			_bit_stone = procon26::rotate270(_bit_stone);
			_rotate = THREE;

			setEdge();
		}

		void setEdge(void) {
      _topmost = findTopmost(_bit_stone);
      _bottommost = findBottommost(_bit_stone);
      _leftmost = findLeftmost(_bit_stone);
      _rightmost = findRightmost(_bit_stone);
		}

		int getLeftmost() {
			return _leftmost;
		}

		int getRightmost() {
			return _rightmost;
		}

		int getTopmost() {
			return _topmost;
		}

		int getBottommost() {
			return _bottommost;
		}
	};
}

#endif // __BITBOARD_HPP_
