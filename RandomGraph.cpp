
#include "RandomGraph.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

typedef struct database_properties {
   int      total_number_graph;
   int      total_number_vertex;
   int      graph_min_size;
   int      graph_max_size;
   double   graph_max_density; // so_canh/sodinh
   int      random_seed;

   void get_file_name(string& fileName){
     fileName = "database_size"+SSTR(total_number_graph)
                          +"_v"+SSTR(total_number_vertex)
                          +"_vMin"+SSTR(graph_min_size)
                          +"_vMax"+SSTR(graph_max_size)
                          +"_seed"+SSTR(random_seed)
                          +".txt";
   }
} DatabaseProperties;

void random_vertex_label(int v[],int n,int maxLabel)
{
  int i=0;
  while (i<n)
  {
    v[i] = ran(maxLabel);
    bool ok = true;
    //check duplicate
    for (int j=0;j<i;j++)
      if (v[i]==v[j]){
        ok=false;
        break;
      }
    if (ok)
      i++;
  }
}
int main()
{
  char* out_file_name ;//= "output.txt";
  DatabaseProperties config;
  config.total_number_graph = 200;
  config.total_number_vertex = 4000;
  config.graph_min_size = 3200;
  config.graph_max_size = config.total_number_vertex;
  config.random_seed = 3571;


  string fileName;
  config.get_file_name(fileName) ;
  fileName = "dataset\\"+fileName;
  out_file_name = (char*) fileName.c_str();
  //generate file name
  //out_file_name = strcat("size",ita())

  if (config.graph_max_size>config.total_number_vertex)
  {
    printf("Max graph size cannot larger than total number of vertex");
    getchar();
    return 1;
  }
  std::ofstream ostr;
  ostr.open(out_file_name);
  if (!ostr.is_open()){
    cout<< "Unable to open file "<<out_file_name<<" for writing."<<endl;
    return 2;
  }
//  FILE *out_file;
//  if ( ( out_file = fopen( out_file_name, "w" ) ) == NULL ) {
//      printf( "Unable to open file %s for writing.\n", out_file_name );
//      return 2;
//   }
  printf( "\n\t", out_file_name );
   ///init random generator
  srand(config.random_seed);
  int graph_size_range = config.graph_max_size - config.graph_min_size;

  for (int i = 0; i <config.total_number_graph;i++)
  {
    //random graph property
    int vertex_count = ran(graph_size_range)+config.graph_min_size;
    //maximum number of edges =
    int max_edges_count = vertex_count*(vertex_count-1)/2;
    int min_edges_count = vertex_count-1;
    int edge_count = ran(max_edges_count-min_edges_count)+min_edges_count;
    if(edge_count < max_edges_count)
      edge_count +=1;

    int* vertex_labels;
    if ( ( vertex_labels = ( int * ) calloc( vertex_count, sizeof( int ) ) )
        == NULL ){
      printf( "Not enough room for this vertex labels\n" );
      return 3;
    }
    random_vertex_label(vertex_labels,vertex_count,config.total_number_vertex);
    random_connected_graph( vertex_count,  //vertex_count,
                              edge_count, //edge_count,
                              1,  //max_weight,
                              true,//weighted_p,
                              ostr, //menu -> outfiles.outfile1
                              i,
                              vertex_labels
                            );
    free(vertex_labels);
  }
   ostr.close();
   printf( "\tGraph is written to file %s.\n", out_file_name );
   return 0;
}
