#include <stdio.h>
#include <assert.h>
#include <string.h>

#define DATA_MAX 1024

// number of bits
int n_bits;
size_t n_data = 0;

#define SEARCH_OXYGEN 1
#define SEARCH_CARBON 0

int search(int* data, int search_type){
   int data_active[DATA_MAX];
   for (size_t i=0; i<n_data; ++i){
      data_active[i] = 1;
   }
   // memset(data_active, 1, sizeof(data_active));
   int result = -1;

   for (int bit=n_bits-1; bit >= 0; --bit){
      // loop through all, IF valid count ones and zeros
      int ones = 0;
      int zeros = 0;

      for (size_t i=0; i<n_data; ++i){
         if (data_active[i] == 1){
            int val = (data[i] & (0x1 << bit)) > 0;
            ones += val; 
            zeros += (1-val);
         }
      }

      int value_to_keep = 0;
      if (search_type == SEARCH_OXYGEN){
         if (ones >= zeros){
            value_to_keep = 1;
         }
      }
      else if (search_type == SEARCH_CARBON){
         if (ones < zeros){
            value_to_keep = 1;
         }
      }

      // throw out stuff that doesn't have value to keep!
      for (size_t i=0; i<n_data; ++i){
         if (data_active[i] == 1){
           int val = (data[i] & (0x1 << bit)) > 0; // look for 1
           data_active[i] = (val == value_to_keep); 
         }
      }
      

      // how many options are left?
      int remaining = 0;
      for (size_t i=0; i<n_data; ++i){
         remaining += data_active[i];
         if (data_active[i]){
            result = data[i];
         }
      }
      assert(remaining != 0);
      if (remaining == 1){
         break;
      }

   }

   return result;
}

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
   n_bits = 0;
   char line1[16];
   status = fscanf(fd, "%s\n", line1); 
   while(line1[n_bits] != '\0'){
      ++n_bits;
   }
   rewind(fd);

   // read a line
   int oxygen_rating = 0;
   int carbon_rating = 0;
   int data[DATA_MAX];
   memset(data, 0, sizeof(data));

   char value;
   n_data = 0;

    for(;;){ // for each line in file

      for (size_t i=0; i<n_bits+1; ++i){ // for each bit

         status = fscanf(fd, "%c", &value);
         if (status == EOF){
            break;
         }

         if (value == '0'){
            data[n_data] = data[n_data] << 1;
         }
         if (value == '1'){
            data[n_data] = data[n_data] << 1;
            data[n_data] |= 1;
         }

      }
      if (status == EOF){
         break;
      }
 
      ++n_data;
   }

   fclose(fd);

   oxygen_rating = search(data, SEARCH_OXYGEN);
   // printf("oxygen_rating=%d\n", oxygen_rating);

   carbon_rating = search(data, SEARCH_CARBON);
   // printf("carbon_rating=%d\n", carbon_rating);

   printf("%d\n", oxygen_rating*carbon_rating);

   return 0;
}
