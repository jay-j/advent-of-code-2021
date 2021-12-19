#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define MAP_WIDTH_MAX 128

int map_cols;
int map_rows;
int map_area;


void print_map(int* map){
   for (size_t i=0; i<map_rows; ++i){
      for (size_t j=0; j<map_cols; ++j){
         printf("%d", map[i*map_cols + j]);
      }
      printf("\n");
   }
}

inline int fast_abs(int x){
   return x*(-1 + 2*(x >= 0));
}


int is_local_min(int* map, size_t i){
   size_t adjacency[] = {1, // right
                  map_cols, // bottom
                  -1, // left
                  -map_cols}; // up

   int result = 1; // the lowest!

   // consider each direction
   for (size_t dir=0; dir<4; ++dir){
      if (i+adjacency[dir] >= map_area){
         continue;
      }
      if (i+adjacency[dir] < 0){
         continue;
      }

      result &= ( map[i] < map[i+adjacency[dir]]);
   }
   return result;
}


int main(int argc, char *argv[]){
   int* map;
   map = (int*) malloc(MAP_WIDTH_MAX * MAP_WIDTH_MAX * sizeof(*map));   

   char filename[64];
   sscanf(argv[1], "%s", filename);

   // read in the file
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);
   // int sscanf_status;

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

   // sweep through map, evaluating each to see if it is the least
   int risk_sum = 0;
   for (size_t i=0; i<map_area; i++){ // for each cell

      // check all adjacent cells.. is this one a local minima?
      int is_min = is_local_min(map, i);

      // if so compute risk and add it
      if (is_min == 1){
         risk_sum += 1 + map[i];
         // printf("found min! map[%ld]=%d\n", i, map[i]);
      }
   }

   printf("%d\n", risk_sum);

   free(map);
   return 0;
}
