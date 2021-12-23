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
   int position[PLAYERS];
   int score[PLAYERS];
   uint64_t qty;
} game;

void advance_position(game* old, game* new, size_t p, int dice){
   new->position[p] = (old->position[p] + dice) % 10;
}

void advance_score(game* old, game* new, size_t p){
   new->score[p] = old->score[p] + (new->position[p]+1); // because position is zero-indexed
}


void calculate_next_state(game gamestate_old, uint64_t* dice_qty, uint64_t* wins){

   // for each option in the dice list.. 
   size_t d[2];
   for( d[0]=3; d[0]<10; ++d[0]){
      for( d[1]=3; d[1]<10; ++d[1]){
         // create a new gamestate
         game gamestate_new;
         gamestate_new.qty = gamestate_old.qty * dice_qty[10*d[1] + d[0]];
         int continue_play = 1;

         for(size_t p=0; p<PLAYERS; ++p){
            // advance position
            advance_position(&gamestate_old, &gamestate_new, p, d[p]);

            // score
            advance_score(&gamestate_old, &gamestate_new, p);

            // exit if there is a winner!
            if (gamestate_new.score[p] >= 21){
               wins[p] += gamestate_new.qty;
               continue_play = 0;
               break; // don't allow the other player to get a win also
            }
         }
         if(continue_play == 1){
            // recursion
            calculate_next_state(gamestate_new, dice_qty, wins);
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

   uint64_t wins[2] = {0, 0};
   calculate_next_state(game_init, dice_list_quantity, wins);

   printf("Player 1 wins: %"PRIu64"\n",wins[0]); 
   printf("Player 2 wins: %"PRIu64"\n",wins[1]);

   return 0;
}
