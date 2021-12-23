#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define PLAYERS 2
#define DICE_SIDES 3
#define DICE_LIST 100

typedef struct{
   size_t position[PLAYERS];
   size_t score[PLAYERS];
   uint64_t qty;
} game;


void advance_position(game* old, game* new, size_t* dice, size_t p){
   new->position[p] = (old->position[p] + dice[p]) % 10;
}


void advance_score(game* old, game* new, size_t p){
   new->score[p] = old->score[p] + (new->position[p]+1); // because position is zero-indexed
}


// recursively play the game, for each possible path keep track of how many possible games can take it
void calculate_next_state(game gamestate_old, uint64_t* dice_qty, uint64_t* wins){

   // for each option in the dice list.. 
   for(size_t d1=3; d1<10; ++d1){
      int player1_wins_first = 0;

      for(size_t d2=3; d2<10; ++d2){
         size_t d[PLAYERS] = {d1, d2};

         // create a new gamestate
         game gamestate_new;
         int continue_play = 1;

         for(size_t p=0; p<PLAYERS; ++p){
            // advance position
            advance_position(&gamestate_old, &gamestate_new, d, p);

            // score
            advance_score(&gamestate_old, &gamestate_new, p);

            // exit if there is a winner!
            if (gamestate_new.score[p] >= 21){
               if (p == 0){
                  // only look at probable quantities from 3 dice (not six like if both players get to roll)
                  player1_wins_first = 1;
                  int dice_tmp[10] = {0, 0, 0, 1, 3, 6, 7, 6, 3, 1};
                  gamestate_new.qty = gamestate_old.qty * dice_tmp[d[0]];
               }
               else{
                  // only one possible roll if player 2 wins
                  gamestate_new.qty = gamestate_old.qty * dice_qty[10*d[1] + d[0]];
               }
               wins[p] += gamestate_new.qty;
               continue_play = 0;
               break; // don't allow the other player to get a win also
            }
         }
         if(continue_play == 1){
            // recursion
            gamestate_new.qty = gamestate_old.qty * dice_qty[10*d[1] + d[0]];
            calculate_next_state(gamestate_new, dice_qty, wins);
         }

         if(player1_wins_first == 1){
            break;
         }

      }
   }
}

int main(int argc, char *argv[]){
   game game_init;
   game_init.qty = 1;
   game_init.score[0] = 0;
   game_init.score[1] = 0;

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
   game_init.position[0] = p1pos;
   printf("P0 pos: %d (zero-indexed)\n", p1pos);

   // get player 2 position
   fgets_status = fgets(line_raw, sizeof(line_raw), fd);
   assert(fgets_status != NULL);
   int p2pos = atoi(line_raw) - 1;
   game_init.position[1] = p2pos;
   printf("P1 pos: %d (zero-indexed)\n", p2pos);
   fclose(fd);

   // Build list of potential dice options
   // sum of each dice could be one of 7 values: [3,9]
   // so possible space of two is 7*7 or 49
   // index.. 10*p2_val + p1_val ..... AKA these coordinates are the points to add!
   // minimum value is 3..  but it is not too bad to use a bigger list than required
   uint64_t dice_list_quantity[DICE_LIST];
   for(size_t i=0; i<DICE_LIST; ++i){
      dice_list_quantity[i] = 0;
   }
   for(int a=1; a<=DICE_SIDES; ++a){
      for(int b=1; b<=DICE_SIDES; ++b){
         for(int c=1; c<=DICE_SIDES; ++c){
            
            for(int d=1; d<=DICE_SIDES; ++d){
               for(int e=1; e<=DICE_SIDES; ++e){
                  for(int f=1; f<=DICE_SIDES; ++f){

                     uint64_t p1_dice_val = a+b+c;
                     uint64_t p2_dice_val = d+e+f;
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
         printf("%ld\t", dice_list_quantity[10*i+j]);
      }
      printf("\n");
   }

   // recursively play the game
   uint64_t wins[2] = {0, 0};
   calculate_next_state(game_init, dice_list_quantity, wins);

   printf("Player 1 wins: %"PRIu64"\n",wins[0]); 
   printf("Player 2 wins: %"PRIu64"\n",wins[1]);

   return 0;
}
