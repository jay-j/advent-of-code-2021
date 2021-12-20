#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAP_WIDTH_MAX 128
#define STEPS 300

int map_cols;
int map_rows;
int map_area;

void print_map(int* map){
   for (size_t i=0; i<map_rows; ++i){
      for (size_t j=0; j<map_cols; ++j){
         printf("%2d ", map[i*map_cols + j]);
      }
      printf("\n");
   }
}


void increase_all(int* map){
   for (size_t i=0; i<map_area; ++i){
      map[i] += 1;
   }
}


inline int fast_abs(int x){
   return x*(-1 + 2*(x >= 0));
}


void zero_map(int* map){
   for (size_t i=0; i<MAP_WIDTH_MAX*MAP_WIDTH_MAX; ++i){
      map[i] = 0;
   }
}


void get_coordinates(int index, int* coords){
   coords[1] = index % map_cols;
   coords[0] = (index - coords[1]) / map_cols;
}


int get_adjacent(int index, int* cells){
   int coords_ref[2];
   get_coordinates(index, coords_ref);   
   int qty = 0;

   size_t adjacency[] = {1, // right
                  1+map_cols, // right-bottom
                  map_cols, // bottom
                  map_cols-1, // bottom-left
                  -1, // left
                  -map_cols-1, // left-up
                  -map_cols, // up
                  -map_cols+1}; // up-right

   for (size_t dir=0; dir<8; ++dir){
      int cell = index+adjacency[dir];

      int coords_new[2];
      get_coordinates(cell, coords_new);
      if (cell < 0){
         continue;
      }
      if (cell >= map_area){
         continue;
      }

      if ((fast_abs(coords_new[0] - coords_ref[0]) <= 1) & (fast_abs(coords_new[1] - coords_ref[1]) <= 1)){
         cells[qty] = cell;
         qty += 1;
      }
   }

   // printf("index %d is adjacent to:  ", index);
   // for (size_t i=0; i<qty; ++i){
   //    printf("%d ", cells[i]);
   // }
   // printf("\n");

   return qty;
}


int process_flash(int* map, int* already_flashed, int* count_flashes){
   int ready_to_flash = 0;

   for (size_t i=0; i<map_area; ++i){
      if ((map[i] > 9) & (already_flashed[i] == 0)){
         already_flashed[i] = 1;
         *count_flashes += 1;

         int adjacent_cells[8];
         int adjacent_qty = get_adjacent(i, adjacent_cells);

         // consider each direction to mark adjacent ones
         for (size_t dir=0; dir<adjacent_qty; ++dir){
            int cell = adjacent_cells[dir];
            
            map[cell] += 1;
            if (map[cell] > 9){
               ++ready_to_flash;
            }

         }
      }
   }
   return ready_to_flash;
}


int main(int argc, char *argv[]){
   int* map;
   map = (int*) malloc(MAP_WIDTH_MAX * MAP_WIDTH_MAX * sizeof(*map));

   int* already_flashed;
   already_flashed = (int*) malloc(MAP_WIDTH_MAX * MAP_WIDTH_MAX * sizeof(*already_flashed));
   zero_map(already_flashed);

   // read in the file
   char filename[64];
   sscanf(argv[1], "%s", filename);
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);

   // read the first line, figure out how many characters to use
   char line_raw[MAP_WIDTH_MAX];
   char* fgets_status;
   fgets_status = fgets(line_raw, sizeof(line_raw), fd);
   assert(fgets_status != NULL);

   // read first line. and figure out how big the map is going to be
   map_cols = 0;
   while (map_cols < MAP_WIDTH_MAX){
      if (line_raw[map_cols] == '\n'){
         break;
      }
      ++map_cols;
   }

   // fill the map
   map_rows = 0;
   for (;;){

      // extract data from this row
      for (size_t col=0; col<map_cols; ++col){
         char value = line_raw[col];
         map[map_rows*map_cols + col] = atoi(&value);
      }

      // read from file the next row
      fgets_status = fgets(line_raw, sizeof(line_raw), fd);

      ++map_rows;
      if (fgets_status == NULL){
         break;
      }
   }

   map_area = map_cols * map_rows;
   printf("detected map size: %d x %d\n", map_rows, map_cols);

   print_map(map);
   fclose(fd);

   int count_flashes = 0;
   int flashed_this_turn;
   int sync_step;
   for (size_t step=0; step<STEPS; ++step){
      flashed_this_turn = 0;
      zero_map(already_flashed);
      increase_all(map);

      int step_process_flash = 1;
      while( step_process_flash > 0){
         step_process_flash = process_flash(map, already_flashed, &count_flashes);
      }

      // reset those that flashed this turn
      for (size_t i=0; i<map_area; ++i){
         if (already_flashed[i] == 1){
            flashed_this_turn++;
            map[i] = 0;
         }
      }

      if (flashed_this_turn == map_area){
         sync_step = step+1;
         break;
      }

      // printf("step complete! map:\n");
      // print_map(map);
   }

   printf("%d\n", sync_step);

   free(map);
   free(already_flashed);
   return 0;
}
