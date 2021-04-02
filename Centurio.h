#include <array>
#define __MOVEBW 256


//actual chess class
class Centurio{

    private:
        std::array<int,128> board;              //stores a chess position
        std::array<int,128*8*2> square_table; //contains values for squares

        std::array<int, 38> movement_masks;                //contains directions for piece movement

        std::array<unsigned int,__MOVEBW*99>  move_buffer;    //keeps all moves calculated in generate_moves for a specific level. saves time of memalloc

        std::array<int, 8> weights;                         //piece values
        std::array<int, 16> adjusted_weights;               //


        int rank_mask,                          //used to extract rank from a square
            file_mask,                          //analogous for file
            nodes_searched,                     //gets incremented until it reaches the threshold max nodes
            switch_var,                         //this is used whan ordering the moves to search
            
            
            move_count,
            first_move_bonus,
            material_difference;


        bool promotion;//flag for pawn promotion
                                

        short generate_moves(short to_move, short level);    //generates a list of moves of length 218 for a given depth level 

        int no_king_capture(short to_move, short level);      //returns -inf if the king can be catured
        void swap_move(short level, short i, short j);                      //changes positions of moves in the search list. this is very important
        
        void prune_move_list(int start, int level);     //used to cut off move list so only "good" moves are evaluated further; disabled atm.
        int quiescience_moves(short to_move, short level); //analyze capture sequences till the end

        short prep_square_vals(short to_move); //do some preprocessing at the beginning of each turn(prepare the adjusted weights, set valuable squares)
    
    public:
        Centurio();                             //Constructor initializes board and important values

        short choose_move(short to_move);   //returns the move to be played as a short 0xAABB wher AA is the origin square, BB the target square
        long search_pos(short to_move, short depth, int alpha, int beta, int eval, short level);//The companion squares containing en passant and casteling info are stored in comp_sq
        
        short play_move(short move);          //playes move and returns target
        short reverse_move(short move, short target, bool prom);
        
        short player_move(short move, short to_move); //to test move generation

        int get_square(int sq);                              //return a square of the board
        



};

