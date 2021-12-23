#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define N_PLAYERS 2

typedef struct{
   int position;
   int score;
} player;

void roll_and_advance(player* p, int* dice, int* roll_counter){
   *dice = (*dice + 1) % 100;
   p->position = (p->position + *dice + 1) % 10;
   *roll_counter += 1;
}

void score(player* p){
   p->score += (p->position + 1);
}

int main(int argc, char *argv[]){
   player players[2];
   players[0].score=0;
   players[1].score=0;

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
   players[0].position = atoi(line_raw) - 1;

   fgets_status = fgets(line_raw, sizeof(line_raw), fd);
   assert(fgets_status != NULL);
   players[1].position = atoi(line_raw) - 1;


   printf("player positions:\n");
   for(size_t i=0; i<N_PLAYERS; ++i){
      printf("%d ", players[i].position);
   }
   printf("\n");

   int dice = -1; // zero indexed
   int roll_counter = 0;
   int game_finished = 0;
   int final_score = -1;

   while(game_finished == 0){ // for each round of the game, until somebody wins 
      for(size_t i=0; i<N_PLAYERS; ++i){
         for(size_t j=0; j<3; ++j){
            roll_and_advance(players+i, &dice, &roll_counter);
         }
         score(players+i);

         //printf("Player %ld moves to %d for a total score of %d\n", i+1, players[i].position, players[i].score);
         if (players[i].score >= 1000){
            game_finished = 1;
            final_score = roll_counter * players[1-i].score;
            //printf("game is finished! %d  %d  %d\n", roll_counter, players[1-i].score, final_score);
            break;
         }
      }
   }

   fclose(fd);

   printf("%d\n",final_score); 

   return 0;
}
