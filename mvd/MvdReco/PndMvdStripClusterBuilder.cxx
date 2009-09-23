// -------------------------------------------------------------------------
// -----             PndMvdStripClusterBuilder source file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

#include "PndMvdStripClusterBuilder.h"

// includes for sort algorithms
#include <algorithm>
#include <functional>
#include <vector>
#include "TMath.h"
#include "math.h"
// enum SensorSide { kTOP, kBOTTOM };


PndMvdStripClusterBuilder::PndMvdStripClusterBuilder()
{
  // dummy
}

PndMvdStripClusterBuilder::~PndMvdStripClusterBuilder()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void PndMvdStripClusterBuilder::Reinit()
{
  fSortedDigis.clear();
  fClusters.clear();
  fTopclusters.clear();
  fBotclusters.clear();
  fLeftDigis.clear();
}

void PndMvdStripClusterBuilder::AddDigi(std::string detName, SensorSide side, Int_t timestamp, Int_t strip, Int_t iPoint)
{
    fSortedDigis[detName][side][timestamp][strip] = iPoint;
}

PndMvdCluster PndMvdStripClusterBuilder::GetCluster(Int_t i)
{
  if(0<=i && fClusters.size() < i)
    return fClusters[i];
  else{
    std::cout<<"-E- PndMvdStripClusterBuilder::GetCluster: Out of bounds i="<<i<<" at size="<<fBotclusters.size()<<std::endl;
    //return; // ??? what to do?
  }
}
PndMvdCluster PndMvdStripClusterBuilder::GetTopCluster(Int_t i)
{
  if(0<=i && fTopclusters.size() < i)
  return fClusters[ (fTopclusters[i]) ];
  else{
    std::cout<<"-E- PndMvdStripClusterBuilder::GetTopCluster: Out of bounds i="<<i<<" at size="<<fBotclusters.size()<<std::endl;
    //return; // ??? what to do?
  }
}
PndMvdCluster PndMvdStripClusterBuilder::GetBotCluster(Int_t i)
{
  if(0<=i && fBotclusters.size() < i)
    return fClusters[ (fBotclusters[i]) ];
  else {
    std::cout<<"-E- PndMvdStripClusterBuilder::GetBotCluster: Out of bounds i="<<i<<" at size="<<fBotclusters.size()<<std::endl;
    //return; // ??? what to do?
  }
}


void PndMvdStripClusterBuilder::AddCluster(const std::vector< Int_t >& onecluster,SensorSide side)
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
  PndMvdCluster cl(onecluster);
  cl.SetSensorSide(side);
  fClusters.push_back(cl);
}






ClassImp(PndMvdStripClusterBuilder);



