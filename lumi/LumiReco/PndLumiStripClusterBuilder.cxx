// -------------------------------------------------------------------------
// -----             PndMvdStripClusterBuilder source file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

#include "PndLumiStripClusterBuilder.h"

// includes for sort algorithms
#include <algorithm>
#include <functional>
#include <vector>
#include "TMath.h"
#include "math.h"
// enum SensorSide { kTOP, kBOTTOM };


PndLumiStripClusterBuilder::PndLumiStripClusterBuilder()
{
  // dummy
}

PndLumiStripClusterBuilder::~PndLumiStripClusterBuilder()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void PndLumiStripClusterBuilder::Reinit()
{
  fSortedDigis.clear();
  fClusters.clear();
  fTopclusters.clear();
  fBotclusters.clear();
  fLeftDigis.clear();
}

void PndLumiStripClusterBuilder::AddDigi(std::string detName, SensorSide side, Int_t timestamp, Int_t strip, Int_t iPoint)
{
    fSortedDigis[detName][side][timestamp][strip] = iPoint;
}

PndLumiCluster PndLumiStripClusterBuilder::GetCluster(Int_t i)
{
  if(0<=i && fClusters.size() < i)
    return fClusters[i];
  else{
    std::cout<<"-E- PndLumiStripClusterBuilder::GetCluster: Out of bounds i="<<i<<" at size="<<fBotclusters.size()<<std::endl;
    //return; // ??? what to do?
  }
}
PndLumiCluster PndLumiStripClusterBuilder::GetTopCluster(Int_t i)
{
  if(0<=i && fTopclusters.size() < i)
  return fClusters[ (fTopclusters[i]) ];
  else{
    std::cout<<"-E- PndLumiStripClusterBuilder::GetTopCluster: Out of bounds i="<<i<<" at size="<<fBotclusters.size()<<std::endl;
    //return; // ??? what to do?
  }
}
PndLumiCluster PndLumiStripClusterBuilder::GetBotCluster(Int_t i)
{
  if(0<=i && fBotclusters.size() < i)
    return fClusters[ (fBotclusters[i]) ];
  else {
    std::cout<<"-E- PndLumiStripClusterBuilder::GetBotCluster: Out of bounds i="<<i<<" at size="<<fBotclusters.size()<<std::endl;
    //return; // ??? what to do?
  }
}


void PndLumiStripClusterBuilder::AddCluster(const std::vector< Int_t >& onecluster,SensorSide side)
{
  Int_t clindex = fClusters.size();
  if(side == kBOTTOM)
  {
    fBotclusters.push_back(clindex);
//     std::cout<<"Adding Cluster number "<<clindex<<" to the bottom list."<<std::endl;
  } else if(side == kTOP)
  {
    fTopclusters.push_back(clindex);
//     std::cout<<"Adding Cluster number "<<clindex<<" to the top list."<<std::endl;
  }
  PndLumiCluster cl(onecluster);
  cl.SetSensorSide(side);
  fClusters.push_back(cl);
}
ClassImp(PndLumiStripClusterBuilder);



