#include <stdio.h>
#include <assert.h>


// filename to read is input argument on command line
int main(int argc, char *argv[]){
   int count = 0;

   char filename[64];
   sscanf(argv[1], "%s", filename);

   //printf("got filename %s\n", filename);

   // read in the file
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);

   // read a line
   int num;
   int status;
   int previous_num = -1;
   for(;;){
      status = fscanf(fd, "%d\n", &num);
      if (status == EOF){
         break;
      }

      count += (num > previous_num);
      previous_num = num;
    }
   --count;

   fclose(fd);

   printf("%d\n", count);

   return 0;
}
