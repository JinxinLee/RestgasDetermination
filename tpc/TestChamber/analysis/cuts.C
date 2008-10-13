#ifndef CUTS_C
#define CUTS_C

#include "../src/TCcluster.h"
#include "../src/TCtrack.h"
#include <algorithm>
#include <vector>
#include <set>


bool IEEE(TCtrack t){

  std::vector<TCcluster> tpc;//will hold clusters in TPC
  for(int i=0;i<t.nCl();++i){
    TCcluster c = t.getCl(i);
    if(c.getId()>=200 && c.getId()<=299){
      for(int j=0;j<c.nRaw();++j){
	tpc.push_back(c.getRaw(j));
      }
    }
  }

  if (t.nClFit()<5){
    return false;
  }
  if (fabs(t.getThX())<1){
    return false;
  }
  if (fabs(t.getThY())<1){
    return false;
  }
  if (fabs(t.getTh())<1){
    return false;
  }
  if (fabs(t.getTh())>30){

    return false;
  }


  return true;
}


#endif
