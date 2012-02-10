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

#ifndef PNDLUMISTRIPCLUSTERBUILDER_H
#define PNDLUMISTRIPCLUSTERBUILDER_H


#include <vector>
#include <map>
#include <string>

#include "PndLumiDigi.h"
#include "PndLumiCluster.h"
// #include "PndLumiStripCluster.h"

typedef std::map<Int_t,Int_t> Indexpair;
typedef std::map<Int_t,Indexpair> Indextriple;
typedef std::map<std::string,std::map<SensorSide,Indextriple> > Fullmap;

class PndLumiStripClusterBuilder {
 public:
  PndLumiStripClusterBuilder();
  ~PndLumiStripClusterBuilder();

  void Reinit();

  void AddDigi(std::string detname, SensorSide side, Int_t timestamp, Int_t strip, Int_t iPoint);

  virtual std::vector< PndLumiCluster > SearchClusters() = 0;

  std::vector< PndLumiCluster >  GetClusters() const {return fClusters;}
  PndLumiCluster GetCluster(Int_t i);
  PndLumiCluster GetTopCluster(Int_t i);
  PndLumiCluster GetBotCluster(Int_t i);
  std::vector< Int_t > GetTopClusterIDs() const {return fTopclusters;}
  std::vector< Int_t > GetBotClusterIDs() const {return fBotclusters;}
  std::vector< Int_t > GetLeftDigiIDs() const {return fLeftDigis;}

protected:

  void AddCluster(const std::vector< Int_t >& onecluster,SensorSide side);
  Fullmap fSortedDigis;
  std::vector< Int_t > fTopclusters;// contains index to fClusterArray
  std::vector< Int_t > fBotclusters;// contains index to fClusterArray
  std::vector< Int_t > fLeftDigis;  // contains index to the not assigned digis
  std::vector< PndLumiCluster > fClusters;

private:
ClassDef(PndLumiStripClusterBuilder,1);
};



#endif // PndLumiStripClusterBuilder
