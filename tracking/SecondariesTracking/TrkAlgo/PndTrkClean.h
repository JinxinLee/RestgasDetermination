/** PndTrkClean
 ** 
 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKCLEAN_H
#define PNDTRKCLEAN_H 1


#include "PndGeoHandling.h"
#include "PndTrkCluster.h"
#include "PndTrkClusterList.h"
#include "PndTrkHit.h"
#include "TClonesArray.h"

class PndTrkClean : public TObject {
 public:    
  PndTrkClean();
  PndTrkClean(TClonesArray *tubearray);
  ~PndTrkClean();    

  int FindMvdLayer(int sensorID);
  PndTrkClusterList Cleanup2(PndTrkCluster *cluster);
  Bool_t CheckPairOfHits(PndTrkHit *hit1, PndTrkHit *hit2);
  int CheckSectorDistribution(PndTrkCluster *cluster);
  PndTrkCluster CleanSectors(PndTrkCluster *cluster, int sector);
  PndTrkClusterList MergeClusters(PndTrkClusterList *clusterlist);
  PndTrkClusterList Split(PndTrkCluster *cluster, std::vector< int > breakpoints);
  Bool_t SplitAtHit(PndTrkCluster *hitlist, PndTrkHit *athit, PndTrkCluster &cluster1, PndTrkCluster &cluster2);


  
 protected:
  PndGeoHandling* fGeoH;
  TClonesArray *fTubeArray;
  ClassDef(PndTrkClean,1);
};


#endif
