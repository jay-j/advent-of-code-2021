#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define LINE_MAX 256


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

      // parse line
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
               printf("ILLEGAL %c on line %s\n", *line_current, line);
               syntax_error_score += score(*line_current);
               break;
            }
         }

         ++line_current;
      } while ((*line_current != '\n') & (*line_current != '\0'));

   
   } while(fgets_status != NULL);


   fclose(fd);

   printf("%d\n", syntax_error_score);

   free(chunks);
   free(line);

   return 0;
}
