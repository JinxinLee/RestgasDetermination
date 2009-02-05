#include <list>
#include <vector>


//static would imply functions only valid for this file
class CsGEMCluster;
class TCcluster;
class TCevent;
/*
  Does a cut on the difference a2-a0, a very crude cut, but nescesary for the silicons to get the multiplicity down.
  Cuts away all clusters with a2-a0 <= cut
*/
void ampDiffCut(const std::list<CsGEMCluster*> &clusterList, std::vector<TCcluster> &tcClusters, double cut, int detID);
/*
  Does a cut on the ratio a1/a2 and a0/a2, a cut on the bananaplot
  Cuts away clusters with a1/a2>cut1 and a0/a2>cut0
*/
void ampRatioCut(const std::list<CsGEMCluster*> &clusterList, std::vector<TCcluster> &tcClusters, double cut0, double cut1, int detID);
