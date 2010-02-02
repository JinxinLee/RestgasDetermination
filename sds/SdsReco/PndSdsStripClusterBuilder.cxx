// -------------------------------------------------------------------------
// -----             PndSdsStripClusterBuilder source file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

#include "PndSdsStripClusterBuilder.h"

// includes for sort algorithms
#include <algorithm>
#include <functional>
#include <vector>
#include "TMath.h"
#include "math.h"
// enum SensorSide { kTOP, kBOTTOM };


PndSdsStripClusterBuilder::PndSdsStripClusterBuilder()
{
  // dummy
}

PndSdsStripClusterBuilder::~PndSdsStripClusterBuilder()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void PndSdsStripClusterBuilder::Reinit()
{
  fSortedDigis.clear();
  fClusters.clear();
  fTopclusters.clear();
  fBotclusters.clear();
  fLeftDigis.clear();
}

void PndSdsStripClusterBuilder::AddDigi(std::string detName, SensorSide side, Int_t timestamp, Int_t strip, Int_t iPoint)
{
    fSortedDigis[detName][side][timestamp][strip] = iPoint;
}

PndSdsClusterStrip PndSdsStripClusterBuilder::GetCluster(Int_t i)
{
  if(0<=i && fClusters.size() < i)
    return fClusters[i];
  else{
    std::cout<<"-E- PndSdsStripClusterBuilder::GetCluster: Out of bounds i="<<i<<" at size="<<fBotclusters.size()<<std::endl;
    //return; // ??? what to do?
  }
}
PndSdsClusterStrip PndSdsStripClusterBuilder::GetTopCluster(Int_t i)
{
  if(0<=i && fTopclusters.size() < i)
  return fClusters[ (fTopclusters[i]) ];
  else{
    std::cout<<"-E- PndSdsStripClusterBuilder::GetTopCluster: Out of bounds i="<<i<<" at size="<<fBotclusters.size()<<std::endl;
    //return; // ??? what to do?
  }
}
PndSdsClusterStrip PndSdsStripClusterBuilder::GetBotCluster(Int_t i)
{
  if(0<=i && fBotclusters.size() < i)
    return fClusters[ (fBotclusters[i]) ];
  else {
    std::cout<<"-E- PndSdsStripClusterBuilder::GetBotCluster: Out of bounds i="<<i<<" at size="<<fBotclusters.size()<<std::endl;
    //return; // ??? what to do?
  }
}


void PndSdsStripClusterBuilder::AddCluster(const std::vector< Int_t >& onecluster,SensorSide side)
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
  PndSdsClusterStrip cl(onecluster);
  cl.SetSensorSide(side);
  fClusters.push_back(cl);
}






ClassImp(PndSdsStripClusterBuilder);



