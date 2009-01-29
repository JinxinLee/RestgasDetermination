#ifndef TCTRACK_H
#define TCTRACK_H
#include"TObject.h"
#include<vector>
#include<iostream>
#include"TCcluster.h"
class TCtrack : public TObject{
 public:
  std::vector<unsigned int> index;
  std::vector<TCcluster*> pointers;
  void clear(){index.clear();}
  void print(){
    std::cout << "TCtrack::print()" << std::endl;
    for(int i=0;i<pointers.size();++i){
      std::cout << pointers.at(i)->x << "'" << pointers.at(i)->y << std::endl;
    }
  }
  ClassDef(TCtrack,1);
};


#endif
