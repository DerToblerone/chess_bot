#include "Centurio.h"
#include <iostream>
#define __RNKMSK 0x70
#define __LIMVAL 1000000

short Centurio::generate_moves(short to_move, short level){
    int origin_square = 0,
        new_square,
        capture_square,

        piece,
        target,

        index,
        direction,
        type,
        move_number = 0;

    bool    r_ctl = true,
            l_ctl = true;//these control casteling
    do{     
        if ((piece=board[origin_square]) & to_move){ 
            type = piece&7;
            index = movement_masks[30 + type]; //look up starting index in the directory

            while(direction = movement_masks[++index]){
                new_square = origin_square;//start at original square
                
                //this loop here is inspired by the micro-max program of H.G.Mueller
                do{
                    capture_square = new_square += direction;//add one step in direction, set capture square

                    if (new_square & 0x88){break;}//if not onboard, break

                    target = board[capture_square];//what piece is targeted right now

                    if (target & to_move){break;}//if it is your own piece, break


                    if (type < 3 && !(direction&7) != !target){break;}  /*16 or -16 &7 is 0 because they only have the 16 bit set.    
                                                                        for the other direction &7 returns True
                                                                        so if the direction is straight(16 or -16) the left side is 1
                                                                        and the target square is not empty, a break occurs
                                                                        also if the direction is diagonal the left side is always 0
                                                                        then, if the target square is also empty, the right side is 1 so 
                                                                        a break occurs(pawn can only move diagonally to capture)*/


                    move_buffer[__MOVEBW*level +move_number] = new_square + 0x0100*origin_square;//encodes origin and target into one short val
                    
                    move_number++;
                        
                        

                        

                    if (type < 5){target += 1;}
                    //these pieces can only move one step in each direction (king pawns knight)
                    
                    if(type<3){
                        if (to_move&8){
                            if(!((new_square&__RNKMSK) - 0x20)){//double pawn move for white
                                //temp_square = new_square; 
                                target--;
                            } 
                        }
                        else{
                            if(!((new_square&__RNKMSK) - 0x50)){//double pawn move for black
                                //temp_square = new_square; 
                                target--;
                            } 
                        }
                    }
                    else if((piece&~24)==36){//filters out unmoved king
                        if((direction == 1) && r_ctl){ //moving sideways and not castled yet
                            if(!(board[origin_square + 1]|board[origin_square + 2])){ //checks squares in between
                                if(board[origin_square+3]&32){                        //checks if rook in the corner has moved
                                    target--;
                                    r_ctl = false;
                                }
                            }
                        }
                        else if((direction == -1) && l_ctl){//analogous for left side
                            if(!(board[origin_square - 1]|board[origin_square - 2]|board[origin_square - 3])){
                                if(board[origin_square-4]&32){
                                    target--;
                                    l_ctl = false;
                                }
                            }
                        }
                    }
                }while(!target);
            }
        }
    }while(origin_square=origin_square+9&0x77);

    move_buffer[__MOVEBW*level +move_number] = 0xFFFF;
    return 0;
}

void Centurio::swap_move(short level, short i, short j){ //swap i and j
    switch_var = move_buffer[__MOVEBW*level + i];//reorder move list so good move gets searched first
    move_buffer[__MOVEBW*level +i] = move_buffer[__MOVEBW*level +j];
    move_buffer[__MOVEBW*level +j] = switch_var;
}


int Centurio::quiescience_moves(short to_move, short level){    //same as normal gen move, but only moves that capture are considered
                                                                //thus we also do not need special move checks
    int origin_square = 0,
        new_square,
        capture_square,

        piece,
        target,

        index,
        direction,
        type,

        move_number = 0;

    bool    r_ctl = true,
            l_ctl = true;//these control casteling
    do{     
        if ((piece=board[origin_square]) & to_move){ 
            type = piece&7;
            index = movement_masks[30 + type]; //look up starting index in the directory

            while(direction = movement_masks[++index]){
                new_square = origin_square;//start at original square

                do{
                    capture_square = new_square += direction;//add one step in direction, set capture square

                    if (new_square & 0x88){break;}//if not onboard, break
                    
                    target = board[capture_square];//what piece is targeted right now

                    if (target & to_move){break;}//if it is your own piece, break

                    if (type < 3 && !(direction&7) != !target){break;}  /*16 or -16 &7 is 0 because they only have the 16 bit set.    
                                                                        for the other direction &7 returns True
                                                                        so if the direction is straight(16 or -16) the left side is 1
                                                                        and the target square is not empty, a break occurs
                                                                        also if the direction is diagonal the left side is always 0
                                                                        then, if the target square is also empty, the right side is 1 so 
                                                                        a break occurs(pawn can only move diagonally to capture)*/
                    
                    if (target&7 == 4){
                        return __LIMVAL;
                    }
                    else if(target&7 > 2){//if a piece of greater value than a pawn is captured, examine this further
                        move_buffer[__MOVEBW*level +move_number] = new_square + 0x0100*origin_square;//encodes origin and target into one short val
                        move_number++;
                    }
                    
                    if (type < 5){target += 1;}
                    
                }while(!target);
            }
        }
    }while(origin_square=origin_square+9&0x77);

    move_buffer[__MOVEBW*level +move_number] = 0xFFFF;
    return 0;
}