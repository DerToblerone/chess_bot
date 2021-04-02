#include "Centurio.h"
#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#define __MOVEBW 256
#define __LIMVAL 1000000
#define __MAXNOD 66000000
//__MOVEBW ... max amount of moves per position 256 is probably too much
//__LIMVAL ... represents infinity
//__MAXNOD ... max amounts of nodes to search before returning


short Centurio::choose_move(short to_move){
    int depth = 1, //gets iteratively incremented
        move,       //stores the move to be tried next
        best_move = 0xFFFF, //stores the final best move to return
        temp_move, //stores the candidate move for best move
        type,       //type of the piece that is moved

        prep, //if this is > 0 we have a book move

        alpha = -__LIMVAL, //lower bound for score
        max_score,          //max score of the top level
        result = -__LIMVAL, //actual result, gets updated after each depth iteration
        
        score,          //here the return of the partial search gets stored
        delta_score,    //score changes that happened this layer
        last_score,     //score of the last move(also just for move list ordering)
        
        target,         //piece on the target square
        start_eval;     //the positional value computed in the preparatio

    nodes_searched = 0; //start a fresh search by setting the searched nodes to 0

    bool prom;//keep track of promotions

    
    prep = prep_square_vals(to_move); //do the pre search setup
    
    start_eval = material_difference; //feed in the base score
    if(prep){   //sometimes, a fixed move is programmed into prep
        std::cout << "search done" <<std::endl;
        return prep; //we play that right away
    }


    move_buffer.fill(0XFFFF); //fill the move list with break symbols
    generate_moves(to_move, 0); //generate moves for the first search layer

    while(depth < 20){ //max depth = 20
        depth++;        //increment depth to search one ply deeper every iteration. we use the previous results to order the moves in the list

        //prepare some variables
        max_score = -__LIMVAL;
        alpha = -__LIMVAL;
        last_score = __LIMVAL;
        
        int score_change_tracker = 0;
        
        //go through the move list step by step
        for(int i = 0; i <218; i++){
            
            //put the current move into move
            move = move_buffer[0*__MOVEBW + i];

            if (move<0xFFFF){ //if it is a valid move, go on with search

                //first setup some variables
                prom = false;
                delta_score = start_eval;
                first_move_bonus = 0; //this actually gets modified in play move

                //now the moves is played:
                target = play_move(move);//check what is on the target square right now

                if(promotion){prom= true;} //sets the promotion flag, this needs to be done since promotion might get changed a lot in the search

                if(weights[target&7] < 0){ //this means the enemy king was captured and the game is over
                    reverse_move(move, target, prom);
                    std::cout << "search done" <<std::endl;

                    return -10;
                }

                //here the change of the score is calculated; weight of piece captured + square table value + first move bonus + promotion
                type = board[move&0x77]&7;
                delta_score += adjusted_weights[target&15] + square_table[(1024*(!(to_move&8))) + (128*type) + (move&0x77)] + first_move_bonus + (800*prom);


                //here the move is searched
                score = -search_pos(24- to_move, depth-1, -__LIMVAL, -alpha, -delta_score, 1);

                //after the search, we take back the move
                reverse_move(move, target, prom);

                //then the nodes searched are incremented
                nodes_searched++;
                if(nodes_searched > __MAXNOD){ //if we are greater than the max, we return the best move. or -1 if we lose next move
                    std::cout << "search done" <<std::endl;

                    if(result <= 1 - __LIMVAL){
                        return -1;
                    }   
                    return best_move;
                }

                if(score>0){score--;} //if our score improved, we decrease it. this ensures that gain over time is valued less than immediate gain

                if(score >= max_score){
                    max_score = score;
                    score_change_tracker++;
                    temp_move = move;

                    //if the max has changed, the top moves of the layer need to be updated:
                    if(i < 4){
                        swap_move(0, 0, i);
                    }
                    else if (i < 8){
                        swap_move(0, 2, 3); //take previous best move and push him back
                        swap_move(0, 1, 2); // so that the move replaced is not of the top 4
                        swap_move(0, 0, 1);
                        swap_move(0, 0, i);
                    }
                    else{
                        swap_move(0, 6, 7); //take previous best move and push him back
                        swap_move(0, 5, 6); // so that the move replaced is not of the top 8
                        swap_move(0, 4, 5);
                        swap_move(0, 3, 4);
                        swap_move(0, 2, 3); 
                        swap_move(0, 1, 2); 
                        swap_move(0, 0, 1);
                        swap_move(0, 0, i);
                    }

                    
                }
                else if (score > last_score){ // if the max was not surpassed, we see if we can atleast swap this move and the previous one
                    swap_move(0, i-1, i);
                    
                }

                //set a new best lower bound                
                alpha = std::max(max_score, alpha);

                last_score = score;

            }
            else{//if no moves are left, break out of loop
                break;
            }
        }
        //update the results
        result = max_score;
        best_move = temp_move;


        std::cout << std::dec <<"max score of layer " << depth << ": " << max_score << " Total score changes: " << score_change_tracker << std::endl;
        //another max node check, maybe redundant
        if(nodes_searched > __MAXNOD){
            std::cout << "search done" <<std::endl;

            if(result <= 1-__LIMVAL){
                return -1;
            }
            
            return best_move;
        }
        
    }
    //if we lost, return -1
    if(result <= 1-__LIMVAL){
        return -1;
    }
    //else return best move
    std::cout << "search done" <<std::endl;

    return best_move;
}