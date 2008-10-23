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

#ifndef PNDMVDSTRIPCLUSTERBUILDER_H
#define PNDMVDSTRIPCLUSTERBUILDER_H


#include <vector>
#include <map>
#include <string>

#include "PndMvdDigiStrip.h"
#include "PndMvdCluster.h"
// #include "PndMvdStripCluster.h"

typedef std::map<Int_t,Int_t> Indexpair;
typedef std::map<std::string,std::map<SensorSide,Indexpair> > Fullmap;

class PndMvdStripClusterBuilder {
 public:
  PndMvdStripClusterBuilder();
  ~PndMvdStripClusterBuilder();

  void Reinit();

  void AddDigi(std::string detname, SensorSide side, Int_t strip, Int_t iPoint);

  std::vector< PndMvdCluster > SearchClusters();

  // getter
//   std::vector< std::vector<Int_t> > GetClusters() const {return fClusters;}
  std::vector< PndMvdCluster >  GetClusters() const {return fClusters;}
  PndMvdCluster GetCluster(Int_t i);
  PndMvdCluster GetTopCluster(Int_t i);
  PndMvdCluster GetBotCluster(Int_t i);
  std::vector< Int_t > GetTopClusterIDs() const {return fTopclusters;}
  std::vector< Int_t > GetBotClusterIDs() const {return fBotclusters;}
  std::vector< Int_t > GetLeftDigiIDs() const {return fLeftDigis;}

private:

  void AddCluster(const std::vector< Int_t >& onecluster,SensorSide side);
  Fullmap fSortedDigis;
  std::vector< Int_t > fTopclusters;// contains index to fClusterArray
  std::vector< Int_t > fBotclusters;// contains index to fClusterArray
  std::vector< Int_t > fLeftDigis;  // contains index to the not assigned digis
  std::vector< PndMvdCluster > fClusters;


ClassDef(PndMvdStripClusterBuilder,1);
};



#endif // PndMvdStripClusterBuilder
