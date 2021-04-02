#include "Centurio.h"
#include <iostream>
#define __LIMVAL 1000000
#define __MAXNOD 66000000

//limval is the value for inf
//maxnod is the max number of nodes to search

//this works basically the same as choose_move (in move_select.cpp) but it has small differences

long Centurio::search_pos(short to_move, short depth, int alpha, int beta, int eval, short level){
    int score;
    
    //if a leaf node is reached, we examine all captures(of value greater than pawn) if the base evaluation of our position is still in the alpha-beta window
    //this is based on the assumption that if we exceed beta, at least one move exists that can match or even surpass the current score. so no further
    //examination needed
    if(depth == 0){
        if(eval < beta){ //stand pat score > beta, stop searching
            if(quiescience_moves(to_move, level)){
                return __LIMVAL;
            }
            if((move_buffer[__MOVEBW*level]) >= 0xFFFF){
                return eval;
                //if no captures are generated, just move on
            }
        }
        else{
            return eval;
        }
    }
    else{
        //if we are not at a leaf node, generate moves the normal way
        generate_moves(to_move, level);
    }

    //same setup as in choose_move
    int current_depth = 0,
        target,

        last_score,
        delta_score = 0,        //here the score changes are accumulated
        max_score = -__LIMVAL,
        result = - __LIMVAL,

        move;               
    
    bool  prom; //keep record if promotion happened

    do{ //we also do iterative deepening here. but we jump straight to full depth after a 2 ply search
        current_depth++;
        if(current_depth > 2){current_depth = depth;} //we want to search to the depth specifeid in the argument
        
        max_score = -__LIMVAL;
        last_score = __LIMVAL;

        //the rest here is identical to choose_move
        for(int i = 0; i <218; i++){
            
            move = move_buffer[__MOVEBW*level + i];
            if (move<0xFFFF){
                prom = false;
                delta_score = 0;
                first_move_bonus = 0;
                if(max_score > beta){break;}

                target = play_move(move);
                if(promotion){prom = true;}

                
                if(weights[target&7] < 0){
                    reverse_move(move, target, prom);
                    return __LIMVAL;
                }

                
                
                //                                                                        here i wrote type berfore, but this safes mem alloc time
                delta_score += adjusted_weights[target&15] + square_table[(1024*(!(to_move&8))) + (128*(board[move&0x77]&7)) + (move&0x77)] + first_move_bonus + (800*prom);//8*128 = 1024
                if(delta_score > 0){delta_score--;}


                score = -search_pos(24- to_move, current_depth-1, -beta, (alpha>max_score) ?  -alpha : -max_score, -eval -delta_score, level + 1);


                if(score > eval){score--;}

                reverse_move(move, target, prom);

                nodes_searched++;
                if(nodes_searched > __MAXNOD){if(current_depth > 1){return result;}}//return result;}
                
                
               if(score >= max_score){
                    max_score = score;
                    
                    if(i < 4){
                    swap_move(level, 0, i);
                    }
                    else{
                        swap_move(level, 2, 3); //take previous best move and push him back
                        swap_move(level, 1, 2); // so that the move replaced is not of the top 3
                        swap_move(level, 0, 1);
                        swap_move(level, 0, i);
                    }

                }
                else if (score > last_score){
                    swap_move(level, i-1,i);

                }

                last_score = score;


            }
            else{
                break;
            }

        }//loop over moves
        result = max_score;
        
    }while(current_depth < depth);
    
    return result;
    

}
