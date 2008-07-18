#ifndef CONSECCUT_C
#define CONSECCUT_C

#include "../src/TCcluster.h"
#include "../src/TCtrack.h"
#include <algorithm>
#include <vector>
#include <set>

//pitches
#define UP 6200 //integer in microns
#define VP 1000

bool consecCut(TCtrack t){

  std::vector<TCcluster> tpc;//will hold clusters in TPC
  for(int i=0;i<t.nCl();++i){
    TCcluster c = t.getCl(i);
    if(c.getId()==200){
      for(int j=0;j<c.nRaw();++j){
	tpc.push_back(c.getRaw(j));
      }
    }
  }
  //std::sort(tpc.begin(),tpc.end(),TCclusterSortUVW());

  std::set<int> uvals;
  std::set<int> vvals;

  for(int i=0;i<tpc.size();++i){
    double u,v;
    u=tpc.at(i).posUVW().X();
    v=tpc.at(i).posUVW().Y();
    uvals.insert( (int) (u*10000));
    vvals.insert( (int) (v*10000));
  }

  bool first;

  int lastu;
  first = true;
  for(std::set<int>::iterator it=uvals.begin();it!=uvals.end();++it){
    if(first){
      first=false;
      lastu=(*it)-UP;
    }
    if(((*it)-lastu) > (UP+2)) {
      return false;
    }
    lastu=*it;
  }
  int lastv;
  first = true;
  for(std::set<int>::iterator it=vvals.begin();it!=vvals.end();++it){
    if(first){
      first=false;
      lastv=(*it)-VP;
    }
    if(((*it)-lastv) > (VP+2)) {
      return false;
    }
    lastv=*it;
  }

  return true;
}


#endif
