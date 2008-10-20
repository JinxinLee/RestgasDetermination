#ifndef CORRECTIONS_C
#define CORRECTIONS_C

#include "../src/TCcluster.h"
#include "../src/TCtrack.h"
#include "../src/TCalign.h"
#include <algorithm>
#include <vector>
#include <set>

//v correction function from fit to profile
double f1(double x){
  return -0.0096+0.024*x;
}
double f2(double x){
  return -0.0154+0.042*x;
}
double f3(double x){
  return -0.0154+0.039*x;
}
double f4(double x){
  return -0.0597+0.152*x;
}

TCtrack vertical(TCtrack t){

  //  std::vector<int> vecInt;
  std::vector<TCcluster> tpc;//will hold clusters in TPC
  for(int i=0;i<t.nCl();++i){
    TCcluster c = t.getCl(i);
    if(c.getId()>=200 && c.getId()<=299){
		tpc.push_back(c);
      }
  }
  //std::sort(tpc.begin(),tpc.end(),TCclusterSortUVW());

  int detId;
  std::vector<TCcluster> corrClusters;
  for(unsigned int icl=0;icl<tpc.size();++icl){   

	detId=t.getCl(icl).getId();
	if(!t.getCl(icl).getFit()) continue;
	TVector3 pos = t.getCl(icl).posUVW();

	if(t.getCl(icl).nPadY()==1){
	  if(t.getCl(icl).posUVW().Y()<0.050001)continue;
	  if(t.getCl(icl).posUVW().Y()>0.7499999)continue;
	}
	if(t.getCl(icl).nPadY()==2){
	  if(t.getCl(icl).posUVW().Y()<0.1499999)continue;
	  if(t.getCl(icl).posUVW().Y()>0.6500001)continue;
	}


	if(t.getCl(icl).nPadY()==1)
	  pos.SetY(pos.Y()-f1(pos.Y()));
	if(t.getCl(icl).nPadY()==2)
	  pos.SetY(pos.Y()-f2(pos.Y()));
	if(t.getCl(icl).nPadY()==3)
	  pos.SetY(pos.Y()-f3(pos.Y()));
	if(t.getCl(icl).nPadY()>3)
	  pos.SetY(pos.Y()-f4(pos.Y()));
	  
	//pos.SetZ(pos.X());
	//pos.SetY(0.4);
	t.getCl(icl).posUVW(pos);
	  
	corrClusters.push_back(t.getCl(icl));
	
  }

  TCtrack corrTrack;
  corrTrack.addClusters(corrClusters);
  TCalign::getInstance();
  corrTrack.fit(detId);
  return corrTrack;
   
}


#endif
