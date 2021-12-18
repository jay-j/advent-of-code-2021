#include <stdio.h>
#include <assert.h>
#include <string.h>

// filename to read is input argument on command line
int main(int argc, char *argv[]){

   char filename[64];
   sscanf(argv[1], "%s", filename);

   //printf("got filename %s\n", filename);

   // read in the file
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);

   // read a line
   int position_horizontal = 0;
   int position_depth = 0;
   int aim = 0;

   char move_direction[16];
   int move_distance;
   int status;
   for(;;){
      status = fscanf(fd, "%s %d\n", move_direction, &move_distance);
      if (status == EOF){
         break;
      }

      if (strcmp(move_direction, "forward") == 0){
         position_horizontal += move_distance;
         position_depth += aim*move_distance;
      }
      if (strcmp(move_direction, "down") == 0){
         aim += move_distance;
      }
      if (strcmp(move_direction, "up") == 0){
         aim -= move_distance;
      }

   }

   fclose(fd);

   printf("%d\n", position_horizontal*position_depth);

   return 0;
}
