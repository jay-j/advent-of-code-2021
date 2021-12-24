#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define LINE_LENGTH_MAX 128

#define NODE_MAX 128
#define NODE_LINKS_MAX 16
#define NODE_TYPE_BIG 1
#define NODE_TYPE_SMALL 0
#define PATH_LENGTH_MAX 128

typedef struct node{
   int n_links;    // quantity of linked nodes in links
   int links[NODE_LINKS_MAX];     // list of IDs of other nodes that this one is connected to
   int type;       // large or small
   char name[8];   // string label for the node to help parsing on creation
} node;
int node_qty;

typedef struct Path{
   int length;
   size_t node_id[PATH_LENGTH_MAX];
} Path;


void print_connections(node* n){
   printf("Node %s  has links: ", n->name);
   for (size_t i=0; i<n->n_links; ++i){
      printf("%d ", n->links[i]);
   }
   printf("\n");
}
      

void print_path(node* network, Path* path){
   printf("Path: ");
   for(size_t i=0; i<path->length; ++i){
      printf("%s ", network[path->node_id[i]].name);
   }
   printf("\n");
}

void cleanup(node* network){
   // free arrays stored inside the network
   // TODO ... if I decide to malloc these ?

   // free the network itself
   free(network);
}

int find_node_by_name(node* network, char* name){
   int result = -1;
   for (size_t i=0; i<node_qty; ++i){
      if (strcmp(name, network[i].name) == 0){
         result = i;
         break;
      }
   }
   return result;
}


int name_to_type(char* name){
   // big if upper case, small if lower case
   int value = (int) name[0];

   int result;
   if (value < 96){
      result = NODE_TYPE_BIG;
   }
   else{
      result = NODE_TYPE_SMALL;
   }
   return result;
}

// determine if node CHECK is on PATH
int node_already_used(node* network, Path* path, node* check){
   int result = 0;
   for(size_t i=0; i<path->length; ++i){
      result |= (check-network == path->node_id[i]);
   }
   return result;
}


void advance_paths(node* network, node* end, node* current, Path* path, int* count){
   // for each node connected to this node
   for(size_t i=0; i<current->n_links; ++i){
      node* candidate = network+current->links[i];
      
      // if end is found, then increment count
      if (candidate == end){
         print_path(network, path);
         *count += 1;
         continue;
      }

      // if node is not on path already OR is a big node
      if (( node_already_used(network, path, candidate) == 0) | (candidate->type == NODE_TYPE_BIG)){

         // create a new path
         Path path_new;
         path_new.length = path->length+1;
         for(size_t j=0; j<path->length; ++j){
            path_new.node_id[j] = path->node_id[j];
         }
         path_new.node_id[path_new.length-1] = (candidate - network);

         // spawn a path from that node onwards
         advance_paths(network, end, candidate, &path_new, count);

      }

   }

}

int main(int argc, char *argv[]){
   node* network;
   network = (node*) malloc(NODE_MAX * sizeof(node));
   node_qty = 0;
   node* start;
   node* end;

   // read in the file
   char filename[64];
   sscanf(argv[1], "%s", filename);
   FILE* fd;
   fd = fopen(filename, "r");
   assert(fd != NULL);
    
   // read the first line
   char line_raw[LINE_LENGTH_MAX];
   char* fgets_status;
   fgets_status = fgets(line_raw, sizeof(line_raw), fd);
   while (fgets_status != NULL){

      // split into a first and second name
      char* line_working = line_raw;
      char* name1 = strtok_r(line_working, "-", &line_working);
      char* name2 = strtok_r(line_working, "\n", &line_working); // \n here removes it from name2
      printf("description connects %s to %s\n", name1, name2);

      // check if either is already a node in the network
      int new1 = find_node_by_name(network, name1);
      int new2 = find_node_by_name(network, name2);

      if (new1 < 0){
         // need to create it!
         new1 = node_qty;
         network[node_qty].n_links = 0;
         network[node_qty].type = name_to_type(name1);
         strcpy(network[node_qty].name, name1);
         ++node_qty;
      }
      if (new2 < 0){
         // need to create it!
         new2 = node_qty;
         network[node_qty].n_links = 0;
         network[node_qty].type = name_to_type(name2);
         strcpy(network[node_qty].name, name2);
         ++node_qty;
      }

      // then make connections, increase node lists as needed
      network[new1].links[network[new1].n_links] = new2;
      network[new1].n_links += 1;

      network[new2].links[ network[new2].n_links ] = new1;
      network[new2].n_links += 1;

      // read in the next line
      fgets_status = fgets(line_raw, sizeof(line_raw), fd);
   }

   fclose(fd);

   for (size_t i=0; i<node_qty; ++i){
      print_connections(network+i);
   }

   // search for the start and end nodes
   for(size_t i=0; i<node_qty; ++i){
      if(strcmp(network[i].name, "start") == 0){
         start = network+i;
         printf("Start node is %ld\n", i);
      }
      if(strcmp(network[i].name, "end") == 0){
         end = network+i;
         printf("End node is %ld\n", i);
      }
   }

   // now search through the tree for stuff... recursive search for the end
   // need to track.. the path that has been taken!
   // don't track every path.. but recursively track the path taken by THIS search. 
   // depth first
   int count = 0;
   Path path;
   path.length = 1;
   path.node_id[0] = (start - network);
   advance_paths(network, end, start, &path, &count);
    
   
   printf("%d\n", count);

   cleanup(network);
   return 0;
}
