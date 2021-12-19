#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define MAP_EDGE 1024
#define MAP_SIZE MAP_EDGE*MAP_EDGE

typedef struct{
   int x;
   int y;
} vec2;

vec2 vec2_add(vec2 p1, vec2 p2){
   vec2 r;
   r.x = p1.x + p2.x;
   r.y = p1.y + p2.y;
   return r;
}

vec2 point_sub(vec2 p1, vec2 p2){
   vec2 r;
   r.x = p1.x - p2.x;
   r.y = p1.y - p2.y;
   return r;
}

// TODO a less shitty implementation for integers..
vec2 vec2_unit(vec2 v){
   vec2 r;
   r.x = v.x;
   if (r.x > 1){
      r.x = 1;
   }
   if (r.x < -1){
      r.x = -1;
   }

   r.y = v.y;
   if (r.y > 1){
      r.y = 1;
   }
   if (r.y < -1){
      r.y = -1;
   }
  return r;
}

inline int vec2_equal(vec2 v, vec2 u){
   return (v.x == u.x) & (v.y == u.y);
}

void print_board(uint8_t* board){
   for(size_t i=0; i<MAP_EDGE; ++i){
      for(size_t j=0; j<MAP_EDGE; ++j){
         printf("%1d ", board[MAP_EDGE*i+j]);
      }
      printf("\n");
   }
}


int main(int argc, char *argv[]){
   uint8_t* map;
   map = (uint8_t*) malloc(MAP_SIZE * sizeof(*map));
   memset(map, 0, MAP_SIZE * sizeof(*map));

   char filename[64];
   sscanf(argv[1], "%s", filename);

   // read in the file
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);
   int sscanf_status;

   char* fgets_status;

   int hazards = 0;
    for(;;){ // for each readline listed 
       // get a readline
       char line_string[32];
       fgets_status = fgets(line_string, sizeof(line_string), fd);
       if(fgets_status == NULL){
          break;
       }
       //printf("need to parse %s\n", line_string);

       // now extract line coordinates from that string
       vec2 line[2]; 
       sscanf_status = sscanf(line_string, "%d,%d -> %d,%d\n", &line[0].x, &line[0].y, &line[1].x, &line[1].y); 
       assert(sscanf_status == 4);

       // turn the line into a useful description
       vec2 delta = point_sub(line[1], line[0]); 
       //printf("delta=%d, %d\n", delta.x, delta.y);
       vec2 uvec = vec2_unit(delta);
       //printf("direction=%d, %d\n", uvec.x, uvec.y);

       // now mark the line
       vec2 current = line[0];
       // mark the coordinates of current on the map
       int coordinate = current.y*MAP_EDGE + current.x;
       map[coordinate] += 1;
       if (map[coordinate] == 2){
          ++hazards;
      }
       do {
          // advance current per delta
          current = vec2_add(current, uvec);
         
          // mark the coordinates of current on the map
          int coordinate = current.y*MAP_EDGE + current.x;
          map[coordinate] += 1;
          if (map[coordinate] == 2){
             ++hazards;
         }
      } while (vec2_equal(current, line[1]) == 0);
  }

   //print_board(map);

   fclose(fd);
   free(map);

   printf("%d\n", hazards);

   return 0;
}

// correct answer :4655!
