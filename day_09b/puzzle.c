#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAP_WIDTH_MAX 128
#define MODE_EXPLORE 0
#define MODE_MARK 1

int map_cols;
int map_rows;
int map_area;

typedef struct {
   int index;
   int size;
   int label;
} basin;


void print_map(int* map){
   for (size_t i=0; i<map_rows; ++i){
      for (size_t j=0; j<map_cols; ++j){
         printf("%2d ", map[i*map_cols + j]);
      }
      printf("\n");
   }
}


void print_basinlist(basin* basins, int n){
   for (size_t j=0; j<n; j++){
      printf(" basin[%ld]  index=%2d  size=%2d\n", j, basins[j].index, basins[j].size);
   }
}

size_t quicksort_partition(basin* list, size_t start, size_t end){
   // pivot value, from the middle of the array
   int pivot_index = (int) floor((start + end)/2 );
   int pivot = list[pivot_index ].size;

   // left index
   size_t i = start - 1;

   // right index
   size_t j = end + 1;

   for(;;){
      // move the left index to the right (at least once) 
      // and while element at the left is less than the pivot
      do {
         ++i;
      } while(list[i].size < pivot);

      // move the right index to the left at least once 
      // and while element at the right index is greater than the pivot
      do {
         --j;
      } while (list[j].size > pivot);

      // if indices crossed, give up!
      if (i >= j){
         return j;
      }

      // swap
      basin tmp = list[i];
      list[i] = list[j];
      list[j] = tmp;
   }

}

// thanks to wikipedia ! Hoare partition
void quicksort(basin* list, size_t start, size_t end){

   if ((start >= 0) & (end >= 0) & (start < end)){

      // partition and get pivot index
      size_t pivot_loc = quicksort_partition(list, start, end);

      // recursive break into subproblems
      quicksort(list, start, pivot_loc);
      quicksort(list, pivot_loc+1, end);
   }
  
   return;
}


inline int fast_abs(int x){
   return x*(-1 + 2*(x >= 0));
}


int is_local_min(int* map, size_t* adjacency, size_t i){

   int result = 1; // the lowest!

   // consider each direction
   for (size_t dir=0; dir<4; ++dir){
      if (i+adjacency[dir] >= map_area){
         continue;
      }
      if (i-adjacency[dir] < 0){
         continue;
      }

      result &= ( map[i] < map[i+adjacency[dir]]);
   }
   return result;
}


int expand_basin(int* map, int* basin_labels, size_t* adjacency, size_t i, int mark){
   assert(i >= 0);
   assert(i < map_area);

   int size = 1;
   basin_labels[i] |= (0x1 << mark);
   // printf("basin labels:\n");
   // print_map(basin_labels);

    // consider each direction
   for (size_t dir=0; dir<4; ++dir){
      int cell = i+adjacency[dir];

      // throw out of grossly out of bounds
      if (cell >= map_area){
         continue;
      }
      if (cell < 0){
         continue;
      }

      // throw out wrapping
      int index_col = i % map_cols;
      int index_row = (i - index_col) / map_cols;
      if ((index_col == map_cols-1) & (dir == 0)){ // prevent wrap over right
         continue;
      }
      if ((index_col == 0) & (dir == 2)){ // prevent wrap over left
         continue;
      }
      if ((index_row == 0) & (dir == 3)){ // prevent wrap over top
         continue;
      }
      if ((index_row == map_rows-1) & (dir == 1)){ // prevent wrap over bottom
         continue;
      }

      // ignore if too tall; basin blocking
      if (map[cell] > 8){
         continue;
      }
      // has this exploration already visited this cell?
      if ((basin_labels[cell] & (0x1 << mark)) > 0){
         continue;
      }

      size += expand_basin(map, basin_labels, adjacency, cell, mark);
   }
   return size;
}

void zero_map(int* map){
   for (size_t i=0; i<MAP_WIDTH_MAX*MAP_WIDTH_MAX; ++i){
      map[i] = 0;
   }
}

int main(int argc, char *argv[]){
   int* map;
   map = (int*) malloc(MAP_WIDTH_MAX * MAP_WIDTH_MAX * sizeof(*map));

   int* basin_labels;
   basin_labels = (int*) malloc(MAP_WIDTH_MAX * MAP_WIDTH_MAX * sizeof(*basin_labels));
   zero_map(basin_labels);

   int* basin_explore;
   basin_explore = (int*) malloc(MAP_WIDTH_MAX * MAP_WIDTH_MAX * sizeof(*basin_explore));
   zero_map(basin_explore);

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

   size_t adjacency[] = {1, // right
                  map_cols, // bottom
                  -1, // left
                  -map_cols}; // up

   // only care about the three biggest
   basin basins[4];
   for (size_t i=0; i<4; ++i){
      basins[i].size = 0;
      basins[i].index = -1;
   }

   // sweep through map, evaluating each to see if it is the least
   // due to puzzle given, ASSUMES 1:1 correspondence between local minima and basins
   for (size_t i=0; i<map_area; i++){ // for each cell

      // check all adjacent cells.. is this one a local minima?
      int is_min = is_local_min(map, adjacency, i);

      // if so start and expand a basin
      if (is_min == 1){
         printf("new basin candidate\n");
         basins[0].index = i;

         // compute basin size
         zero_map(basin_explore);
         basins[0].size = expand_basin(map, basin_explore, adjacency, basins[0].index, 1);
         print_map(basin_explore);
         printf("  candidate basin size: %d\n", basins[0].size);

         // sort basins into the top 3
         quicksort(basins, 0, 3);

         // if the top three changed
         if (basins[0].index != i){
            printf("NEW TOP 3 BASINS!!!\n");

            // clear map, remark top 3 basins
            zero_map(basin_labels);

            for (size_t j=1; j<4; j++){
               int index = basins[j].index;
               if (index >= 0){
                  int tmp = expand_basin(map, basin_labels, adjacency, index, j+1);
                  assert(tmp > 0);
               }
            }
            printf("TOP 3 basins after some labeling\n");
            print_map(basin_labels);
         }
         
      }
   }

   int result = 1;
   for (size_t i=1; i<4; i++){
      result *= basins[i].size;
   }

   printf("%d\n", result);

   free(map);
   free(basin_labels);
   free(basin_explore);
   return 0;
}
