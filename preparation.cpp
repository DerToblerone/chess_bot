#include "Centurio.h"

short Centurio::prep_square_vals(short to_move){

    int total_material = 0,
        my_king_square = 0,
        opp_king_square = 0;
    
    material_difference =0;


    //some static return moves, play e4 and answer e5 to e4
    
    if(!move_count){
        return 0x1434;
    }
    if(move_count == 1){
        if(board[0x34]){
            return 0x6444;
        }
        if(board[0x24]){
            return 0x6444;
        }
    }




    //find out about material
    for (int i = 0; i < 128; i++){
            if(board[i]&7 == 4){
                if(board[i]&to_move){
                    my_king_square = i;
                }
                else{
                    opp_king_square = i;
                }
            }
            material_difference += ((board[i]&to_move) ? 1 : -1)*weights[board[i]&7];
            total_material += weights[board[i]&7];
        }

    //remove the incentive to go on casteling square after casteling
    if(board[0x04] == 0){
        for (int i = 4*128; i <(5*128); i++){
            square_table[i] = 0;
        }
    }
    if(board[0x74] == 0){
        for (int i = 4*128; i <(5*128); i++){
            square_table[i + 1024] = 0;
        }
    }

    //if in the lead, make captures and trades more appealing. if behind, avoid trades or captures at all costs
    for(int j = 0; j < 16; j++){
        if(weights[j&7] != 0){
            adjusted_weights[j] = weights[j&7] - ((material_difference/50)*((to_move&8)?(j>7):(j < 8)));
        }
    }


    int distance;
    //set up so that king moves towards the enemie king in the endgame
    if ((total_material <= 2400)|| ((total_material <  4000) && (material_difference > 1000))){//make king approach other king in the endgame
        if(material_difference >= 0){
            if(to_move&8){
                for (int i = 4*128; i <(5*128); i++){
                    distance = abs(i/0x10 -opp_king_square/0x10) + abs(i&0x7 - opp_king_square&0x7);
                    square_table[i] = 15 - distance;
                }
            }
        }
    }


    
    //after the opening has finished, remove the square values (otherwise the program thinks it can generate high scores by moving only on these squares)
    if(move_count >= 16 && move_count < 20){//opening is over
        for (int i = 3*128; i <(4*128); i++){
            square_table[i] = 0;
            square_table[i + 1024] = 0; //the opening is over and the table needs to change
        }
        for (int i = 5*128; i <(6*128); i++){
            square_table[i] = 0;
            square_table[i + 1024] = 0;
        }
    }
    else if(move_count > 22){
    //dont forget knight developement
    
        if(board[0x01]==11){
            square_table[(3*128) + 0x22] = 6;
        }
        if(board[0x06]==11){
            square_table[(3*128) + 0x25] = 6;
        }
        if(board[0x71]==19){
            square_table[(3*128) + 1024 + 0x52] = 6;
        }
        if(board[0x76]==19){
            square_table[(3*128) +  1024 +0x55] = 6;
        }
    }

    return 0;
}

void set_endgame_mode(){}