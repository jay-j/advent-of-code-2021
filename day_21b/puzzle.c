#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define PLAYERS 2
#define POSITIONS 10
#define SCORES 22
#define BOARD POSITIONS*SCORES

typedef struct{
   int position;
   int score;
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
         printf("%ld\t", board[BOARD*player + POSITIONS*score +pos]);
      }
      printf("\n");
   }
}

int calculate_next_state(const uint64_t* boards, uint64_t* boards_working, size_t player, size_t score, size_t pos, int* dice_qty, uint64_t* wins){
   
   int continue_play = 0;
   
   // multiply results here by number of games in the state being left
   uint64_t qty_src = boards[BOARD*player + POSITIONS*score + pos];

   // for each option in the dice list.. 
   for( size_t d=3; d<10; ++d){
      // calculate what the new state would be
      int new_pos = advance_position(pos, d);
      int new_score = advance_score(score, new_pos);
      uint64_t new_qty = (uint64_t)dice_qty[d] * (uint64_t)qty_src;

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
   int dice_list[11];
   for(size_t i=0; i<11; ++i){
      dice_list[i] = 0;
   }
   for (size_t i=1; i<4; ++i){
      for(size_t j=1; j<4; ++j){
         for(size_t k=1; k<4; ++k){
            int tmp = i+j+k;
            dice_list[tmp] += 1;
         }
      }
   }
   printf("Dice Quantity Options:\n");
   for(size_t i=3; i<10; ++i){
      printf("dice_sum[%ld] = %d\n", i, dice_list[i]);
   }

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
            if (player == 0){
               print_boardstate(boards_working, player);
            }
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

   printf("Player 1 wins: %ld\n",wins[0]); 
   printf("Player 2 wins: %ld\n",wins[1]);

   return 0;
}
