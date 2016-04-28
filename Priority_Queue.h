#ifndef PRIORITY_QUEUE_H_INCLUDED
#define PRIORITY_QUEUE_H_INCLUDED

#include <iostream>
#include <queue>

typedef ExPattern<int, int> PAT;

class Queue_Item{
public:
  void free(){
//    if (subgraph!=NULL)
//      delete(subgraph);
  }
  void print(){
    cout<<"score= "<<score<<"\t time= "<<insert_time<<endl;
    cout<<"idset = <";
    for(int i=0;i<idset.size();i++)
      cout<<idset[i]<<",";
    cout<<">"<<endl;
    cout<<"subgraph: "<<endl;
    cout<<*subgraph<<endl;
  }

  void print_to_file(ostream& ostr,int id){
    ostr<<"t #"<<id<<" size= "<<subgraph->size()<<", score= "<<score<<", time= "<<insert_time<<", freq= "<<idset.size()<<", idset= [";
    for(int i=0;i<idset.size();i++){
        if (i==0)
          ostr<<idset[i];
        else
          ostr<<", "<<idset[i];
    }
    ostr<<"]"<<endl;
    //ostr<<"subgraph: "<<endl;
    subgraph->print(ostr);
  }
  ///TODO: score cua subgraph o moi graph co the khac nhau => score luu score o graph nao?
  double score;
  vector<int> idset;
  long insert_time;
  PAT* subgraph;
};

class Priority_Queue
{
  bool isLessThan(const Queue_Item* a, const Queue_Item* b) const
    {
      if (a->idset.size() < b->idset.size())
        return true;
      else if (a->idset.size() == b->idset.size())
        if (a->score < b->score)
          return true;
        else if (a->score == b->score )
          ///TODO: check lai cho so sanh nay, ko biet dung hay sai
          if (a->insert_time < b->insert_time)
            return true;
      return false;
    }

  public:
    Priority_Queue(int max_size)
    {
      _max_size = max_size;
    }

    void free(){
      for(int i=0;i<_data.size();i++){
        _data[i]->free();
        delete(_data[i]);
      }
    }

    void push(Queue_Item* item)
    {
      if(_data.size()==0){
        _data.push_back(item);
        return;
      }
      int i=_data.size()-1;
      if (!isLessThan(item, _data[i])){
        _data.push_back(item);
        return;
      }else
        _data.push_back(_data[i]);

      while (i>0 && isLessThan(item,_data[i-1])){
        _data[i]=_data[i-1];
        i--;
      }
      _data[i] = item;
    }
    void evictLast()
    {
      if (_data.size()>0){
        _data[0]->free();
        _data.erase(_data.begin());
      }
    }

    void print()
    {
      for(int i=_data.size()-1;i>=0;i--){
        Queue_Item* item = _data[i];
        cout<<(_data.size()-i-1)<<"): "<<endl;
        item->print();
      }
    }

    void print_to_file(char* out_file_name)
    {
      std::ofstream ostr;
      ostr.open(out_file_name);
      if (!ostr.is_open()){
        cout<< "Unable to open file "<<out_file_name<<" for writing."<<endl;
        return;
      }
      cout<< "Begin write queue to file '"<<out_file_name<<"' \n";
      for(int i=_data.size()-1;i>=0;i--){
        Queue_Item* item = _data[i];
        //ostr<<(_data.size()-i-1)<<"): "<<endl;
        int id = _data.size()-1-i;
        item->print_to_file(ostr,id);
      }
      ostr.close();
    }

    int size(){ return _data.size();}

    bool isFull(){return _data.size()>=_max_size; }

    double getHalfAvgScore()
    {
      int half = _data.size()/2;
      double sumScore = 0;
      for(int i=0;i<half;i++)
        sumScore += _data[i]->score;
      return sumScore/half;
    }

    double getMinScore()
    {
      return _data[0]->score;
    }

    Queue_Item* findByGraph(PAT* g)
    {
      string str = g->get_canonical_code().to_string();
      //cout<<"DEBUG: findByGraph, str_code = "<<str<<endl;

      for(int i=0;i<_data.size();i++){
//        string str2 = _data[i]->subgraph->get_canonical_code().to_string();
//        cout<<"DEBUG: findByGraph, str_code2 = "<<str2<<endl;
        if (str.compare(_data[i]->subgraph->get_canonical_code().to_string())==0)
          return _data[i];
      }
      return NULL;
    }

    int _max_size;
    vector<Queue_Item*> _data;

};


#endif // PRIORITY_QUEUE_H_INCLUDED
