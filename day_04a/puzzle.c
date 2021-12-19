#include <stdio.h>
#include <assert.h>
#include <string.h>

#define DATA_MAX 1024
#define MAX_STEPS 100
#define BOARD_SIZE 5

// number of bits
size_t game_steps;

void print_board(int* board){
   for(size_t i=0; i<BOARD_SIZE; ++i){
      for(size_t j=0; j<BOARD_SIZE; ++j){
         printf("%2d ", board[BOARD_SIZE*i+j]);
      }
      printf("\n");
   }
}

// TODO
int score_board(int* board){
   int result = 0;
   for (size_t i=0; i<BOARD_SIZE*BOARD_SIZE; ++i){
      result += board[i];
   }

   return result;
}

// TODO
// input number_choice[], input board[25]
// return steps to win
// return score when win
int play_game(int* number_choice, int* board, int* score){
   int step = 0;
   while (step < game_steps){

      // search board for number match, remove if it is the case
      for (size_t i=0; i<BOARD_SIZE*BOARD_SIZE; ++i){
         board[i] -= number_choice[step]*(board[i] == number_choice[step]);
      }

      int win = 0;
      // search for win condition row
      for (size_t i=0; i<BOARD_SIZE; ++i){
         int win_row = 1;
         for (size_t j=0; j<BOARD_SIZE; ++j){
            win_row &= (board[5*i+j] == 0);
         }
         win += win_row;
      }
      // search for win condition col
      for (size_t i=0; i<BOARD_SIZE; ++i){
         int win_col = 1;
         for (size_t j=0; j<BOARD_SIZE; ++j){
            win_col &= (board[5*j+i] == 0);
         }
         win += win_col;
     }

      if (win > 0){
         break;
      }

      ++step;
   }

   *score = number_choice[step]*score_board(board);
   return step;
}

int main(int argc, char *argv[]){

   char filename[64];
   sscanf(argv[1], "%s", filename);

   // read in the file
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);
   int status;

   // read the first line, figure out how many characters to use
   char line1_raw[1024];
   char* fgets_status;
   fgets_status = fgets(line1_raw, sizeof(line1_raw), fd);
   assert(fgets_status != NULL);
   //printf("got the first line: %s\n", line1_raw);

   // extract pieces from that first line
   char* line1_working = line1_raw;
   int number_choice[MAX_STEPS];
   game_steps = 0;
   while (game_steps < MAX_STEPS){
      char* token = strtok_r(line1_working, ",", &line1_working);
      if (token == NULL){
         break;
      }
      
      status = sscanf(token, "%d", number_choice+game_steps); 
      // printf("got step: %d, new status: %d\n", number_choice[game_steps], status);
      ++game_steps;
   }

   // read in each gameboard, play one by one, store which one is best 
   int best_board_steps = game_steps;
   int best_board_score = -1;

    for(;;){ // for each game board 
      int board[BOARD_SIZE*BOARD_SIZE];
      int board_steps;
      int board_score;

      // skip the blank line
      char line_blank[2];
      fgets_status = fgets(line_blank, sizeof(line_blank), fd);
      if (fgets_status == NULL){
         break;
      }

      // find the board
      for (size_t i=0; i<BOARD_SIZE; ++i){ 
         // pull one line of characters from the board
         char line_board[32];
         fgets_status = fgets(line_board, sizeof(line_board), fd);
         assert(fgets_status != NULL);
         
         // now extract numbers from that line
         char* line_board_working = line_board;
         char* token;
         size_t j=0;
         while ((token = strtok_r(line_board_working, " ", &line_board_working))){
            status = sscanf(token, "%d", board+5*i+j);
            assert(status == 1);
            ++j;
         }
      }

      // play the game
      // printf("got board:\n");
      // print_board(board);
      board_steps = play_game(number_choice, board, &board_score);      

      // record the best found score
      if (board_steps < best_board_steps){
         // printf("new winner!\n");
         best_board_steps = board_steps;
         best_board_score = board_score;
      }
 
   }

   fclose(fd);

   printf("%d\n", best_board_score);

   return 0;
}
