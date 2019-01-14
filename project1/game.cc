// C++ standard I/O and library includes
#include <iostream>
#include <fstream>
#include <vector>

// C++ stadard library using statements
using std::cout ;
using std::cerr ;
using std::endl ;

using std::vector ;
using std::ofstream ;
using std::ifstream ;
using std::ios ;

#include "game.h"


int game_state::size() const {
    int count =0 ;
    for(int i=0;i<IDIM*JDIM;++i)
      if(board[i]==1)
        count++ ;
    return count ;
}

void game_state::Init(unsigned char buf[IDIM*JDIM]) {
  for(int i=0;i<IDIM*JDIM;++i)
    switch(buf[i]) {
    case '0':
      board[i] = HOLE ;
      break ;
    case '1':
      board[i] = PEG ;
      break ;
    default:
      board[i] = NA ;
    }
}

void game_state::SaveBoard(unsigned char buf[IDIM*JDIM]) {
  for(int i=0;i<IDIM*JDIM;++i)
    switch(board[i]) {
    case HOLE:
      buf[i] = '0' ;
      break ;
    case PEG:
      buf[i] = '1' ;
      break ;
    default:
      buf[i] = '2' ;
      break ;
    }
}
  void game_state::makeMove(const move &m) {
    const int i = m.i ;
    const int j = m.j ;
    access(i,j) = PEG ;
    switch(m.dir) {
    case 0:
      access(i+1,j) = HOLE ;
      access(i+2,j) = HOLE ;
      break ;
    case 1:
      access(i-1,j) = HOLE ;
      access(i-2,j) = HOLE ;
      break ;
    case 2:
      access(i,j+1) = HOLE ;
      access(i,j+2) = HOLE ;
      break ;
    case 3:
      access(i,j-1) = HOLE ;
      access(i,j-2) = HOLE ;
      break ;
    }
  }

  bool game_state::validMove(const move &m) const {
    const int i = m.i ;
    const int j = m.j ;
    if(access(i,j) != HOLE)
      return false ;
    switch(m.dir) {
    case 0:
      return (i+2 < IDIM) && (access(i+1,j)==PEG) && (access(i+2,j)==PEG) ;
    case 1:
      return (i-2 >= 0) && (access(i-1,j)==PEG) && (access(i-2,j)==PEG) ;
    case 2:
      return (j+2 < JDIM) && (access(i,j+1)==PEG) && (access(i,j+2)==PEG) ;
    default:
      return (j-2 >= 0) && (access(i,j-1)==PEG) && (access(i,j-2)==PEG) ;
    }
  }

  void game_state::validMoveList(std::vector<move> & move_list) const {
    move_list.clear() ;
    for(int i=0;i<IDIM;++i)
      for(int j=0;j<JDIM;++j)
        for(int m=0;m<4;++m){
          if(validMove(move(i,j,m)))
            move_list.push_back(move(i,j,m)) ;
        }
  }
  std::ostream &game_state::Print(std::ostream &s) const {
    for(int j=0;j<JDIM;++j){
      for(int i=0;i<IDIM;++i) 
        if(access(i,j)==PEG)
          s << 'X' ;
        else if(access(i,j)==HOLE)
          s << '*' ;
        else
          s << ' ' ;
      s << std::endl ;
    }
  }

bool depthFirstSearch(const game_state &s, int &size, move solution[]) {
  vector<move> search_tree ;
  s.validMoveList(search_tree) ;
  if(search_tree.size() == 0)
    return s.Winner() ;
  int loc = size ;
  solution[size] = move() ;
  size++ ;
  for(int i=0;i<search_tree.size();++i) {
    solution[loc] = search_tree[i] ;
    game_state new_s = s ;
    new_s.makeMove(solution[loc]) ;
    if(depthFirstSearch(new_s, size, solution))
      return true ;
  }
  size = loc ;
  return false ;
}
  
