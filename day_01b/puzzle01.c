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
   int num[4];
   int status;
   // fill up without analysis
   for (size_t i=0; i<3; i++){
      status = fscanf(fd, "%d\n", num+i);
   }

   for(;;){
      // fill in the last number
      status = fscanf(fd, "%d\n", num+3);
      if (status == EOF){
         break;
      }

      // compute both sums
      int sum_old = num[0]+num[1]+num[2];
      int sum_new = num[1]+num[2]+num[3];
      count += (sum_new > sum_old);

      // rotate the array
      num[0] = num[1];
      num[1] = num[2];
      num[2] = num[3];
    }

   fclose(fd);

   printf("%d\n", count);

   return 0;
}
