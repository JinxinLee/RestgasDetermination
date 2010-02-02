// -------------------------------------------------------------------------
// -----             PndSdsStripClusterBuilder header file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------


/** PndSdsStripClusterBuilder.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** Calculator class to find clusters in one channel dimension
 ** for the MVD strip sensors
 **/

#ifndef PNDSDSSIMPLESTRIPCLUSTERFINDER_H
#define PNDSDSSIMPLESTRIPCLUSTERFINDER_H


#include <vector>
#include <map>
#include <string>

#include "PndSdsDigiStrip.h"
#include "PndSdsCluster.h"
#include "PndSdsStripClusterBuilder.h"
// #include "PndSdsStripCluster.h"

class PndSdsSimpleStripClusterFinder : public PndSdsStripClusterBuilder {
 public:
  PndSdsSimpleStripClusterFinder();
  PndSdsSimpleStripClusterFinder(Int_t rad);
  ~PndSdsSimpleStripClusterFinder();

  std::vector< PndSdsCluster > SearchClusters();
  void SetSearchRadius(Int_t rad=0) {fRadius=rad;};

private:
  Int_t fRadius;

ClassDef(PndSdsSimpleStripClusterFinder,1);
};



#endif // PndSdsSimpleStripClusterFinder
