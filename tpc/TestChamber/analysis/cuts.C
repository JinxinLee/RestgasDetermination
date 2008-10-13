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

  if (tr.nClFit()>5){
    if (fabs(tr.getThX())>1){
      if (fabs(tr.getThY())>1){
	if (fabs(tr.getTh())>1){
	  if (fabs(tr.getTh())<30){

      return false;
	  }
	}
      }
    }
  }

  return true;
}


#endif
