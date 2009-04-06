#ifndef HELPERS_H
#define HELPERSH_

#include <list>
#include <vector>
#include <string>

//static would imply functions only valid for this file
class CsGEMCluster;
class TCcluster;
class TCevent;
class TGraph;
class TH1D;
/*
  Does a cut on the difference a2-a0, a very crude cut, but nescesary for the silicons to get the multiplicity down.
  Cuts away all clusters with a2-a0 <= cut
*/
void ampDiffCut(const std::list<CsGEMCluster*> &clusterList, std::vector<TCcluster> &tcClusters, double cut, int detID, std::string alignmentFile,int& counter,double umin=-1, double umax=-1);
/*
  Does a cut on the ratio a1/a2 and a0/a2, a cut on the bananaplot
  Cuts away clusters with a1/a2>cut1 and a0/a2>cut0
*/
void ampRatioCut(const std::list<CsGEMCluster*> &clusterList, std::vector<TCcluster> &tcClusters, double cutA1A3, double cutA2A3, int detID,std::string alignmentFile, int& counter, double umin =-1, double umax=-1);

void clusterFiller(TCcluster cl, std::vector<TCcluster>& clusters, TH1D* hitPointHist,TH1D* uHist, TH1D* errHist, TGraph* graph, int& counter, bool x);



#endif
