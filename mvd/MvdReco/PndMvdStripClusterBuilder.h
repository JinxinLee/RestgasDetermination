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
#include "PndMvdClusterStrip.h"
// #include "PndMvdStripCluster.h"

typedef std::map<Int_t,Int_t> Indexpair;
typedef std::map<Int_t,Indexpair> Indextriple;
typedef std::map<std::string,std::map<SensorSide,Indextriple> > Fullmap;

class PndMvdStripClusterBuilder {
 public:
  PndMvdStripClusterBuilder();
  ~PndMvdStripClusterBuilder();

  void Reinit();

  void AddDigi(std::string detname, SensorSide side, Int_t timestamp, Int_t strip, Int_t iPoint);

  virtual std::vector< PndMvdClusterStrip > SearchClusters() = 0;

  std::vector< PndMvdClusterStrip >  GetClusters() const {return fClusters;}
  PndMvdClusterStrip GetCluster(Int_t i);
  PndMvdClusterStrip GetTopCluster(Int_t i);
  PndMvdClusterStrip GetBotCluster(Int_t i);
  std::vector< Int_t > GetTopClusterIDs() const {return fTopclusters;}
  std::vector< Int_t > GetBotClusterIDs() const {return fBotclusters;}
  std::vector< Int_t > GetLeftDigiIDs() const {return fLeftDigis;}

protected:

  void AddCluster(const std::vector< Int_t >& onecluster,SensorSide side);
  Fullmap fSortedDigis;
  std::vector< Int_t > fTopclusters;// contains index to fClusterArray
  std::vector< Int_t > fBotclusters;// contains index to fClusterArray
  std::vector< Int_t > fLeftDigis;  // contains index to the not assigned digis
  std::vector< PndMvdClusterStrip > fClusters;

private:
ClassDef(PndMvdStripClusterBuilder,1);
};



#endif // PndMvdStripClusterBuilder
