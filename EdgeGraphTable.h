#ifndef EDGEGRAPHTABLE_H_INCLUDED
#define EDGEGRAPHTABLE_H_INCLUDED

#include <iostream>


typedef ExPattern<int, int> PAT;

class EdgeGraphTable
{
  /*
    mapping from edge to idset of supported graphs
    ordering the edge set by its frequent
    indexing the edge set by its id

    add a new graph g
      for all edge e in g
        if (e is not in edge set)
          insert e with frequency=1, graph_set={g} into the edge set
        else // e is already in the edge set
        {
          update record of e in the edge set by:
          frequency += 1;
          graph_set += g; //add g to the graph_set
          re-order the edge set by frequencies
        }

    remove a graph g
      for all edge e in g
      {
        graph_set -= g;
        frequency -=1;
        if (frequency ==0 ) // graph_set is empty
          delete record of e
        re-order the edge set by frequencies
      }

  */

  //Data members
  //map<edge, idset> map_edge_graphs;
  //vector<pair<edge, int>> edge_frequent; // <edge, count>

public:
  EdgeGraphTable()
  {

  }

  void add_graph(PAT* g)
  {
    /*
      for each edge e in graph
        find
    */
  }

};


#endif // EDGEGRAPHTABLE_H_INCLUDED
