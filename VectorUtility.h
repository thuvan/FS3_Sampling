//! \file lattice_node.h - struct to represent a node on itemset lattice
#ifndef _VECTOR_UTILITY_H
#define _VECTOR_UTILITY_H

#include <algorithm>
#include<vector>

int find_in_vector(const vector<int>& v, const int& value)
{
  for(int i=0;i<v.size();i++)
    if (v[i]==value)
      return i;
  return -1;
}

#endif
