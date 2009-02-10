// -------------------------------------------------------------------------
// -----             PndMvdStripClusterBuilder header file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------


/** PndMvdStripCluster.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 *@author L.Ackermann
 **
 ** Calculator class to find clusters in channel dimension and time dimension
 ** for the MVD strip sensors
 **/

#ifndef PNDMVDSTRIPCLUSTERFINDER_H
#define PNDMVDSTRIPCLUSTERFINDER_H


#include <vector>
#include <map>
#include <string>

#include "PndMvdDigiStrip.h"
#include "PndMvdCluster.h"
#include "PndMvdStripClusterBuilder.h"
// #include "PndMvdStripCluster.h"


class PndMvdStripClusterFinder : public PndMvdStripClusterBuilder {
 public:
  PndMvdStripClusterFinder();
  PndMvdStripClusterFinder(Int_t NrofmissedChannels, Int_t NrofmissedTimestamps);
  ~PndMvdStripClusterFinder();

  std::vector< PndMvdCluster > SearchClusters();


private:

void ClusterIterator(Int_t newCh, Int_t newFrameID, std::map<Int_t, std::map<Int_t, std::vector<Int_t> > >& hitmap, std::vector< Int_t >& onecluster);
  Int_t fNmCh;
  Int_t fNmTS;

ClassDef(PndMvdStripClusterFinder,1);
};



#endif // PndMvdSimpleStripClusterFinder
