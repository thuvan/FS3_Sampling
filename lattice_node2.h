#ifndef LATTICE_NODE2_H_INCLUDED
#define LATTICE_NODE2_H_INCLUDED


#include <ext/hash_set>
#include "graph_iso_check.h"
#include <algorithm>
#include "helper_funs.h"
#include "pattern_factory.h"

//! A lattice_node template structure
template <typename PAT >
struct lattice_node2
{

  typedef lattice_node2<PAT> L_NODE;
  typedef typename PAT::VERTEX_T V_T;
  typedef typename PAT::EDGE_T E_T;
  typedef pair<int, int> EDGE;

  string get_key() {
    if (_key==NULL)
      _key = createKey(_vids);
    return _key;
  }

  string createKey(vector<V_T> vids)
  {
    ostringstream t_ss;
    //sort vids
    sort(vids.begin(),vids.end());
    //get string from ordered ids
    for(int i=0;i<vids.size();i++)
      t_ss << vids[i];
    string t_str = t_ss.str();
    return t_str;
  }

	//! Constructor
  lattice_node(vector<V_T> vids) {
    _vids = vids;
  }

  bool _is_processed; //!< it is true, when we know all neighbors and their status of this pattern
  PAT* _pat;//!< Store a pattern in lattice node
  vector<V_T> _vids; //!< Store id of vertex in current subgraph
  vector<L_NODE*> _neighbors;//!< Store all the neighbors of a node

  vector<double> _neighbor_prob;
  int _super_cnt;

  string _key=NULL;
};


#endif // LATTICE_NODE2_H_INCLUDED
