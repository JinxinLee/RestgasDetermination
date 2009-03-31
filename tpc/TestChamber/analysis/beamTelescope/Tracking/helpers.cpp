
#include "helpers.h"
#include "../../../src/TCtrack.h"
#include <list>
#include <iostream>
#include "../Hits.h"
#include "../../../src/TCevent.h"
#include "../../../src/TCalign.h"
#include <TH1D.h>
#include <TGraph.h>
using namespace std;
void ampDiffCut(const std::list<CsGEMCluster*> &clusterList, std::vector<TCcluster> &tcClusters, double cut, int detID,string alignmentFile, int& counter){
  TCalign* a = TCalign::getInstance("../../../alignment/AlignmentFiles/simRealAlign.txt");
  a->clear();
  a->read(alignmentFile);
  double pitch=a->getPitch(detID);
  for(std::list<CsGEMCluster*>::const_iterator it =clusterList.begin();it!=clusterList.end();it++ ){
    double amp3=(*it)->GetAmp3();
    double noise=(*it)->GetNoise();
    double amp1=(*it)->GetAmp1();
    if((amp3-amp1>cut*noise)){
      double x=((*it)->GetCenter());
      TVector3 pos(x*pitch,0,0);
      TVector3 err(((*it)->GetErrCenter())*pitch,0.1,0.1);
      double amp((*it)->GetAmp3());
      TCcluster _c(pos,err,amp,detID);
      _c.setFit(true);
      tcClusters.push_back(_c);
    }else{
      counter++;
    }
  }
}
void ampRatioCut(const std::list<CsGEMCluster*> &clusterList, std::vector<TCcluster> &tcClusters,  double cutA1A3, double cutA2A3, int detID,string alignmentFile, int& counter){
  TCalign* a = TCalign::getInstance("../../../alignment/AlignmentFiles/simRealAlign.txt");
  a->clear();
  a->read(alignmentFile);
  double pitch = a->getPitch(detID);
  for(std::list<CsGEMCluster*>::const_iterator it =clusterList.begin();it!=clusterList.end();it++ ){
    double ratioA2A3=((*it)->GetAmp2())/((*it)->GetAmp3());
    double ratioA1A3=((*it)->GetAmp1())/((*it)->GetAmp3());
    if(ratioA1A3<cutA1A3 && ratioA2A3<cutA2A3){
      
      double x=((*it)->GetCenter())*pitch;
      TVector3 pos(x,0,0);
      TVector3 err(0,0,0);
      if(detID==7||detID==8){
        err=TVector3(0.5,5.0,0.1);
      }else{
        err=TVector3(((*it)->GetErrCenter())*pitch,0.5,0.1);
      }
      double amp((*it)->GetAmp3());
      TCcluster _c(pos,err,amp,detID);
      _c.setFit(true);
      tcClusters.push_back(_c);
      //}
    }else{
      counter++;
    }
  }
}

void clusterFiller(TCcluster cl, std::vector<TCcluster>& clusters, TH1D* hitPointHist, TGraph* graph, int& counter, bool x){
  clusters.push_back(cl);
  TVector3 spacePoint = cl.posXYZ();
  double point;
  if(x){
    point=spacePoint.x();
  }else{
    point=spacePoint.y();
  }
  hitPointHist->Fill(point);
  graph->SetPoint(counter, spacePoint.z(),point);
  counter++;
}

