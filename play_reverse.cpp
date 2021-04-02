#include "Centurio.h"
#include <iostream>
#include <stdlib.h>

short Centurio::play_move(short move){  //this function modiefies the board and returns a short
                                        //the return value contains information about casteling 
                                        //and if it was the first move of that piece
    int origin_square = move/0x0100,
        new_square = move%0x0100,
        piece,
        temp,
        target;

    bool first_move = false;
    promotion = false;
    move_count++;

    //after the variable setup, we determine if a first move takes place

    piece = board[origin_square];
    if(piece&32){
        first_move = true;
        piece = piece&31;
        first_move_bonus = 10-(piece&0x06); //small bonus to move a piece for the first time
    }

    if((piece&7) == 4){//this is for casteling
        temp =new_square - origin_square;
        if(temp == 2){
            //kingside castle
            board[new_square - 1] = board[new_square + 1]&31;
            board[new_square + 1] = 0;
        }
        else if(temp == -2){
            //queenside castle
            board[new_square + 1] = board[new_square - 2]&31;
            board[new_square - 2] = 0;
        }
    }
    else if((piece&7) < 3){//and this for pawn promotion
        if(((new_square&0x70) == 0) | ((new_square&0x70) == 0x70)){
            promotion = true;
            piece = 7 + (piece&24);
        }
    }
    
    //set the new squares on the board
    target = board[new_square];

    board[new_square] = piece;//remove virigin bit
    board[origin_square] = 0;

    return first_move ? (target + 128) : target; //return target piece + first move info

}

short Centurio::reverse_move(short move, short target, bool prom){ 
    int origin_square = move/0x0100,
        new_square = move%0x0100,
        piece,
        temp;

    move_count--;
    piece = board[new_square];

    //same as in play move, just take everything back

    if(prom){ //revert to pawn after promotion
        piece = (piece&24) + ((piece&24)/8);
    }

    if((piece&7) == 4){
        temp =new_square - origin_square;
        if(temp == 2){
            //kingside castle
            board[new_square + 1] = board[new_square - 1] +32;
            board[new_square - 1] = 0;
        }
        else if(temp == -2){
            //queenside castle
            board[new_square - 2] = board[new_square + 1] + 32;
            board[new_square + 1] = 0;
        }
    }
    
    board[origin_square] = (target&128) ? (piece + 32) : piece; //reset virgin bit if it was previously set
    board[new_square] = target&127;//remove the "information" part
    return 0;
    
    

}

short Centurio::player_move(short move, short to_move){//if the player enters a move, it is played if it is found in the generated list
    generate_moves(to_move, 0);
    for(int i = 0; i <218; i++){
        if (move_buffer[i]<0xFFFF){
            if(move == move_buffer[i]){
                return play_move(move);
            }
        }
    }
    return -1;
}