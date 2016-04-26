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

/*! \fn void intersect_vectors(vector<int>& v1,const vector<int>& v2){
 		*  \brief intersect two vector v1 and v2. Result will be stored in v1

	*/
void intersect_vectors(vector<int>* v1,const vector<int>& v2){
  for(int i=v1->size()-1;i>=0;i--)
  {
    bool found=false;
    for(int j=0;j<v2.size();j++)
      if (v1->operator[](i)==v2[j])
      {
        found = true;
        break;
      }
    if (!found) //neu graph thu i trong support_set ko support cho canh it thi remove
      v1->erase (v1->begin()+i);
  }
}

/*! \fn void union_vectors(vector<int>* v1,const vector<int>& v2){
 		*  \brief union two vector v1 and v2. Result will be stored in v1

	*/
void union_vectors(vector<int>* v1,const vector<int>& v2){
  for(int i=0;i<v2.size();i++)
  {
    bool found=false;
    for(int j=0;j<v1->size();j++)
      if(v2[i]==v1->operator[](j))
      {
        found = true;
        break;
      }
    if (!found) //neu graph thu i trong support_set ko support cho canh it thi remove
      v1->push_back(v2[i]);
  }
}

/*! \fn void subtract_vectors(vector<int>* v1,const vector<int>& v2){
 		*  \brief subtract set v1 by set v2. Result will be stored in v1

	*/
void subtract_vectors(vector<int>* v1,const vector<int>& v2){
  for(int i=v1->size()-1;i>=0;i--)
  {
    bool found=false;
    for(int j=0;j<v2.size();j++)
      if (v1->operator[](i)==v2[j])
      {
        found = true;
        break;
      }
    if (found) //neu graph thu i trong support_set ko support cho canh it thi remove
      v1->erase (v1->begin()+i);
  }
}

#endif
