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
}

void PndMvdStripClusterBuilder::AddDigi(std::string detName, SensorSide side, Int_t strip, Int_t iPoint)
{
    fSortedDigis[detName][side][strip] = iPoint;
}


std::vector< PndMvdCluster >  PndMvdStripClusterBuilder::SearchClusters()
{
  ///  -----  search for clusters  -----
  ///  Take neighbouring fired strips.
  ///  use that std::map is a SORTED object
  fClusters.clear();
  fTopclusters.clear();
  fBotclusters.clear();
  std::vector< Int_t > onecluster;

  //fSortedDigis[sensor][side][stripnr]=digiindex 
  for (Fullmap::iterator itSensors = fSortedDigis.begin();
        itSensors != fSortedDigis.end(); ++itSensors)
  {
    for (std::map<SensorSide,Indexpair>::iterator itSide = (itSensors->second).begin();
          itSide != (itSensors->second).end(); ++itSide)
    {
//       std::cout<<"clsterfinder: ";
//       if(itSide->first == kTOP) std::cout<<"top"<<std::endl;
//       else std::cout<<"bottom"<<std::endl;

      // create a flagmap 
      Indexpair flagmap;
      for (Indexpair::iterator itStrip = (itSide->second).begin(); itStrip!= (itSide->second).end(); ++itStrip)
      {flagmap[itStrip->second]=1;}

      for (Indexpair::iterator itStrip = (itSide->second).begin();
            itStrip!= (itSide->second).end();itStrip++)
      {
        if( !(1==flagmap[itStrip->second]) ) continue;
        for(Indexpair::iterator itStrip3 = itStrip; itStrip3 != (itSide->second).end(); ++itStrip3)
        {
          if( !(1==flagmap[itStrip3->second]) ) continue;
          if( fabs(itStrip->first - itStrip3->first) > 1) continue;
          onecluster.push_back(itStrip3->second);
          flagmap[itStrip3->second]=-1;//do not reuse this digi
          itStrip=itStrip3;
//           std::cout<<"add strip "<<itStrip3->first << " from digi "<<itStrip3->second <<std::endl;
        }
//         std::cout<<" --- "<<std::endl; 
        AddCluster(onecluster,itSide->first);
        onecluster.clear();
      } // end loop itStrip
      if (onecluster.size()>0)
      {
          std::cout<<"mvd strip clusterfinder: cluster hangover? "<<onecluster.size()<<std::endl;
          AddCluster(onecluster,itSide->first);
          onecluster.clear();
      }
    }// end loop it Side
  }// end loop sensor
  return fClusters;
}

PndMvdCluster PndMvdStripClusterBuilder::GetCluster(Int_t i)
{
  if(0<=i && fClusters.size() < i)
    return fClusters[i];
}
PndMvdCluster PndMvdStripClusterBuilder::GetTopCluster(Int_t i)
{
  if(0<=i && fTopclusters.size() < i)
  return fClusters[ (fTopclusters[i]) ];
}
PndMvdCluster PndMvdStripClusterBuilder::GetBotCluster(Int_t i)
{
  if(0<=i && fBotclusters.size() < i)
  return fClusters[ (fBotclusters[i]) ];
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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// double PndMvdStripClusterBuilder::meanColumn(std::vector<PndMvdDigiPixel*>& list){
//   double mean = 0.;
//   if (list.size() == 0){
//     std::cout<<" there are no channels in list to form cluster!"<<std::endl;
//   }
//   else{
//     // loop over digis to get weighted mean
//     double sumWeights = 0.;
//     double sumChannel = 0.;
//     for (int loopList = 0; loopList<list.size(); loopList++) {
//       sumWeights += list[loopList]->getTot(); 
//       sumChannel += list[loopList]->getTot() * list[loopList]->getColumn();
//     }
//     // calc the weighted mean
//     mean = sumChannel/sumWeights;
//   }
//   return mean;
// }
// 
// 
// double PndMvdStripClusterBuilder::errMeanColumn(std::vector<PndMvdDigiPixel*>& list, double pitch){
// 
//   // only geometric resolution so far....
//   double errMean = list.size()*pitch/sqrt(12);
//   return errMean;
// }




