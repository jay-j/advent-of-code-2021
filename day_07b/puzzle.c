#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define CRABS_MAX 2000

int n_crabs;

void print_crabs(int* list){
   for (size_t i=0; i<n_crabs; i++){
      printf("%d ", list[i]);
   }
   printf("\n");
}


int mean(int* list){
   int sum = 0;
   for (size_t i=0; i<n_crabs; i++){
      sum += list[i];
   }
   printf("mean=%g\n", (double)sum / (double) n_crabs);
   sum = (int) round((double)sum / (double) n_crabs);
   return sum;
}


inline int fast_abs(int x){
   return x*(-1 + 2*(x >= 0));
}


int fuel(int* list, int target){
   int sum = 0;

   for (size_t i=0; i<n_crabs; i++){
      int distance = fast_abs(list[i] - target);
      while (distance > 0){
         sum += distance;
         --distance;
      }
   }
   return sum;
}


int main(int argc, char *argv[]){

   char filename[64];
   sscanf(argv[1], "%s", filename);

   // read in the file
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);
   int sscanf_status;

   // read the first line, figure out how many characters to use
   char line_raw[4000];
   char* fgets_status;
   fgets_status = fgets(line_raw, sizeof(line_raw), fd);
   assert(fgets_status != NULL);

   // extract initial population from that first line
   int crab_positions[CRABS_MAX];
   memset(crab_positions, 0, CRABS_MAX*sizeof(*crab_positions));

   char* line_working = line_raw;
   char* token = strtok_r(line_working, ",", &line_working);
   while (token != NULL){
      // store the value
      int num;
      sscanf_status = sscanf(token, "%d", &num);
      assert(sscanf_status == 1);
      crab_positions[n_crabs] = num;
      ++n_crabs;
      
      token = strtok_r(line_working, ",", &line_working);
   }
   fclose(fd);

   printf("n_crabs=%d\n", n_crabs);

   int best_goal = -1;
   int best_fuel = INT32_MAX;

   for (int i=-1; i<2; ++i){
      int goal_i = mean(crab_positions) + i;
      int fuel_i = fuel(crab_positions, goal_i);

      if (fuel_i < best_fuel){
         best_fuel = fuel_i;
         best_goal = goal_i;
      }

   }

   printf("goal=%d\n", best_goal);
   printf("%d\n", best_fuel );

   return 0;
}

// 93397632