//! \file random_walk_manager_freq.h - class to perform the random walk
#ifndef _RANDOM_SUBGRAPH_WALK_MANAGER2_H_
#define _RANDOM_SUBGRAPH_WALK_MANAGER2_H_

#include <algorithm>
#include <ext/hash_set>
#include <ext/hash_map>
#include "helper_funs.h"
#include "pattern_factory.h"
#include "lattice_node.h"
#include "random.h"
#include "time_tracker.h"
#include "functional"

/**
 * This performs random walk on frequent patterns to find uniform sample of frequent
 * patterns. It save the frequent patterns for fast processing
 */
//! A RandomWalkManager_Freq template class.
template<class PAT>
class Uniform_SubGraph_Random_Walk
{

  public:

  typedef lattice_node<PAT> LATTICE_NODE;
  typedef HASHNS::hash_map<string, LATTICE_NODE*, hash_func<string>, equal_to<string> > NODE_MAP;
  typedef typename NODE_MAP::iterator NS_IT;
  typedef typename NODE_MAP::const_iterator CONST_NS_IT;

  typedef HASHNS::hash_map<string, int, hash_func<string>, equal_to<string> > FREQ_CNT_MAP;
  typedef typename FREQ_CNT_MAP::iterator FC_IT;
  typedef Database<PAT> DATABASE;
  typedef PatternFactory<PAT> PATTERN_FACTORY;

	//! Constructor
  Uniform_SubGraph_Random_Walk(DATABASE* d,int graph_id, int subgraph_size) {
    _last = 0;
    _pf = PATTERN_FACTORY::instance(d);
    _database = d;
    _subgraph_size=subgraph_size;
    _graph_id = graph_id;
    _graph = _database->get_graph_by_id(_graph_id);
    cout << "Create a random walk manager for graph id:" << _graph_id << endl;
  }

	//! get PatternFactory object
  PatternFactory<PAT>* get_pat_factory() const {
    return _pf;
  }

  // random walk manager initialize with a frequent pattern node
	/*! \fn LATTICE_NODE* initialize()
 		*  \brief A member function to initialize the walk in itemset Lattice. Initialization completed
		*		by selecting an size one frequent pattern.
 		*  \return a pointer of LATTICE_NODE type.
 		*/
  LATTICE_NODE* initialize() {
    vector<int> vids;
    _pf->get_random_subgraph(_graph, _subgraph_size,vids);

    cout<<"vertex indexs: ";
    for(int i=0;i<vids.size();i++)
      cout<<vids[i]<<" ";

    _last_node = create_lattice_node(_graph,vids);
    process_node(_last_node);
    return _last_node;
  }

  PAT* sampling_subgraph(double& score)
  {
    LATTICE_NODE* current;
    if(!_isInitialized)
      current = initialize();
    else
      current = get_next(current);

    LATTICE_NODE* next=NULL;
//    while(next==NULL)
//    {
//
//    }
    //dx = neighbor_count of x;
    //a_subx = score of graph x;
    /*while (a neighbor y is not found)
        y = a random neighbor of x;
        dy = possible neighbor of y
    */
    return next->_pat;;
  }

	/*! \fn LATTICE_NODE* get_next(LATTICE_NODE* current) const
 		*  \brief A member function to get next node on itemset lattice to jump from current.
		*	 Acceptance probability calculation of Metropolis-Hastings
		* algorithm is implemented here.
		*	\param current a pointer of LATTICE_NODE.
		* \return a pointer of LATTICE_NODE.
	*/
  LATTICE_NODE* get_next(LATTICE_NODE* current) const {
    int total=current->_neighbor_prob.size();
#ifdef PRINT
   std::copy(current->_neighbor_prob.begin(), current->_neighbor_prob.end(), ostream_iterator<double>(cout," "));
    cout << endl;
#endif
    vector<double> prob(total+1);
    prob[0]=current->_neighbor_prob[0];
    for (int i=1; i<total; i++) {
      prob[i]=prob[i-1]+current->_neighbor_prob[i];
    }
    assert(prob[total-1]<=1.00001);
    prob[total]=1;
#ifdef PRINT
    std::copy(prob.begin(), prob.end(), ostream_iterator<double>(cout," "));
    cout << endl;
#endif
    int idx;
    do {
      idx = randomWithDiscreteProbability(prob);
    } while (idx == total);
//    cout << "returning with:" << idx << endl;
    return current->_neighbors[idx];
  }

	/*! \fn LATTICE_NODE* create_lattice_node(PAT*& p)
 		*  \brief A member function to create a new lattice node.
		*	 It first check whether the pattern p come as parameter is already a lattice node from its canonical code.
		*	 If not a new lattice node is created.
		*	\param p a reference of a pointer of PAT.
		* \return a pointer of LATTICE_NODE
	*/
//  LATTICE_NODE* create_lattice_node(PAT*& p) {
//    const typename PAT::CAN_CODE& cc = check_isomorphism(p);
//    p->set_canonical_code(cc);
//    std::string min_dfs_cc = cc.to_string();
//
//    LATTICE_NODE* node = exists(min_dfs_cc);
//    if (node == 0) {  // new pattern
//      node = new LATTICE_NODE(p);
//      node->_is_processed = false;
//      insert_lattice_node(min_dfs_cc, node);
//    }
//    else {
//      delete p;
//      p = node->_pat;
//    }
//    return node;
//  }

  LATTICE_NODE* create_lattice_node(PAT*& graph,vector<int>& vids) {
    //get subgraph from vids
    PAT* p = _pf->make_subgraph_from_vids(graph,vids);

    //get can_code of subgraph
    const typename PAT::CAN_CODE& cc = check_isomorphism(p);
    p->set_canonical_code(cc);
    std::string min_dfs_cc = cc.to_string();

    LATTICE_NODE* node = exists(min_dfs_cc);
    if (node == 0) {  // new pattern
      node = new LATTICE_NODE(p,vids);
      node->_is_processed = false;
      insert_lattice_node(min_dfs_cc, node);
    }
    else {
      delete p;
      p = node->_pat;
    }
    return node;
  }

	/*! \fn LATTICE_NODE* exists(string p)
 		*  \brief A member function to check exixtance of a lattice node.
		*	\param p a string.
		* \return a pointr of LATTICE_NODE.
	*/
  LATTICE_NODE* exists(string p) {;
    CONST_NS_IT it = _node_map.find(p);
    return (it != _node_map.end())? it->second : 0;
  }

	/*! \fn void insert_lattice_node(string p, LATTICE_NODE* ln)
 		*  \brief A member function to store newly created lattice node.
		*	\param p a string.
		* \param ln a pointer of LATTICE_NODE.
	*/
  void insert_lattice_node(string p, LATTICE_NODE* ln) {
    _node_map.insert(make_pair(p, ln));
  }

	/*! \fn void process_node(LATTICE_NODE* n)
 		*  \brief A member function to process a lattice node.
		* This function generates all frequent super and sub patterns of a processed node (n).
		* It also perform the degree calculation of n as well as of all minned super and sup patterns.
		*	\param n a LATTICE_NODE pointer
	*/
  void process_node(LATTICE_NODE* n) {
    if (n->_is_processed) return;
    PAT* p = n->_pat;

    vector<int> vids = n->_vids;
    vector<vector<int> > nbs_vids(vids.size());

    int dx = _pf->count_neighbor_subgraph(_graph,vids,nbs_vids);
    cout<<"neighbors count: "<<dx<<endl;
    for(int i=0;i<nbs_vids.size();i++)
    {
      cout<<i<<": ";
      for (int j=0;j<nbs_vids[i].size();j++)
        cout<<nbs_vids[i][j]<<" ";
      cout<<endl;
    }

    //assert(p->get_sup_ok() == 0);
//#ifdef PRINT
    cout << "Current pattern:\n";
    cout << *p;
//#endif
    vector<PAT*> neighbors;
    _pf->get_neighbors_subgraph(p,n->_vids, neighbors);
#ifdef PRINT
    cout << "Its neighbors:" << endl;
   cout << "Total neighbors="<< neighbors.size() << endl;
#endif
  //compute score of neighbors and push to neighbors list of pat
//   for (int i=0; i<neighbors.size(); i++) {
//      PAT* one_neighbor = neighbors[i];
//#ifdef PRINT
//      cout << *one_neighbor;
//#endif
//      int its_degree=_pf->get_super_degree(one_neighbor)+ _pf->get_sub_degree(one_neighbor);
//#ifdef PRINT
//      cout << "Its degree:" << its_degree << endl;
//#endif
//      double prob = 1.0 / (its_degree>neighbors.size()? its_degree : neighbors.size());
//      LATTICE_NODE* ln = create_lattice_node(one_neighbor);
//      int status;
//      n->_neighbors.push_back(ln);
//      n->_neighbor_prob.push_back(prob);
//
//      const typename PAT::CAN_CODE& cc = check_isomorphism(one_neighbor);
//      one_neighbor->set_canonical_code(cc);
//    }
    n->_is_processed=true;
  }

  private:
  NODE_MAP _node_map;	//!< store all lattice node.
  PatternFactory<PAT>* _pf;//!< a PatternFactory object
  PAT* _last;//!< store last node of the random walk.
  vector<int> _last_vids;
  LATTICE_NODE* _last_node;

  bool _isInitialized = false;

  time_tracker tt;
  int _graph_id;
  int _subgraph_size;
  DATABASE* _database;
  PAT* _graph ;
};

#endif
