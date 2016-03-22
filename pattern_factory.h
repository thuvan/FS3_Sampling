//! \file pattern_factory.h - class to bridge between Random Walk Manager and Database class
#ifndef _PATTERN_FACTORY_
#define _PATTERN_FACTORY_


#include "pattern.h"
#include "database.h"
#include "graph_iso_check.h"


//! A PatternFactory template class
template <typename PAT>
class PatternFactory {

  public:

    typedef typename PAT::VERTEX_T V_T;
    typedef typename PAT::EDGE_T E_T;
    typedef typename PAT::EDGE EDGE;
    typedef Database<PAT> DATABASE;
    typedef typename DATABASE::EDGE_INFO_MAP EDGE_INFO_MAP;
    typedef typename DATABASE::INFO_CIT INFO_CIT;

		/*! \fn static PatternFactory<PAT>* instance(DATABASE* d)
 		*  \brief A member function to set _instance private member.
 		*  \param d a pointer of Database class object.
		*  \return a pointer of PatternFactory object.
 		*/
    static PatternFactory<PAT>* instance(DATABASE* d) {
      if (_instance == 0) {
        _instance = new PatternFactory<PAT>(d);
      }
      return _instance;
    }

		/*! \fn PAT* make_single_edge_pattern(V_T src, V_T dest, E_T e)
 		*  \brief A member function to make pattern of size one.
 		*  \param src,dest a V_T type.
		*  \param e a E_T type.
		*  \return a pointer of PAT.
 		*/
    PAT* make_single_edge_pattern(V_T src, V_T dest, E_T e) {
      vector<V_T> vlabels(2);
      if (src < dest) {
        vlabels[0] = src;
        vlabels[1] = dest;
      }
      else {
        vlabels[0] = dest;
        vlabels[1] = src;
      }
      PAT* p = new PAT(vlabels);
      p->add_edge(0,1,e);
      EDGE edge = make_pair(make_pair(vlabels[0], vlabels[1]), e);
      const vector<int>& vat = _d->get_edge_vat(edge);
      p->set_vat(vat);
      p->set_sup_status(0);
      int minsup = _d->get_minsup();
      if (vat.size() >= minsup)
        p->set_freq();
      return p;
    }

    PAT* make_subgraph_from_vids(PAT* graph, vector<int> vids) {
      vector<V_T> vlabels(vids.size());
      for(int i=0;i<vids.size();i++)
        vlabels[i]=graph->label(vids[i]);

      PAT* p = new PAT(vlabels);
      for(int i=0;i<vids.size();i++){
        for(int j=i+1;j<vids.size();j++)
        if (graph->edge_exist(vids[i],vids[j]))
        {
          E_T e = graph->get_edge_label(vids[i],vids[j]);
          p->add_edge(i,j,e);
        }
      }
      return p;
    }

    // precondition: given pattern is entirely consistent with correct support count
    // and support list
		/*! \fn PAT* pattern_with_edge_removed(PAT* p, const int& a, const int& b)
 		*  \brief A member function returning a pattern after removing an edge.
 		*  \param a,b a constant reference of integer.
		 		*  \param p a pointer of PAT .
		*  \return a pointer PAT.
 		*/
    PAT* pattern_with_edge_removed(PAT* p, const int& a, const int& b) {
#ifdef PRINT
      cout << "Edge(" << a << "," << b << ") are to be removed from:" << endl;
      cout << *p << endl;
      const vector<int>& cur_vat = p->get_vat();
      cout << "its vat size:" << cur_vat.size() << endl;
      std::copy(cur_vat.begin(), cur_vat.end(), ostream_iterator<int>(cout, " "));
      cout << endl;
#endif
      PAT* clone = p->clone();
      EDGE edge = clone->remove_edge(a,b);

      // now support list needs to be updated
      _d->get_exact_sup_from_super_pat_vat(clone);
#ifdef PRINT
      cout << "done\n";
#endif
      return clone;
    }


    //Alg: take any frequent edge and make a single edge pattern
    PAT* get_one_maximal_pattern() {
      int minsup = _d->get_minsup();
      EDGE edge = _d->get_a_freq_edge();
      // cout << "Making single edge pattern:" << edge.first.first << " " << edge.first.second << " " << edge.second << endl;
      PAT* cand_pat = make_single_edge_pattern(edge.first.first, edge.first.second, edge.second);
      return extend_until_max(cand_pat, minsup);
      cand_pat->set_freq();
    }
    PAT* extend_until_p(PAT*& cand_pat, int p){
      //int cur_size =
      int v_count = 2;
    }

  int count_neighbor_subgraph(PAT* graph, vector<int>& vids,vector<vector<int>* >& nbs_vids)
  {
    int c=0;
    for(int i=0;i<vids.size();i++) // chạy từng đỉnh một
    {
      vector<int> nb;
      for(int h=0;h<vids.size();h++)
      {
        if (h==i) // nếu là đỉnh đang duyệt thì bỏ qua. có nghĩa là chỉ duyệt neighbor của các đỉnh còn lại
          continue;
        graph->get_adj_matrix()->neighbors(vids[h],nb);
      }
      //Check if neighbor vertex is not in subgraph => add to neighbor vector
      for(int j=0;j<nb.size();j++)
      {
        //check node j is already in subgraph
        bool isInSubgraph=false;
        for (int k=0;k<vids.size();k++)
          if (nb[j]== vids[k])
          {
            isInSubgraph =true;
            break;
          }

        if (isInSubgraph)
          continue;

        //check if node j is already added
        std::vector<int>::iterator it;
        it = find (nbs_vids[i]->begin(), nbs_vids[i]->end(), nb[j]);
        if(it == nbs_vids[i]->end()){ // check id is existed at nbs_vids[i] or not?
          nbs_vids[i]->push_back(nb[j]);
          c++;
        }
      }
    }
    return c;
  }

    /*! \fn void* get_random_subgraph(PAT* graph, int subgraph_size,vector<int>& vids)
 		*  \brief A member function to get vertex ids of a random subgraph with given size
 		*	\param graph a pointer of Graph.
 		*	\param size size of subgraph.
 		*	\param vids vector that store the vids of a random subgraph
 		*/
  void get_random_subgraph(PAT* graph, int subgraph_size,vector<int>& vids){
    if (graph->size() <subgraph_size){
      cout<<"graph size "<<graph->size()<<" is less than subgraph_size"<<subgraph_size<<endl;
      return;
    }

    vector<V_T> v_nbs;
    int vid1 = boost_get_a_random_number(0,graph->size());
    vids.push_back(vid1);
    graph->get_adj_matrix()->neighbors(vid1,v_nbs);

    while (vids.size()<subgraph_size){
      int idx=boost_get_a_random_number(0, v_nbs.size());
      int vid2 = v_nbs[idx];
      v_nbs.erase(v_nbs.begin()+idx);
      //if vid2 is not existed in vid then push_back
      bool existed = false;
      for(int i=0; i<vids.size();i++)
        if (vids[i] == vid2){
          existed=true;
          break;
        }
      if (!existed){
        vids.push_back(vid2);
        graph->get_adj_matrix()->neighbors(vid2,v_nbs);
      }
    }
  }

//
//  PAT* get_random_subgraph(PAT* graph, int subgraph_size){
//      if (graph->size() <subgraph_size)
//        return NULL;
//
//      vector<int> vids;
//      vector<V_T> v_nbs;
//      int vid1 = boost_get_a_random_number(0,graph->size());
//      vids.push_back(vid1);
//      graph->get_adj_matrix()->neighbors(vid1,v_nbs);
//
//      while (vids.size()<subgraph_size){
//        int idx=boost_get_a_random_number(0, v_nbs.size());
//        int vid2 = v_nbs[idx];
//        v_nbs.erase(v_nbs.begin()+idx);
//        //if vid2 is not existed in vid then push_back
//        bool existed = false;
//        for(vector<int>::iterator it; it<vids.end();it++)
//        if (it* ==vid2){
//          existed=true;
//          break;
//        }
//        if (!existed)
//          vids.push_back(vid2);
//      }
//
//
//      PAT* cand_pat = make_single_edge_pattern(edge.first.first, edge.first.second, edge.second);

//      //get random one edge of graph
//      EDGE edge = _d->get_random_edge(graph);
//      V_T v1 = edge.first.first;
//      V_T v2 = edge.first.second;
//
//      cout << "Making edge with:" << edge.first.first << " " << edge.first.second << " " << edge.second << endl;
//      PAT* cand_pat = make_single_edge_pattern(edge.first.first, edge.first.second, edge.second);
//      cout << "This single edge pattern is made:" << endl;
//      //cout << *cand_pat << endl;
//
//      vector<V_T> vset;
//      vset.push_back(v1);
//      vset.push_back(v2);
//
//      while (vset.size()<subgraph_size)
//      {
//        //get random a vertex, v1;
//        int idx=boost_get_a_random_number(0, vset.size());
//        v1 = vset[idx];
//        //get random a neighbor of the vertex v1, called v2
//        vector<V_T> nbs;
//        graph->get_adj_matrix()->neighbors(v1,nbs);
//        for(int i=0;i<nbs.size();i++)
//          cout<<nbs[i]<<" ";
//        cout <<endl;
//
//
//        //check whether v2 connect with any vertex in vset,and add the corresponding edge
//      }
//      return cand_pat;
//
//      //PAT* cand_pat = get_one_random_one_edge_frequent_pattern();
//      //extend the one_edge
//
//    }



    PAT* get_one_random_maximal_pattern() {
      PAT* cand_pat = get_one_random_one_edge_frequent_pattern();
      int minsup = _d->get_minsup();
      return extend_until_max(cand_pat, minsup);
      cand_pat->set_freq();
    }

		/*! \fn PAT* get_one_frequent_pattern()
 		*  \brief A member function to get a frequent pattern of size one.
		*  \return Pattern type pointer.
 		*/
    PAT* get_one_frequent_pattern() {
      EDGE edge = _d->get_a_freq_edge();
      PAT* cand_pat = make_single_edge_pattern(edge.first.first, edge.first.second, edge.second);
      return cand_pat;
    }

		/*! \fn PAT* get_one_random_one_edge_frequent_pattern()
 		*  \brief A member function to get a random frequent pattern of size one.
		*  \return Pattern type pointer.
 		*/
    PAT* get_one_random_one_edge_frequent_pattern() {
      EDGE edge = _d->get_a_random_freq_edge();
      cout << "Making edge with:" << edge.first.first << " " << edge.first.second << " " << edge.second << endl;
      PAT* cand_pat = make_single_edge_pattern(edge.first.first, edge.first.second, edge.second);
      cout << "This single edge pattern is made:" << endl;
      //cout << *cand_pat << endl;
      return cand_pat;
    }


		/*! \fn int get_super_degree(PAT*& pat)
 		*  \brief A member function to count sub degree of a pattern.
		* This method first find all possible removable edges from a current pattern.Number of removable edges count is the sub degree.
		*  \param pat a pointer of PAT type.
		* \return integer
 		*/
    int get_sub_degree(PAT* pat) {
    //  cout << "In get_sub_degree" << endl;
      if (pat->size() == 0) {
        cout << "ERROR: we are not allowing any null pattern, exiting"<< endl;
        exit(1);
      }
      if (pat->size() <= 2) {
        // cout << "This pattern have a null sub pattern, returing 1" << endl;
        return 1;
      }
      pat->find_removable_edges();
      const vector<pair<int, int> >& re = pat->get_removable_edges();
     // cout << "Out get_sub_degree" << endl;
      return re.size();
    }

		/*! \fn void get_sub_patterns(PAT* pat, vector<PAT*>& sub_patterns)
 		*  \brief A member function to generates all sub patterns of a pattern by removing an edge.
		*  \param pat a pointer of PAT type.
		*  \param sub_patterns a reference of a PAT* vector.
 		*/
    void get_sub_patterns(PAT* pat, vector<PAT*>& sub_patterns) {
      if (pat->size() == 0) return;
      if (pat->size() <= 2) {
        //PAT* null_pat = pat->make_null_pattern(_d->size());
        //sub_patterns.push_back(null_pat);
        return;
      }
      pat->find_removable_edges();
      const vector<pair<int, int> >& re = pat->get_removable_edges();
      //cout << "Number of removable edges:" << re.size();
      set<std::string> codes;

      vector<pair<int, int> >::const_iterator cit = re.begin();
      for (;cit != re.end(); cit++) {
        PAT* sub_pat = pattern_with_edge_removed(pat, cit->first, cit->second);
        check_isomorphism(sub_pat);
        std::string cc_str = sub_pat->get_canonical_code().to_string();
        if (codes.find(cc_str) != codes.end()) {
          delete(sub_pat);
        }
        else {
          sub_patterns.push_back(sub_pat);
          codes.insert(cc_str);
        }
      }
    }

		/*! \fn int get_super_degree(const PAT* pat)
 		*  \brief A member function to calculate super degree of a pattern.
		* This method first find all possible single extension(forward and backward) from a current pattern. For each
		* extension it counts the number of feasible frequent super pattern(current + extension).
		*  \param pat a constant pointer of PAT.

		* \return integer
 		*/

    int get_super_degree(const PAT* pat) {
#ifdef PRINT
      cout<<"In call to get_super_degree" << endl;
#endif

      PAT* edge=0;
      PAT* cand_pat=0;
      int ret_val=0;

      EDGE this_edge;
      typedef map<EDGE, int> EDGE_FREQ;
      typedef typename EDGE_FREQ::const_iterator F_CIT;
      typedef map<std::string, int > ALL_PAT;
      typedef typename ALL_PAT::iterator APIT;
      int minsup = _d->get_minsup();

      if (pat->size() == 0) {
        const EDGE_INFO_MAP& eim = _d->get_all_edge_info();
        return eim.size();
      }
      for (int vid=0; vid<pat->size(); vid++) {  // extentions from all vertices

        V_T src_v=pat->label(vid);
#ifdef PRINT
        cout << "Extending from vertex-id:" << vid << " with label:" << src_v << endl;
#endif

        const typename DATABASE::NEIGHBORS& nbrs=_d->get_neighbors(src_v);

        typename DATABASE::NCIT nit = nbrs.begin();
        for(;nit != nbrs.end();nit++) {
          V_T dest_v = nit->first;
          E_T e_label = nit->second;

          if (src_v < dest_v)
            this_edge = make_pair(make_pair(src_v, dest_v), e_label);
          else
            this_edge = make_pair(make_pair(dest_v, src_v), e_label);
          int frequency = pat->edge_counter(this_edge);
          int max_freq = _d->get_freq(this_edge);
          if (frequency > max_freq) {
            continue;
          }

          edge = make_single_edge_pattern(src_v, dest_v, e_label);

          // trying fwd-extension from this vertex
          cand_pat = pat->clone();
          int lvid = cand_pat->add_vertex(dest_v);
          cand_pat->add_edge(vid, lvid, e_label);
          cand_pat->join_vat(edge);
          if (cand_pat->get_vat().size() < minsup) {
            delete cand_pat;
          }
          else {
            bool freq = _d->get_exact_sup_optimal(cand_pat);
            if (freq == true) { // is the pattern frequent?
#ifdef PRINT
          //    cout << *cand_pat << endl;
#endif
              delete cand_pat;
              ret_val++;
            }
            else {
              delete cand_pat;
            }
          }
          // now trying all the possible back-edges
          vector<int> dest_vids;
          pat->get_vids_for_this_label(dest_v, dest_vids);
          vector<int>::iterator vit = dest_vids.begin();
          while (vit < dest_vids.end()) {
            if (*vit <= vid || pat->edge_exist(vid, *vit))
              vit = dest_vids.erase(vit);
            else
    	      ++vit;
          }

          for (vector<int>::iterator it= dest_vids.begin(); it < dest_vids.end(); it++) {
            cand_pat = pat->clone();
            cand_pat->add_edge(vid, *it, e_label);
            cand_pat->join_vat(edge);
            if (cand_pat->get_vat().size() < minsup) {
              delete cand_pat;
            }
            else {
              bool freq = _d->get_exact_sup_optimal(cand_pat);
              if (freq == true) { // is the pattern frequent?
#ifdef PRINT
           //     cout << *cand_pat << endl;
#endif
                delete cand_pat;
                ret_val++;
              }
              else {
                delete cand_pat;
              }
            }
          }
          delete edge;
          edge = 0;
        }
      }
     // cout << "Returning from get_super_degree" << endl;
      return ret_val;
    }

    void get_neighbors_subgraph(const PAT* pat,vector<int>& vids, vector<PAT*>& super_patterns) {
#ifdef PRINT
      cout<<"In call to get_all_frequent_super_pattern\n";
#endif

      PAT* edge=0;
      PAT* cand_pat=0;

      EDGE this_edge;
      typedef map<EDGE, int> EDGE_FREQ;
      typedef typename EDGE_FREQ::const_iterator F_CIT;
      typedef map<std::string, int > ALL_PAT;
      typedef typename ALL_PAT::iterator APIT;
      int subgraph_size = _d->get_subgraph_size();

      ///TODO: not implemented


    }

			/*! \fn void get_freq_super_patterns(const PAT* pat, vector<PAT*>& super_patterns)
 		*  \brief A member function to generate super pattern of a pattern.
		* This method first find all possible single extension(forward and backward) from a current pattern. For each
		* extension it creates all frequent new pattern(current + extension).
		*  \param pat a constant pointer of PAT.
		*  \param super_patterns a reference of a PAT* vector.
		* \return integer
 		*/
    void get_freq_super_patterns(const PAT* pat, vector<PAT*>& super_patterns) {
#ifdef PRINT
      cout<<"In call to get_all_frequent_super_pattern\n";
#endif

      PAT* edge=0;
      PAT* cand_pat=0;

      EDGE this_edge;
      typedef map<EDGE, int> EDGE_FREQ;
      typedef typename EDGE_FREQ::const_iterator F_CIT;
      typedef map<std::string, int > ALL_PAT;
      typedef typename ALL_PAT::iterator APIT;
      int minsup = _d->get_minsup();

      if (pat->size() == 0) {
        const EDGE_INFO_MAP& eim = _d->get_all_edge_info();
        INFO_CIT cit;
        for (cit = eim.begin(); cit != eim.end(); cit++) {
          PAT* p = make_single_edge_pattern(cit->first.first.first, cit->first.first.second, cit->first.second);
          super_patterns.push_back(p);
        }
        return;
      }

      for (int vid=0; vid<pat->size(); vid++) {  // extentions from all vertices

        V_T src_v=pat->label(vid);
#ifdef PRINT
        cout << "Extending from vertex-id:" << vid << " with label:" << src_v << endl;
#endif

        const typename DATABASE::NEIGHBORS& nbrs=_d->get_neighbors(src_v);

        typename DATABASE::NCIT nit = nbrs.begin();
        for(;nit != nbrs.end();nit++) {
          V_T dest_v = nit->first;
          E_T e_label = nit->second;
#ifdef PRINT
        cout << "Extending to vertex-id:" << dest_v << " with edge label:" << e_label <<nbrs.size()<< endl;
#endif
          if (src_v < dest_v)
            this_edge = make_pair(make_pair(src_v, dest_v), e_label);
          else
            this_edge = make_pair(make_pair(dest_v, src_v), e_label);
          int frequency = pat->edge_counter(this_edge);
          int max_freq = _d->get_freq(this_edge);
          if (frequency > max_freq) {
            continue;
          }

          edge = make_single_edge_pattern(src_v, dest_v, e_label);

          // trying fwd-extension from this vertex
          cand_pat = pat->clone();
          int lvid = cand_pat->add_vertex(dest_v);
          cand_pat->add_edge(vid, lvid, e_label);
          cand_pat->join_vat(edge);
          if (cand_pat->get_vat().size() < minsup) {
            delete cand_pat;
          }
          else {
            bool freq = _d->get_exact_sup_optimal(cand_pat);
            if (freq == true) { // is the pattern frequent?
#ifdef PRINT
              cout << *cand_pat << endl;
#endif
              super_patterns.push_back(cand_pat);
            }
            else {
              delete cand_pat;
            }
          }
          // now trying all the possible back-edges
          vector<int> dest_vids;
          pat->get_vids_for_this_label(dest_v, dest_vids);
          vector<int>::iterator vit = dest_vids.begin();
          while (vit < dest_vids.end()) {
            if (*vit <= vid || pat->edge_exist(vid, *vit))
              vit = dest_vids.erase(vit);
            else
    	      ++vit;
          }

          for (vector<int>::iterator it= dest_vids.begin(); it < dest_vids.end(); it++) {
            cand_pat = pat->clone();
            cand_pat->add_edge(vid, *it, e_label);
            cand_pat->join_vat(edge);
            if (cand_pat->get_vat().size() < minsup) {
              delete cand_pat;
            }
            else {
              bool freq = _d->get_exact_sup_optimal(cand_pat);
              if (freq == true) { // is the pattern frequent?
#ifdef PRINT
                cout << *cand_pat << endl;
#endif
                super_patterns.push_back(cand_pat);
              }
              else {
                delete cand_pat;
              }
            }
          }
          delete edge;
          edge = 0;
        }
      }
      // sometimes few neighbors are actually unique, need to check that
      set<std::string> codes;
      for (int i=0; i < super_patterns.size();) {

        check_isomorphism(super_patterns[i]);
        std::string cc_str = super_patterns[i]->get_canonical_code().to_string();
        pair<set<std::string>::iterator, bool> ret_val = codes.insert(cc_str);
        if (ret_val.second == false) {
          delete super_patterns[i];
          super_patterns.erase(super_patterns.begin()+i);
        }
        else
          i++;
      }
    }

    static PatternFactory<PAT>* set_static_data() {
      return 0;
    }

    DATABASE* get_database() const {
      return _d;
    }
  protected:
    PatternFactory(DATABASE* d) {
      _d = d;
    }

  private:

  // find exact support list after joining
  bool update_vat(PAT* pat) const {
    bool is_freq = _d->get_exact_sup(pat);
    if (is_freq ==false) return false;
    return true;
  }

  DATABASE* _d;
  static PatternFactory<PAT>* _instance;
};

#endif
