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

size_t quicksort_partition(int* list, size_t start, size_t end){
   // pivot value, from the middle of the array
   int pivot_index = (int) floor((start + end)/2 );
   int pivot = list[pivot_index ];

   // left index
   size_t i = start - 1;

   // right index
   size_t j = end + 1;

   for(;;){
      // move the left index to the right (at least once) 
      // and while element at the left is less than the pivot
      do {
         ++i;
      } while(list[i] < pivot);

      // move the right index to the left at least once 
      // and while element at the right index is greater than the pivot
      do {
         --j;
      } while (list[j] > pivot);

      // if indices crossed, give up!
      if (i >= j){
         return j;
      }

      // swap
      int tmp = list[i];
      list[i] = list[j];
      list[j] = tmp;
   }

}

// thanks to wikipedia ! 
void quicksort(int* list, size_t start, size_t end){
   if ((start >= 0) & (end >= 0) & (start < end)){

      // partition and get pivot index
      size_t pivot_loc = quicksort_partition(list, start, end);

      // recursive break into subproblems
      quicksort(list, 0, pivot_loc);
      quicksort(list, pivot_loc+1, end);
   }
  
   return;
}


int median(int* list){
   printf("the UNsorted list!\n");
   print_crabs(list);

   // sort list
   quicksort(list, 0, n_crabs-1);

   printf("the sorted list!\n");
   print_crabs(list);

   // seek the middle
   int result = 0;

   // n_crabs=10.. indexes 0 thru 9.. 
   if (n_crabs % 2 == 0){
      // even list, have to split the difference to get median
      result += list[n_crabs/2];
      result += list[n_crabs/2 - 1];
      result /= 2;
   }
   else{
      // odd numbered list, can just pick the middle-est one
      result = list[ (int) floor(n_crabs/2) ];
   }

   return result;
}

int mean(int* list){
   int sum = 0;
   for (size_t i=0; i<n_crabs; ++i){
      sum += list[i];
   }
   sum /= n_crabs;
   printf("mean= %d\n", (int) sum);
   return (int) sum;
}

int fuel(int* list, int median){
   int sum = 0;

   for (size_t i=0; i<n_crabs; i++){
      int tmp = list[i] - median;
      if (tmp < 0){
         tmp *= -1;
      }
      sum += tmp;
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

   //int goal = median(crab_positions);
   int goal = mean(crab_positions);

   printf("%d\n", fuel(crab_positions, goal));

   return 0;
}
