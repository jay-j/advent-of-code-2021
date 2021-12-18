#include <stdio.h>
#include <assert.h>
#include <string.h>

// filename to read is input argument on command line
int main(int argc, char *argv[]){

   char filename[64];
   sscanf(argv[1], "%s", filename);

   // read in the file
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);
   int status;

   // read the first line, figure out how many characters to use
   char line1[16];
   status = fscanf(fd, "%s\n", line1); 
   int N = 0;
   while(line1[N] != '\0'){
      ++N;
   }
   rewind(fd);

   // read a line
   int gamma_rate = 0;
   int epsilon_rate = 0;
   char value;
   int ones[N];
   int zeros[N];
   for (size_t i=0; i<N; ++i){
      ones[i] = 0;
      zeros[i] = 0;
   }

    for(;;){ // for each line in file

      for (size_t i=0; i<N+1; ++i){ // for each bit

         status = fscanf(fd, "%c", &value);
         if (status == EOF){
            break;
         }

         if (value == '0'){
            zeros[i] += 1;
         }
         if (value == '1'){
            ones[i] += 1;
         }

      }
      if (status == EOF){
         break;
      }
 
   }

   fclose(fd);

   // gamma: most common bit
   // epsilon: least common bit
   for (size_t i=0; i<N; ++i){
      int msb = (ones[i] > zeros[i]);
      gamma_rate = gamma_rate << 1;
      gamma_rate |= msb;

      int lsb = (ones[i] < zeros[i]);
      epsilon_rate = epsilon_rate << 1;
      epsilon_rate |= lsb;
   }
 //  printf("gamma rate: %d\n", gamma_rate);
//   printf("epsilon rate: %d\n", epsilon_rate);


   printf("%d\n", gamma_rate*epsilon_rate);

   return 0;
}
