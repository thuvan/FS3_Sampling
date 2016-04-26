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
#include "VectorUtility.h"

/**
 * This performs random walk on frequent patterns to find uniform sample of frequent
 * patterns. It save the frequent patterns for fast processing
 */
//! A RandomWalkManager_Freq template class.
template<class PAT>
class Uniform_SubGraph_Random_Walk
{

  public:
  typedef typename PAT::VERTEX_T V_T;
  typedef typename PAT::EDGE_T E_T;
  typedef typename PAT::EDGE EDGE;
  typedef typename multiset<EDGE>::iterator EDGE_IT;
  typedef typename multiset<EDGE>::const_iterator EDGE_CIT;

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

  /*! \fn LATTICE_NODE* create_lattice_node(PAT*& p)
 		*  \brief A member function to create a new lattice node.
		*	 It first check whether the pattern p come as parameter is already a lattice node from its canonical code.
		*	 If not a new lattice node is created.
		*	\param p a reference of a pointer of PAT.
		* \return a pointer of LATTICE_NODE
	*/
  LATTICE_NODE* create_lattice_node(vector<int>& vids) {
    LATTICE_NODE* lNode = new LATTICE_NODE();
    for(int i=0;i<vids.size();i++)
    {
      lNode->_vids.push_back(vids[i]);
      //lNode->_nbs_vids.push_back(new vector<int>());
    }
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
      node->_is_processed = false;
      insert_lattice_node(node_key, node);
    }
    else {
      delete lNode;
    }
    cout <<"end create_lattice_node\n";
    return node;
  }


  // random walk manager initialize with a frequent pattern node
	/*! \fn LATTICE_NODE* initialize()
 		*  \brief A member function to initialize the walk in itemset Lattice. Initialization completed
		*		by selecting an size one frequent pattern.
 		*  \return a pointer of LATTICE_NODE type.
 		*/
  LATTICE_NODE* initialize() {
    vector<int> vids; //!< Store id of vertex in current subgraph

    //get random vertex id
    _pf->get_random_subgraph(_graph, _subgraph_size,vids);

    LATTICE_NODE* lNode;
    lNode = create_lattice_node(vids);

//    cout<<"vertex indexs: ";
//    for(int i=0;i<lNode->_vids.size();i++)
//      cout<<lNode->_vids[i]<<" ";
//    cout<<endl;

    //_last_node = create_lattice_node(_graph,vids);
    _last_node = lNode;
    _isInitialized=true;
    return _last_node;
  }

  	/*! \fn LATTICE_NODE* get_next(LATTICE_NODE* current) const
 		*  \brief A member function to get next node on itemset lattice to jump from current.
		*	 Acceptance probability calculation of Metropolis-Hastings
		* algorithm is implemented here.
		*	\param current a pointer of LATTICE_NODE.
		* \return a pointer of LATTICE_NODE.
	*/
  LATTICE_NODE* get_random_next(LATTICE_NODE* current){
    vector<int> vids;
    int idx = boost_get_a_random_number(0,current->_neighbors_count);
    current->get_neighbor_at(idx,vids);

    LATTICE_NODE* lNode = create_lattice_node(vids);
    return lNode;
  }

  struct sort_pred {
    bool operator()(const std::pair<int,double> &left, const std::pair<int,double> &right) {
        return left.second < right.second;
    }
  };

   	/*! \fn LATTICE_NODE* get_next(LATTICE_NODE* current) const
 		*  \brief A member function to get next node on itemset lattice to jump from current.
		*	 Acceptance probability calculation of Metropolis-Hastings
		* algorithm is implemented here.
		*	\param current a pointer of LATTICE_NODE.
		* \return a pointer of LATTICE_NODE.
	*/
  LATTICE_NODE* get_random_next2(LATTICE_NODE* current){
    cout <<"begin get_random_next2\n";
    ///rank vertex of subgraph
    vector<pair<int,double> > vrank(current->_vids.size()); // <index in _vids, rank>
    for(int i=0;i<current->_vids.size();i++)
    {
      int vid = current->_vids[i];
      V_T vlb = _graph->label(vid);
      ///rank of v = frequency(v) * #neighbor(v);
      int vfreq = _database->get_vertex_frequency(vlb);
      int nb_count = current->get_neighbors_of_vid(vid)->size();
      vrank[i] = make_pair(i,vfreq*nb_count); ///OPINION: frequency of vertex is very large compare to number of its neighbor
    }
    std::sort(vrank.begin(), vrank.end(),sort_pred());
    ///test
//    cout << "rank of vertexes:"<<endl;
//    for(int i=0;i<vrank.size();i++)
//      cout<<"\t"<<current->_vids[vrank[i].first] <<", "<<vrank[i].second<<endl;

    ///select vertex for remove: select vertex co rank min
    int removeVid;
    vector<int>* replacableVids;
    int i = 0;
    int removeIndex=-1;
    while (i<vrank.size()){
      removeVid = current->_vids[vrank[i].first];
      replacableVids = current->get_neighbors_of_vid(removeVid);
      if (replacableVids->size() > 0){
        removeIndex = vrank[i].first;
        break;
      }
      i++;
    }
    cout<< "removed vertex index: "<<removeIndex<<", vid: "<<removeVid<<endl;
    cout<< "replaceable vids: ";
    print_vector(*replacableVids);

    ///select vertex that has max rank
    double maxScore=-1;
    int maxIndex = -1;
    int addVid;
    for(int i=0;i<replacableVids->size();i++){
      int vid = replacableVids->at(i);
      V_T vlb = _graph->label(vid);
      ///rank of v = frequency(v) * #neighbor(v);
      int vfreq = _database->get_vertex_frequency(vlb);
      vector<int> nbs;
      _graph->get_adj_matrix()->neighbors(vid,nbs);
      double score = vfreq*nbs.size();
      if (score>maxScore){
        maxScore = score;
        maxIndex = i;
        addVid = vid;
      }
    }
    cout<<" add vertex index: "<<maxIndex<<", vid: "<<addVid<<", score: "<<maxScore<<endl;
    ///tao next node
    vector<int> vids(current->_vids);
    vids[removeIndex]=addVid;
    cout<< "vids of next subgraph: ";
    print_vector(vids);

    LATTICE_NODE* lNode = create_lattice_node(vids);
    cout <<"end get_random_next2\n";
    return lNode;
  }



  LATTICE_NODE* sampling_subgraph(double& score)
  {
    if(!_isInitialized)
      _last_node = initialize();

    process_node(_last_node);

    double score_x = _last_node->_score;
    double nbs_x = _last_node->_neighbors_count;

    LATTICE_NODE* next=NULL;
    ///TODO: viet vong while o day
    while(next==NULL)
    {
      next = get_random_next(_last_node);
      process_node(next);
      //tinh score of y
      double score_y = next->_score;
      double nbs_y = next->_neighbors_count;
      double accp_value = (nbs_x * score_y)/(nbs_y * score_x);
      double accp_probablility = accp_value<=1?accp_value:1;
      double rd = random_uni01();
      //check probability
      if (rd <= accp_probablility){
        _last_node = next;
        break;
      }
      next = NULL;
    }
    score = _last_node->_score;
    return _last_node;
  }

  LATTICE_NODE* sampling_subgraph_by_Edge_Graph(double& score)
  {
    cout <<"begin sampling_subgraph_by_Edge_Graph\n";
    if(!_isInitialized)
      _last_node = initialize();

    process_node(_last_node);
    double score_x = _last_node->_score;
    double nbs_x = _last_node->_neighbors_count;

    LATTICE_NODE* next=NULL;
    ///TODO: viet vong while o day
    while(next==NULL)
    {
      cout <<"begin while\n";
      next = get_random_next2(_last_node);
      process_node(next);
      cout <<"while after process_node\n";
      //tinh score of y
      double score_y = next->_score;
      double nbs_y = next->_neighbors_count;
      double accp_value = (nbs_x * score_y)/(nbs_y * score_x);
      double accp_probablility = accp_value<=1?accp_value:1;
      double rd = random_uni01();
      //check probability
      if (rd <= accp_probablility){
        _last_node = next;
        break;
      }
      next = NULL;
    }
    score = _last_node->_score;
    cout <<"end sampling_subgraph_by_Edge_Graph\n";
    return _last_node;
  }

  double compute_score(LATTICE_NODE* lNode)
  {
    //score cua subgraph bang frequency cua subgraph do
    //= size cua intersection cua support_set cua cac canh cua subgraph
    double score;
    PAT* pat = lNode->_pat;
    const multiset<EDGE>& mset = pat->get_edgeset();
    //vector<int> spset_intersec;
    lNode->_support_set=new vector<int>();
    //multiset<int>::iterator it;
    for (EDGE_IT it=mset.begin(); it!=mset.end(); ++it) //duyet qua danh sach canh
    {
      vector<int> spset = _database->get_support_set(*it); //lay ds graph support canh it
      if(lNode->_support_set->size()==0) //neu support_set cua subgraph = 0
      {
        for(int i=0;i<spset.size();i++)
          lNode->_support_set->push_back(spset[i]); //them cac graph support canh it vao ds
      }
      else // support set da co du lieu
      {
        intersect_vectors(lNode->_support_set,spset);
      }
    }
    ///find set of all missing edge in subgraph
    vector<EDGE> missing_edge_set;
    for(int i=0;i<lNode->_vids.size();i++)
      for(int j=i+1;j<lNode->_vids.size();j++)
      {
        int v1 = lNode->_vids[i];
        int v2 = lNode->_vids[j];
        if (_graph->get_adj_matrix()->at(v1,v2)==false){
          V_T vl1 = _graph->get_adj_matrix()->label(v1);
          V_T vl2 = _graph->get_adj_matrix()->label(v2);
          EDGE e = (vl1<vl2)? make_pair(make_pair(vl1, vl2), 1) : make_pair(make_pair(vl2, vl1), 1);
          missing_edge_set.push_back(e);
        }
      }
    cout<<"missing edges: "<<missing_edge_set.size()<<endl;
    for(int i=0;i<missing_edge_set.size();i++)
      cout<<"("<<missing_edge_set[i].first.first<<", "<<missing_edge_set[i].first.second<<"); ";
    cout <<endl;

    ///find union set of graphs that support missing edges
    vector<int> support_missing_edges_graphid;
    for(int i=0;i<missing_edge_set.size();i++)
    {
      vector<int> spset = _database->get_support_set(missing_edge_set[i]); //lay ds graph support canh it
      union_vectors(&support_missing_edges_graphid,spset);
    }
    ///subtract intersection of full support set by union of support set of missing edges
    subtract_vectors(lNode->_support_set,support_missing_edges_graphid);

    lNode->_score = lNode->_support_set->size();
    return lNode->_support_set->size();
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
  void process_node(LATTICE_NODE* n)
  {
    if (n->_is_processed)
      return;
    cout <<"begin process_node\n";
    //khoi tao mang neighbor_vids
    for(int i=0;i<n->_vids.size();i++)
    {
      n->_nbs_vids.push_back(new vector<int>());
    }

    int dx = _pf->count_neighbor_subgraph(_graph, n->_vids,n->_nbs_vids);
    n->_neighbors_count = dx;

    vector<int> vids = n->_vids; // các id cua cac dinh trong subgraph do
    vector<vector<int>* > nbs_vids = n->_nbs_vids; // chứa các neighbor của từng đỉnh

    cout<<"neighbors count: "<<dx<<endl;
    for(int i=0;i<nbs_vids.size();i++)
    {
      cout<<i<<") v"<<vids[i]<<": ";
      for (int j=0;j<nbs_vids[i]->size();j++)
        cout<<nbs_vids[i]->operator[](j)<<" ";
      cout<<endl;
    }
    cout<<"make_subgraph_from_vids \n";
    ///TODO: chi tao subgraph instance khi nao can, chuyen ra ben ngoai cho xu ly queue
    PAT* p = _pf->make_subgraph_from_vids(_graph,n->_vids);
    cout<<"after make_subgraph_from_vids \n";
    cout<< *p <<endl;
    const typename PAT::CAN_CODE& cc = check_isomorphism(p);
    cout<<"after check_isomorphism \n";
    p->set_canonical_code(cc);
    cout<<"after set_canonical_code \n";
    n->_pat = p;

    cout << "Current pattern:\n";
    cout << *p;

    compute_score(n);
    n->_is_processed=true;
    cout <<"end process_node\n";
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
