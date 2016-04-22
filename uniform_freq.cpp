//! \file uniform_freq.cpp - main() function
#include <iostream>
#include "database.h"
#include "uniform_subgraph_random_walk.h"
#include "vectorUtility.h"
#include "Priority_Queue.h"

using namespace std;

char* datafile;
int subgraph_size;
int uniq_pat_count;
int max_iter;
int top_k = 3;

typedef ExPattern<int, int> PAT;
typedef Uniform_SubGraph_Random_Walk<PAT> RANDOM_WALK;
typedef map<int,RANDOM_WALK*> RDW_MAP; //random walk manager of graph_id
typedef typename RDW_MAP::iterator RDW_MAPIT; //!< Iterator
typedef typename RDW_MAP::const_iterator RDW_MAPCIT; //!< Constant Iterator

template<> vector<int> Database<PAT>::_no_data = Database<PAT>::set_static_data();
template<> PatternFactory<PAT>* PatternFactory<PAT>::_instance = PatternFactory<PAT>::set_static_data();
//template<> PatternFactory<PAT>::set_static_data();

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
    else if(strcmp(argv[i],"-k") == 0){
      top_k=atoi(argv[++i]);
    }
    else{
      print_usage(argv[0]);
    }
  }
}//end parse_args()

int selectGraphForSampling(Database<PAT>* database){
  //select a graph uniformly
  int graph_id = database->get_random_graph_id();

//  //get random graph
//  if(cur_iter <= top_k)
//  {
//      graph_id = database ->get_random_GraphID_by_FirstRow(); // just get randomly 1 graph from the first row.
//  }
//  else
//  {
//      graph_id = database ->get_random_GraphID(); // get randomly 1 graph from all rows.
//  }
  return graph_id;
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
    database->build_ordered_edges_list();
  }
  catch (exception& e) {

    cout << e.what() << endl;
  }
	database->print_database();
	database->print_ordered_edges_list();
  int cur_iter=0;
  ///TODO:
  //QUEUE* queue;
  cout<<"Begin sampling \n";
  RDW_MAP rdw_map;

  PAT* g;
  //max_iter = 2;

  Priority_Queue Q(top_k*2);

  while (cur_iter<=max_iter){
      cur_iter++;
      int graph_id;

      graph_id = selectGraphForSampling(database);
      cout<< "selected graph: \n"<< graph_id<<endl;
      g = database->get_graph_by_id(graph_id);
      cout << *g;

      RDW_MAPIT it = rdw_map.find(graph_id);
      RANDOM_WALK* rdw;
      if (it == rdw_map.end()){
        //the graph_id is sampled in the first time
        rdw = new RANDOM_WALK(database,graph_id,subgraph_size);
        rdw_map.insert(make_pair(graph_id,rdw));
      }else{
        rdw = it->second;
      }

      double h_score ;
      PAT* h = rdw->sampling_subgraph_by_Edge_Graph(h_score); // return a subgraph and it's score
      if (h!=NULL){
        cout<<"Sampled subgraph: score = "<<h_score<<endl;
        cout<<*h;
      }

      if (Q.isFull() && h_score<Q.getHalfAvgScore())
        continue;

      Queue_Item* qItem = Q.findByGraph(h);
      if (qItem!=NULL)
      {

        int preSupport = qItem->idset.size();
        int idx = find_in_vector(qItem->idset,graph_id);
        if (idx==-1){
          qItem->idset.push_back(graph_id);
          qItem->insert_time = cur_iter;
        }
      }
      else
      {
        if (Q.isFull())
          Q.evictLast();

        qItem = new Queue_Item();
        qItem->subgraph = h;
        qItem->idset.push_back(graph_id);
        qItem->insert_time = cur_iter;

        qItem->score = h_score;
        Q.push(qItem);
      }
  }
  cout<<"==============================="<<endl;
  cout<<"SAMPLING RESULT"<<endl;
  Q.print();
  ///TODO: delete random_walks, delete Queue
  delete database;
  cout << "FINISHED, PRESS ENTER TO EXIST!"<<endl;
  getchar();
}
