//! \file uniform_freq.cpp - main() function
#include <iostream>
#include "database.h"
#include "uniform_subgraph_random_walk.h"
#include "vectorUtility.h"
#include "Priority_Queue.h"


//#define PRINT_DEBUG

using namespace std;

char* datafile;
char* out_file_name;
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
typedef lattice_node<PAT> LATTICE_NODE;

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

int selectGraphForSampling(Database<PAT>* database, PAT** g){
  //select a graph uniformly
  int graph_id =-1;
  do{
    graph_id = database->get_random_graph_id();
    *g = database->get_graph_by_id(graph_id);
  }while((*g)->size()<= subgraph_size);
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
  //parse_args(argc, argv); //-c 10 -s 4
  //datafile="dataset\\GRAPH_int_toy3.txt";

  datafile="dataset\\graphdata_20000712-20000712_total339.txt";
  out_file_name = "dataset\\graphdata_20000712-20000712_total339_20000_6.KFSAM.output";

  //datafile ="dataset\\database_size5_v5_vMin4_vMax5_seed3571.txt";
  //out_file_name= "dataset\\database_size5_v5_vMin4.KFSAM.output";

  subgraph_size= 6;
  max_iter=20000;
  top_k=100;

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
#ifdef PRINT_DEBUG
	#database->print_database();
	#database->print_ordered_edges_list();
#endif // PRINT_DEBUG

  int cur_iter=0;
  ///TODO:
  //QUEUE* queue;
  cout<<"Begin sampling \n";
  RDW_MAP rdw_map;

  PAT* g;
  //max_iter = 2;

  Priority_Queue Q(top_k*2);
  int similar=0;
  double maxTime =0;
  while (cur_iter<=max_iter){
      if(similar == top_k*8)
      {
        cout<<"stop";
        break;
        cur_iter==max_iter;
      }

      cur_iter++;
      cout<<"cur_iter = "<<cur_iter<<endl;

      int graph_id;

      graph_id = selectGraphForSampling(database,&g);
      // cout<< "selected graph: \n"<< graph_id<<endl;
      g = database->get_graph_by_id(graph_id);
      //cout << *g;

      RDW_MAPIT it = rdw_map.find(graph_id);
      RANDOM_WALK* rdw;
      if (it == rdw_map.end()){
        //the graph_id is sampled in the first time
        rdw = new RANDOM_WALK(database,graph_id,subgraph_size);
        rdw_map.insert(make_pair(graph_id,rdw));
      }else{
        rdw = it->second;
      }

      double freq ;

      //LATTICE_NODE* lNode = rdw->sampling_subgraph_by_Edge_Graph(freq); // return a subgraph and it's score
      LATTICE_NODE* lNode = rdw->sampling_subgraph(freq);
      PAT* h =lNode->_pat;

      /*if (h!=NULL){
        cout<<"Sampled subgraph: score = "<<freq<<endl;
        cout<<"Sampled subgraph: key = "<<lNode->get_key()<<endl;
        //cout<<*h;
      } */

      if (Q.isFull() && freq<=Q.getMinScore())
      {

        continue;
      }
      Queue_Item* qItem = Q.findByGraph(h);
      //if (qItem!=NULL)
      //{
      //  qItem->insert_time = cur_iter;
      //}
      //else
      //{
      similar +=1;
      if(qItem==NULL){
        if (Q.isFull())
          Q.evictLast();

        qItem = new Queue_Item();
        qItem->graphID = graph_id;
        qItem->subgraph = h;
        //qItem->idset.push_back(graph_id);
        for(int i=0;i<lNode->_support_set->size();i++)
          qItem->idset.push_back(lNode->_support_set->at(i));

        qItem->insert_time = cur_iter;
        qItem->score = freq;
        if(maxTime<cur_iter)
          maxTime=cur_iter;

        Q.push(qItem);
        similar = 0;
      }
      //}
  }
  cout<<"==============================="<<endl;
  cout<<"SAMPLING RESULT"<<endl;
  //Q.print();
  cout<<"last iter"<<cur_iter<<endl;
  cout<<"similar"<< similar<<endl;
  cout<<"maxTime"<<maxTime<<endl;
  Q.print_to_file(out_file_name);
  Q.free();
  ///TODO: delete random_walks, delete Queue
  delete database;
  cout << "FINISHED, PRESS ENTER TO EXIST!"<<endl;
  getchar();
}
