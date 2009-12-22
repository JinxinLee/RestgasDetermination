// -------------------------------------------------------------------------
// -----             PndMvdStripClusterBuilder header file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------


/** PndMvdStripClusterBuilder.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** Calculator class to find clusters in one channel dimension
 ** for the MVD strip sensors
 **/

#ifndef PNDMVDSIMPLESTRIPCLUSTERFINDER_H
#define PNDMVDSIMPLESTRIPCLUSTERFINDER_H


#include <vector>
#include <map>
#include <string>

#include "PndMvdDigiStrip.h"
#include "PndMvdClusterStrip.h"
#include "PndMvdStripClusterBuilder.h"
// #include "PndMvdStripCluster.h"

class PndMvdSimpleStripClusterFinder : public PndMvdStripClusterBuilder {
 public:
  PndMvdSimpleStripClusterFinder();
  PndMvdSimpleStripClusterFinder(Int_t rad);
  ~PndMvdSimpleStripClusterFinder();

  std::vector< PndMvdClusterStrip > SearchClusters();
  void SetSearchRadius(Int_t rad=0) {fRadius=rad;};

private:
  Int_t fRadius;

ClassDef(PndMvdSimpleStripClusterFinder,1);
};



#endif // PndMvdSimpleStripClusterFinder
