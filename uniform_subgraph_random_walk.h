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
    LATTICE_NODE* lNode = new LATTICE_NODE();

    //get random vertex id
    _pf->get_random_subgraph(_graph, _subgraph_size,lNode->_vids);

//    cout<<"vertex indexs: ";
//    for(int i=0;i<lNode->_vids.size();i++)
//      cout<<lNode->_vids[i]<<" ";
//    cout<<endl;

    //get key code for Lattice node
    std::string node_key = lNode->get_key();
    cout<<"code: "<<node_key<<endl;

//    cout<<"vertex indexs after getkey(): ";
//    for(int i=0;i<lNode->_vids.size();i++)
//      cout<<lNode->_vids[i]<<" ";
//    cout<<endl;

    //push into node map
    LATTICE_NODE* node = exists(node_key);
    if (node == 0) {  // new pattern
      node = lNode;
      //create subgraph instance
      ///TODO: chi tao subgraph instance khi nao can, chuyen ra ben ngoai cho xu ly queue
      PAT* p = _pf->make_subgraph_from_vids(_graph,lNode->_vids);
      const typename PAT::CAN_CODE& cc = check_isomorphism(p);
      p->set_canonical_code(cc);
      node->_pat = p;

      node->_is_processed = false;
      insert_lattice_node(node_key, node);
    }
    else {
      //delete p;
      //p = node->_pat;
      delete lNode;
    }

    //_last_node = create_lattice_node(_graph,vids);
    _last_node = node;
    process_node(_last_node);

    _isInitialized=true;
    return _last_node;
  }

  PAT* sampling_subgraph(double& score)
  {
    if(!_isInitialized)
      _last_node = initialize();

     LATTICE_NODE* next=NULL;
    ///TODO: viet vong while o day
    while(next==NULL)
    {
        //next = random 1 neighbor of _last_node
            // random 1 neighbor

                //kiem tra co con connected hay ko

                //Neu connect:
                    //them 1 dinh bk trong ds canh ke cua cac dinh con lai
                //Neu ko connect:
                    //tim dinh ke chung cua 2 dinh bi dut
            // sau khi tìm duoc dỉnh để remove và đỉnh để add thì xây dựng subgraph mới y
        //tinh score of y

        //check probability
        next = _last_node; //gan tam de cho ko bi loi
    }

    //return next->_pat;;
    return next->_pat;
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
    ///TODO:
    //khoi tao cac field: score, neighbors

    if (n->_is_processed) return;
        PAT* p = n->_pat;

    vector<int> vids = n->_vids; // các id cua cac dinh trong subgraph do
    vector<vector<int> > nbs_vids(vids.size()); // chứa các neighbor của từng đỉnh

    int dx = _pf->count_neighbor_subgraph(_graph,vids,nbs_vids);
    cout<<"neighbors count: "<<dx<<endl;
    for(int i=0;i<nbs_vids.size();i++)
    {
      cout<<i<<": ";
      for (int j=0;j<nbs_vids[i].size();j++)
        cout<<nbs_vids[i][j]<<" ";
      cout<<endl;
    }

    cout << "Current pattern:\n";
    cout << *p;
    //vector<PAT*> neighbors;
    //_pf->get_neighbors_subgraph(p,n->_vids, neighbors);
    //cout << "Its neighbors:" << endl;
    //cout << "Total neighbors="<< neighbors.size() << endl;

    n->_is_processed=true;
  }

//================================= CODE DA BI XOA ====================================

//	/*! \fn LATTICE_NODE* get_next(LATTICE_NODE* current) const
// 		*  \brief A member function to get next node on itemset lattice to jump from current.
//		*	 Acceptance probability calculation of Metropolis-Hastings
//		* algorithm is implemented here.
//		*	\param current a pointer of LATTICE_NODE.
//		* \return a pointer of LATTICE_NODE.
//	*/
//  LATTICE_NODE* get_next(LATTICE_NODE* current) const {
//    ///TODO: not implemented
//    return NULL;
//    int total=current->_neighbor_prob.size();
//#ifdef PRINT
//   std::copy(current->_neighbor_prob.begin(), current->_neighbor_prob.end(), ostream_iterator<double>(cout," "));
//    cout << endl;
//#endif
//    vector<double> prob(total+1);
//    prob[0]=current->_neighbor_prob[0];
//    for (int i=1; i<total; i++) {
//      prob[i]=prob[i-1]+current->_neighbor_prob[i];
//    }
//    assert(prob[total-1]<=1.00001);
//    prob[total]=1;
//#ifdef PRINT
//    std::copy(prob.begin(), prob.end(), ostream_iterator<double>(cout," "));
//    cout << endl;
//#endif
//    int idx;
//    do {
//      idx = randomWithDiscreteProbability(prob);
//    } while (idx == total);
////    cout << "returning with:" << idx << endl;
//    return current->_neighbors[idx];
//  }


//	/*! \fn LATTICE_NODE* create_lattice_node(PAT*& p)
// 		*  \brief A member function to create a new lattice node.
//		*	 It first check whether the pattern p come as parameter is already a lattice node from its canonical code.
//		*	 If not a new lattice node is created.
//		*	\param p a reference of a pointer of PAT.
//		* \return a pointer of LATTICE_NODE
//	*/
//  LATTICE_NODE* create_lattice_node(PAT*& graph,vector<int>& vids) {
//    //get subgraph from vids
//    PAT* p = _pf->make_subgraph_from_vids(graph,vids);
//
//    //get can_code of subgraph
//    const typename PAT::CAN_CODE& cc = check_isomorphism(p);
//    p->set_canonical_code(cc);
//    std::string min_dfs_cc = cc.to_string();
//
//    LATTICE_NODE* node = exists(min_dfs_cc);
//    if (node == 0) {  // new pattern
//      node = new LATTICE_NODE(p,vids);
//      node->_is_processed = false;
//      insert_lattice_node(min_dfs_cc, node);
//    }
//    else {
//      delete p;
//      p = node->_pat;
//    }
//    return node;
//  }

//=======================================================================================

  private:
  NODE_MAP _node_map;	// hashtable of lattice node, save all lattice node are processed //!< store all lattice node.
  PatternFactory<PAT>* _pf; // unity class to process graphs and subgraphs //!< a PatternFactory object
  PAT* _last;//!< store last node of the random walk.
  vector<int> _last_vids;
  LATTICE_NODE* _last_node; // current node (current subgraph), chứa danh sách các Lattice_node là neighbor của nó.

  bool _isInitialized = false;

  time_tracker tt;
  int _graph_id; //id of super graph
  int _subgraph_size;// size of subgraph will be sampled
  DATABASE* _database; // the pointer to database
  PAT* _graph ; //pointer to super graph
};

#endif
