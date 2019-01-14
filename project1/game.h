#ifndef GAME_H
#define GAME_H

// C++ standard I/O and library includes
#include <iostream>
#include <vector>


// Dimensions of the game board
#define IDIM (5)
#define JDIM (5)


// This structure records a move in the game.  The move is given by a
// position and one of 4 directions to move.
struct move {
  int i, j, dir ;
  move() {i=-1;j=-1;dir=-1;}
  move(int in,int jn,int d):i(in),j(jn),dir(d) {}
} ;

struct game_state {
  // This structure saves the state of the board (holes, pegs, noholes)
  enum board_slots {HOLE,PEG,NA} ;
  board_slots board[IDIM*JDIM] ;
  // Access a i,j location of the board
  board_slots &access(int i, int j) { return board[j+i*JDIM] ;}
  board_slots access(int i, int j) const { return board[j+i*JDIM] ;}
  // return the number of pegs in the gameboard
  int size() const ;
  // A winning configuration is when 1 peg is left
  bool Winner() const {return size() == 1 ;}
  int initStringSize() { return IDIM*JDIM; }
  // Inititialize the game state from a char string initStringSize length
  void Init(unsigned char buf[IDIM*JDIM]) ;
  // write the state into a character array
  void SaveBoard(unsigned char buf[IDIM*JDIM]) ;
  // update the board state based on a move
  void makeMove(const move &m) ;
  // check to see if a move is valid according to the game rules
  bool validMove(const move &m) const ;
  // make a list of all valid moves given the current game state
  void validMoveList(std::vector<move> & move_list) const ;
  // print out the board to stream s
  std::ostream &Print(std::ostream &s) const ;
} ;

// Search for a solution to the game, if a solution is found, the
// vector of moves that obtains this is stored in solution
extern bool depthFirstSearch(const game_state &s, int &size, move solution[]) ;


#endif
