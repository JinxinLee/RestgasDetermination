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

#ifndef PNDLUMISIMPLESTRIPCLUSTERFINDER_H
#define PNDLUMISIMPLESTRIPCLUSTERFINDER_H


#include <vector>
#include <map>
#include <string>

#include "PndLumiDigi.h"
#include "PndLumiCluster.h"
#include "PndLumiStripClusterBuilder.h"
// #include "PndMvdStripCluster.h"

class PndLumiSimpleStripClusterFinder : public PndLumiStripClusterBuilder {
 public:
  PndLumiSimpleStripClusterFinder();
  PndLumiSimpleStripClusterFinder(Int_t rad);
  ~PndLumiSimpleStripClusterFinder();

  std::vector< PndLumiCluster > SearchClusters();
  void SetSearchRadius(Int_t rad=0) {fRadius=rad;};

private:
  Int_t fRadius;

ClassDef(PndLumiSimpleStripClusterFinder,1);
};



#endif // PndLumiSimpleStripClusterFinder
