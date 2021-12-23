#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define PLAYERS 2
#define POSITIONS 10
#define SCORES 22
#define BOARD POSITIONS*SCORES
#define DICE_SIDES 3
#define DICE_LIST 100

typedef struct{
   int position[PLAYERS];
   int score[PLAYERS];
   uint64_t qty;
} game;

int advance_position(int position, int dice){
   int new_pos = (position + dice) % 10;
   return new_pos;
}

int advance_score(int score, int new_pos){
   score += (new_pos+1); // because position is zero-indexed
   return score;
}

void zero_board(uint64_t* b){
   for (size_t i=0; i<PLAYERS*BOARD; ++i){
      b[i] = 0;
   }
}

void print_boardstate(uint64_t* board, size_t player){
   printf("--Board State for Player %ld --\n", player);
   for (int pos=-1; pos<POSITIONS; ++pos){
      printf("%d\t", pos);
   }
   printf("\n------------------------------------------------------------------------------------\n");
   for (size_t score=0; score<SCORES; ++score){
      printf("%ld\t|", score);
      for(size_t pos=0; pos<POSITIONS; ++pos){
         printf("%"PRIu64"\t", board[BOARD*player + POSITIONS*score +pos]);
      }
      printf("\n");
   }
}

int calculate_next_state(const uint64_t* boards, uint64_t* boards_working, size_t score, size_t pos, int* dice_qty, uint64_t* wins){

   int continue_play = 0;
   
   // multiply results here by number of games in the state being left
   uint64_t qty_src = boards[BOARD*player + POSITIONS*score + pos];

   // for each option in the dice list.. 
   for( size_t d1=3; d1<10; ++d1){
      for(size_t d2=3; d2<10; ++d2){
      // calculate what the new state would be
      int new_pos = advance_position(pos, d1);
      int new_score = advance_score(score, new_pos);
      uint64_t new_qty = (uint64_t)dice_qty[d1 * (uint64_t)qty_src;

      if (new_score >= 21){
         new_score = 21;
         *wins += new_qty; 
      }
      else{
         continue_play = 1;
      }

      // add to boards_working
      boards_working[BOARD*player + POSITIONS*new_score + new_pos] += new_qty;
      }

    }

   return continue_play;
}

int main(int argc, char *argv[]){
   // create storage structure.. index: (POSITIONS*SCORES)*player + (POSITIONS)*scores + position
   // need to expand state space to track what happens to who...
   uint64_t boards[PLAYERS*BOARD];
   zero_board(boards);
   uint64_t boards_working[PLAYERS*BOARD]; // temporary game state while steps are computed

   // read in the file
   char filename[64];
   sscanf(argv[1], "%s", filename);
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);

   // read the first line, get player 1 position
   char line_raw[10];
   char* fgets_status;
   fgets_status = fgets(line_raw, sizeof(line_raw), fd);
   assert(fgets_status != NULL);
   int p1pos = atoi(line_raw) - 1;
   boards[(POSITIONS*SCORES)*0 + (POSITIONS)*0 + p1pos] = 1;
   printf("P0 pos: %d (zero-indexed)\n", p1pos);

   fgets_status = fgets(line_raw, sizeof(line_raw), fd);
   assert(fgets_status != NULL);
   int p2pos = atoi(line_raw) - 1;
   boards[(POSITIONS*SCORES)*1 + (POSITIONS)*0 + p2pos] = 1;
   printf("P1 pos: %d (zero-indexed)\n", p2pos);
   fclose(fd);

   // Build list of potential dice options
   // sum of each dice could be one of 7 values: [3,9]
   // so possible space of two is 7*7 or 49
   // index.. 10*p2_val + p1_val ..... AKA these coordinates are the points to add!
   // minimum value is 3..  but it is not too bad to use a bigger list than required
   int dice_list_quantity[DICE_LIST];
   for(size_t i=0; i<DICE_LIST; ++i){
      dice_list_quantity[i] = 0;
   }
   for(int a=1; a<=DICE_SIDES; ++a){
      for(int b=1; b<=DICE_SIDES; ++b){
         for(int c=1; c<=DICE_SIDES; ++c){
            
            for(int d=1; d<=DICE_SIDES; ++d){
               for(int e=1; e<=DICE_SIDES; ++e){
                  for(int f=1; f<=DICE_SIDES; ++f){

                     int p1_dice_val = a+b+c;
                     int p2_dice_val = d+e+f;
                     dice_list_quantity[10*p2_dice_val + p1_dice_val] += 1;
                  }
               }
            }
         }
      }
   }
   
   printf("Dice Quantity Options:\n");
   for(size_t i=0; i<10; ++i){
      for(size_t j=0; j<10; ++j){
         printf("%d\t", dice_list_quantity[10*i+j]);
      }
      printf("\n");
   }

   // now squash the dice list.. don't actually need it to be 2D
   int dice_list[10];
   for(size_t i=0; i<10; ++i){
      dice_list[i] =0;
   }
   for(size_t i=0; i<10; ++i){
      for(size_t j=0; j<10; ++j){
         dice_list[i] += dice_list_quantity[10*i+j];
      }
      printf("%d ", dice_list[i]);
   }
   printf("\n");
   

   int continue_play_latched[2] = {1, 1};
   uint64_t wins[2] = {0, 0};

   while((continue_play_latched[0] == 1) & (continue_play_latched[1] == 1)){
      zero_board(boards_working);
      int continue_play_tmp[2] = {0, 0};

      // for each unique game state 
      for(size_t player=0; player<2; ++player){
         if (continue_play_latched[player] > 0){
            for(size_t score=0; score<SCORES-1; ++score){ // NOT ALL SCORES; don't continue to play finished boards
               for(size_t pos=0; pos<POSITIONS; ++pos){
                  // if there are some boards in this state..
                  if (boards[BOARD*player + POSITIONS*score + pos] > 0){
                     // calculate what the next states (poisition & score) could be
                     // result is stored in boards working
                     continue_play_tmp[player] += calculate_next_state(boards, boards_working, player, score, pos, dice_list, wins+player);

                  }
               }
            }
            //if (player == 0){
               print_boardstate(boards_working, player);
            //}
         }
      }

      // all steps played, now copy over
      for(size_t i=0; i<PLAYERS*BOARD; ++i){
         boards[i] = boards_working[i];
      }

      // put continue play flags to use
      for(size_t i=0; i<PLAYERS; ++i){
         if (continue_play_tmp[i] == 0){
            continue_play_latched[i] = 0;
         }
      }

   }
   // possible states = options_score * options_position * options_dice3
   // compute range of states for each? 
   //
   // track number of outcomes that have each amount of points? and have each 
   // track number of outcomes in each possible game state? 

   printf("Player 1 wins: %"PRIu64"\n",wins[0]); 
   printf("Player 2 wins: %"PRIu64"\n",wins[1]);

   return 0;
}
