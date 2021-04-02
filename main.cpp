//cd documents/code/c++/chess/Organized Approach
//g++ main.cpp Centurio.h Centurio.cpp search.cpp move_gen.cpp play_reverse.cpp -o chess.exe


#include "Centurio.h"
#include <iostream>

void print_board(Centurio c){
    char pieces[17] ="._&nkbrq_+_NKBRQ"; 
    std::cout << "  -------------------\n";
    for(int i = 7; i >= 0; i--){
        std::cout << i << " | ";
        for (int j = 0; j < 8; j++){
            std::cout << pieces[c.get_square(j + 16*i)&15] << " ";
        }
        std::cout << "|\n";
    }
    std::cout << "  -------------------\n";
    std::cout << "    0 1 2 3 4 5 6 7\n";
    std::cout << "\n";
}


//just a simple loop so you can play the program
int main(){
    Centurio chess = Centurio();
    print_board(chess);
    int side = 8;
    int a;
    int move;

    int osq;
    int tsq;
    while(true){
        
        while(true){
            
            try{
                
                std::cout << "origin square: ";
                std::cin >> std::hex >> osq;
                std::cout << "\n";
                std::cout << "new square: ";
                std::cin >> std::hex >> tsq;
                std::cout << "\n";
                
                if (chess.player_move(osq*0x100 + tsq, side) >= 0){
                    side = 24 - side;
                    break;
                }
            }
            catch(...){
               std::cout << "error" << std::endl;
            }
        }
        
        
        move = chess.choose_move(side);
        //std::cout << std::hex << move << std::endl;

        if(move<0){
            print_board(chess);
            std::cout<< "game over, the winner is ";
            if(move < -5){ //move = -10, this means if the king is exposed on ply one the side to move wins 
                std::cout << ((side&8) ? "white" : "black") << std::endl;
            }
            else{
                std::cout << ((side&8) ? "black" : "white") << std::endl;
            }
            std::cin >> a;
            return 0;
        }
        chess.play_move(move);
        
        print_board(chess);
        side = 24-side;
        //std::cin>>a;
        if(a > 5){return 0;}
    }

};