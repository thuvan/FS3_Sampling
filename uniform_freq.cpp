//! \file uniform_freq.cpp - main() function
#include <iostream>
#include "database.h"
#include "uniform_subgraph_random_walk.h"

using namespace std;

char* datafile;
int subgraph_size;
int uniq_pat_count;
int max_iter;

typedef ExPattern<int, int> PAT;
typedef Uniform_SubGraph_Random_Walk<PAT> RANDOM_WALK;
typedef map<int,RANDOM_WALK*> RDW_MAP; //random walk manager of graph_id
typedef typename RDW_MAP::iterator RDW_MAPIT; //!< Iterator
typedef typename RDW_MAP::const_iterator RDW_MAPCIT; //!< Constant Iterator

template<> vector<int> Database<PAT>::_no_data = Database<PAT>::set_static_data();
template<> PatternFactory<PAT>* PatternFactory<PAT>::_instance = PatternFactory<PAT>::set_static_data();
//template<> PatternFactory<PAT>::set_static_data();

class Queue_Item{
public:
  double score;
  vector<int> idset;
  long insert_time;
  PAT subgraph;
};


void print_usage(char *prog) {
  cerr<<"Usage: "<<prog<<" -d data-file -c count -s subgraph-size"<<endl;
  exit(0);
}
/*! Parsing arguments */
void parse_args(int argc, char* argv[]) {
  if(argc<7) {
    print_usage(argv[0]);
  }

  for (int i=1; i < argc; ++i){
    if (strcmp(argv[i], "-d") == 0){
      datafile=argv[++i];
    }
    else if (strcmp(argv[i], "-s") == 0){
      subgraph_size=atoi(argv[++i]);
      cout << "Size of Subgraph:" << subgraph_size << " " << endl;
    }
    else if(strcmp(argv[i],"-c") == 0){
      max_iter=atoi(argv[++i]);
    }
    else{
      print_usage(argv[0]);
    }
  }
}//end parse_args()

Queue_Item& sampling_subgraph(RDW_MAP& rdw_map,Database<PAT>* const database,int const graph_id,int const subgraph_size)
{
  //get random_walk_manager of graph;
  RDW_MAPIT it = rdw_map.find(graph_id);
  RANDOM_WALK* rdw;
  if (it == rdw_map.end()){
    //the graph_id is sampled in the first time
    rdw = new RANDOM_WALK(database,graph_id,subgraph_size);
    rdw_map.insert(make_pair(graph_id,rdw));

  }else{
    rdw = it->second;
  }
  lattice_node<PAT>* start = rdw->initialize();
  //lattice_node<PAT>* cur_sate = rdw->sample_subgraph(subgraph_size);
  lattice_node<PAT>* cur_sate = rdw->get_next(start);
  Queue_Item rs;

  return rs;
}

/*! main function */
int main(int argc, char *argv[]) {

	bool zero_neighbors;
  parse_args(argc, argv);
  Database<PAT>* database;
  /* creating database and loading data */
  try {
    database = new Database<PAT>(datafile);
    database->set_subgraph_size(subgraph_size);
    database->set_minsup(1);
  }
  catch (exception& e) {

    cout << e.what() << endl;
  }
	database->print_database();

  int cur_iter=0;
  ///TODO:
  //QUEUE* queue;
  cout<<"Begin sampling \n";
  RDW_MAP rdw_map;

  PAT* g;

  while (cur_iter<=max_iter){
    cur_iter++;
    cout<<"Begin sampling iter "<<cur_iter<<"\n";
    //select a graph uniformly
    int graph_id = database->get_random_graph_id();
    cout<< "random graph: \n"<< graph_id<<endl;
    g = database->get_graph_by_id(graph_id);
    cout << *g;

    const typename PAT::CAN_CODE& cc = check_isomorphism(g);
    g->set_canonical_code(cc);
    cout << g->get_canonical_code().to_string() << endl;



    //sampling subgraph of the selected graph using Random walk
    Queue_Item subgraph = sampling_subgraph(rdw_map,database,graph_id,subgraph_size);

//    double h_score = compute_score(database,sub_graph);
//    if (queue.is_full() && (score < queue.lower_Half_Avg_Score()))
//      continue;
//
//    CAN_CODE h_code = sub_graph->get_canonical_code();
//    if (queue.contain(h_code)){
//      prev_support = sub_graph->get_su
//    }

  }

  delete database;

  /* creating random_walk_manager and starting walk */
//  Uniform_Freq_Random_Walk<PAT> rwm(database, max_iter);
//
//	//will call initialize followed by walk again and again until it gets a single edge pattern with some neighbors.
//	do {
//    lattice_node<PAT>* start = rwm.initialize();
//    zero_neighbors = rwm.walk(start,cur_iter);
//  } while (zero_neighbors == 0);
//
//  delete database;

}
