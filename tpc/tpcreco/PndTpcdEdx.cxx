#include"PndTpcdEdx.h"
#include<iostream>
#include<cassert>
#include<cmath>
#include<algorithm>

bool dedxSort(PAIR p1,PAIR p2){
  //TODO: improve!
  if(fabs(p1.second)<1.E-10) return false;
  if(fabs(p2.second)<1.E-10) return true;
  return p1.first/p1.second < p2.first/p2.second;
}

double PndTpcdEdx::simpleMean(){
  double sumdE=0.;
  double sumdx=0.;
  for(unsigned int i=0;i<_data.size();++i){
	sumdE+=_data.at(i).first;
	sumdx+=_data.at(i).second;
  }
  if(std::fabs(sumdx)>1.E-10) return sumdE/sumdx;
  else return 0;
}

double PndTpcdEdx::truncMean(double trash){
  int N = _data.size();

  //std::cout<<"N="<<N<<std::endl;
  
  sort(_data.begin(),_data.end(),dedxSort);

  double cut=trash;  //throw away this percentage of entries (high tail)

  int intCut=(int)(cut*N);

  double sum=0.;
  int NinSum=0;
  for(int i=0;i<(N-intCut);++i){
	if(fabs(_data.at(i).second)>1.E-10){
	  sum+=_data.at(i).first/_data.at(i).second;
	  ++NinSum;
	}
  }
  //std::cout << "NinSum " << NinSum  << std::endl;
  if(NinSum>0)return sum/NinSum;
  return -1.;
}

ClassImp(PndTpcdEdx)
