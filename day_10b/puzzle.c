#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define LINE_MAX 256


size_t quicksort_partition(uint64_t* list, size_t start, size_t end){
   // pivot value, from the middle of the array
   int pivot_index = (int) floor((start + end)/2 );
   uint64_t pivot = list[pivot_index ];

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
      uint64_t tmp = list[i];
      list[i] = list[j];
      list[j] = tmp;
   }

}

// thanks to wikipedia ! Hoare partition
void quicksort(uint64_t* list, size_t start, size_t end){
   if ((start >= 0) & (end >= 0) & (start < end)){

      // partition and get pivot index
      size_t pivot_loc = quicksort_partition(list, start, end);

      // recursive break into subproblems
      quicksort(list, start, pivot_loc);
      quicksort(list, pivot_loc+1, end);
   }
  
   return;
}


int open_chunk(char c){
   if (c == '['){
      return 1;
   }
   if (c == '{'){
      return 2;
   }
   if (c == '('){
      return 3;
   }
   if (c == '<'){
      return 4;
   }
   return 0;
}

int close_chunk(char c){
   if (c == ']'){
      return 1;
   }
   if (c == '}'){
      return 2;
   }
   if (c == ')'){
      return 3;
   }
   if (c == '>'){
      return 4;
   }
   return 0;
}

char close_to_open(char c){
   char result;
   switch (c){
      case ']':
         result = '[';
         break;
      case '}':
         result = '{';
         break;
      case ')':
         result = '(';
         break;
      case '>':
         result = '<';
         break;
      default:
         assert(0);         
   }
   return result;
}

int score(char c){
   int result;
   switch (c){
      case ']':
         result = 57;
         break;
      case '}':
         result = 1197;
         break;
      case ')':
         result = 3;
         break;
      case '>':
         result = 25137;
         break;
      default:
         assert(0);         
   }
   return result;
}

int autocorrect_value(char c){
   int result;
   switch (c){
      case '[':
         result = 2;
         break;
      case '{':
         result = 3;
         break;
      case '(':
         result = 1;
         break;
      case '<':
         result = 4;
         break;
      default:
         assert(0);         
   }
   return result;
}

int main(int argc, char *argv[]){
   char* chunks;
   chunks = (char*) malloc(LINE_MAX * sizeof(*chunks));

   char* line;
   line = (char*) malloc(LINE_MAX * sizeof(*line));


   // read in the file
   char filename[64];
   sscanf(argv[1], "%s", filename);
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);

   int syntax_error_score = 0;
   uint64_t autocorrect_scores[100];
   int autocorrect_n = 0;

   char* fgets_status;

   do {
      // read next line line
      fgets_status = fgets(line, LINE_MAX, fd);
      if (fgets_status == NULL){
         break;
      }

      // reset 
      char* chunk_current = chunks;
      char* line_current = line;
      int line_syntax_error = 0;

      // parse line, searching for syntax fails
      do{ 
         if (open_chunk(*line_current) > 0){
            // advance a chunk
            *chunk_current = *line_current;
            ++chunk_current;
         }
         if (close_chunk(*line_current) > 0 ){
            // backtrack
            --chunk_current;
            if ((*chunk_current != close_to_open(*line_current)) | (chunk_current < chunks)){
               // printf("ILLEGAL %c on line %s\n", *line_current, line);
               syntax_error_score += score(*line_current);
               line_syntax_error = 1;
               break;
            }
         }

         ++line_current;
      } while ((*line_current != '\n') & (*line_current != '\0'));

      // don't try to complete the line if it has syntax errors
      if (line_syntax_error > 0){
         continue;
      }

      // now complete the line!
      printf("incomplete line: %s\n", line);
      
      autocorrect_scores[autocorrect_n] = 0;
      while (chunk_current > chunks){
         autocorrect_scores[autocorrect_n] *= 5;
         
         // detect the missing character
         --chunk_current;
         autocorrect_scores[autocorrect_n] += autocorrect_value(*chunk_current);

      }
      printf("  autocorrect score: %ld\n", autocorrect_scores[autocorrect_n]);

      ++autocorrect_n;
   
   } while(fgets_status != NULL);

   // now run the sorting algorithm and get the scores
   quicksort(autocorrect_scores, 0, autocorrect_n-1);
   for (size_t i=0; i<autocorrect_n; ++i){
      printf("%ld ", autocorrect_scores[i]);
   }
   printf("\n");

   // pick out the middle score
   printf("autocorrect_n = %d\n", autocorrect_n);
   int choice = (int) floor(autocorrect_n / 2.0);


   fclose(fd);

   printf("%ld\n", autocorrect_scores[choice]);

   free(chunks);
   free(line);

   return 0;
}
