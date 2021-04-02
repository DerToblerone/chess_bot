#include "Centurio.h"
#include <iostream>
#define __MOVEBW 256


void Centurio::prune_move_list(int start, int level){
    move_buffer[level*__MOVEBW + start] = 0xFFFF; //add break signal to move list 
}

int Centurio::get_square(int sq){
    return board.at(sq); //return the square of the board at sq
}