
#include "helpers.h"
#include "../../../src/TCtrack.h"
#include <list>
#include <iostream>
#include "../Hits.h"
#include "../../../src/TCevent.h"
#include "../../../src/TCalign.h"
using namespace std;
void ampDiffCut(const std::list<CsGEMCluster*> &clusterList, std::vector<TCcluster> &tcClusters, double cut, int detID){
  TCalign* a = TCalign::getInstance("../../../alignment/AlignmentFiles/simRealAlign.txt");
  a->clear();
  a->read("../../../alignment/AlignmentFiles/simRealAlign.txt");
  double pitch=a->getPitch(detID);
  for(std::list<CsGEMCluster*>::const_iterator it =clusterList.begin();it!=clusterList.end();it++ ){
    double amp3=(*it)->GetAmp3();
    double amp1=(*it)->GetAmp1();
    if(amp3-amp1>cut){
      double x=((*it)->GetCenter())*pitch;
      //cout<<"x "<<x<<" pitch "<<pitch<<endl;
      TVector3 pos(x,0,0);
      TVector3 err(((*it)->GetErrCenter())*pitch,1,1);
      double amp((*it)->GetAmp3());
      TCcluster _c(pos,err,amp,detID);
      _c.setFit(true);
      tcClusters.push_back(_c);
    }
  }
}
void ampRatioCut(const std::list<CsGEMCluster*> &clusterList, std::vector<TCcluster> &tcClusters,  double cut0, double cut1, int detID){
  TCalign* a = TCalign::getInstance("../../../alignment/AlignmentFiles/simRealAlign.txt");
  a->clear();
  a->read("../../../alignment/AlignmentFiles/simRealAlign.txt");
  double pitch = a->getPitch(detID);
  for(std::list<CsGEMCluster*>::const_iterator it =clusterList.begin();it!=clusterList.end();it++ ){
    double ratio1=((*it)->GetAmp2())/((*it)->GetAmp3());
    double ratio2=((*it)->GetAmp1())/((*it)->GetAmp3());
    if(ratio1<cut0 && ratio2<cut1){

      double x=((*it)->GetCenter())*pitch;
      //cout<<"x "<<x<<" pitch "<<pitch<<endl;
      TVector3 pos(x,0,0);
      TVector3 err(((*it)->GetErrCenter())*pitch,5,1);
      double amp((*it)->GetAmp3());
      //        pedestalRMS=_c.GetNoise();
      TCcluster _c(pos,err,amp,detID);
      _c.setFit(true);
      tcClusters.push_back(_c);
    }
  }
}
