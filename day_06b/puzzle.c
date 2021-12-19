#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define FISH_TIMER_MAX 10
#define GENERATIONS 256

uint64_t fish_sum(uint64_t* population){
   uint64_t sum=0;
   for(size_t i=0; i<FISH_TIMER_MAX; ++i){
      sum += population[i];
   }
   return sum;
}

void fish_timer_decrease(uint64_t* population){
   for(size_t i=1; i<FISH_TIMER_MAX; ++i){
      population[i-1] = population[i];
   }
}

void fish_reproduce(uint64_t* population){
   population[FISH_TIMER_MAX-1] = population[0]; // newborn fish
   population[7] += population[0]; // reset reproduction timer
   population[0] = 0;
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
   char line_raw[1024];
   char* fgets_status;
   fgets_status = fgets(line_raw, sizeof(line_raw), fd);
   assert(fgets_status != NULL);

   // extract initial population from that first line
   uint64_t population[FISH_TIMER_MAX];
   memset(population, 0, FISH_TIMER_MAX*sizeof(*population));

   char* line_working = line_raw;
   char* token = strtok_r(line_working, ",", &line_working);
   while (token != NULL){
      // store the value
      int num;
      sscanf_status = sscanf(token, "%d", &num);
      assert(sscanf_status == 1);
      assert(num < FISH_TIMER_MAX);

      population[num+1] += 1;
      
      token = strtok_r(line_working, ",", &line_working);
   }
   fclose(fd);

   // now simulate!
   // for generations...
   for (size_t generation=0; generation<GENERATIONS; ++generation){

      // decrease all timers
      fish_timer_decrease(population);

      // if zero turn to 6 AND add a new 8
      fish_reproduce(population);
   }

   printf("%ld\n", fish_sum(population));

   return 0;
}
