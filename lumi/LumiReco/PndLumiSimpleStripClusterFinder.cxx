// -------------------------------------------------------------------------
// -----             PndMvdSimpleStripClusterFinder source file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

#include "PndLumiSimpleStripClusterFinder.h"

// includes for sort algorithms
#include <algorithm>
#include <functional>
#include <vector>
#include "TMath.h"
#include "math.h"

// enum SensorSide { kTOP, kBOTTOM };


PndLumiSimpleStripClusterFinder::PndLumiSimpleStripClusterFinder(): PndLumiStripClusterBuilder()
{
  // dummy
	fRadius = 1;
}


PndLumiSimpleStripClusterFinder::PndLumiSimpleStripClusterFinder(Int_t rad): PndLumiStripClusterBuilder()
{
	fRadius = rad;
}


PndLumiSimpleStripClusterFinder::~PndLumiSimpleStripClusterFinder()
{
}


std::vector< PndLumiCluster >  PndLumiSimpleStripClusterFinder::SearchClusters()
{
	///  -----  search for clusters  -----
	///  Take neighbouring fired strips.

	fClusters.clear();
	fTopclusters.clear();
	fBotclusters.clear();
	std::vector< Int_t > onecluster;
	Indexpair::iterator tempStrip;

	for (Fullmap::iterator itSensors = fSortedDigis.begin();
		itSensors != fSortedDigis.end(); ++itSensors){		// iterate over sensor

		for (std::map<SensorSide,Indextriple>::iterator itSide = (itSensors->second).begin();
		itSide != (itSensors->second).end(); ++itSide){		// iterate over sensor side

			for (Indextriple::iterator itTime = (itSide->second).begin();
			itTime != (itSide->second).end(); ++itTime){	// iterate over timestamp

				// create a flagmap
				Indexpair flagmap;

				for (Indexpair::iterator itStrip = (itTime->second).begin();
				itStrip!= (itTime->second).end(); ++itStrip){	// iterate over channel

					flagmap[itStrip->second]=1;

					}

				for (Indexpair::iterator itStrip = (itTime->second).begin();
				itStrip!= (itTime->second).end();itStrip++){

					tempStrip=itStrip;

					if( !(1==flagmap[itStrip->second]) ) continue;

					for(Indexpair::iterator itStrip3 = itStrip; itStrip3 != (itTime->second).end(); ++itStrip3){

						if( !(1==flagmap[itStrip3->second]) ) continue;

						if( fabs(tempStrip->first - itStrip3->first) > fRadius ) continue;

						onecluster.push_back(itStrip3->second);
						flagmap[itStrip3->second]=-1;//do not reuse this digi
						tempStrip=itStrip3;

					}

					AddCluster(onecluster,itSide->first);
					onecluster.clear();

				} // end loop itStrip

				if (onecluster.size()>0){
					std::cout<<"-w- PndLumiSimpleStripClusterFinder::SearchClusters(): cluster hangover? "<<onecluster.size()<<std::endl;
					AddCluster(onecluster,itSide->first);
					onecluster.clear();
				}
				for (Indexpair::iterator itflag = flagmap.begin(); itflag!= flagmap.end(); ++itflag){

					if (1==itflag->second) fLeftDigis.push_back(itflag->first);

				}

			} // end loop itTime

		}// end loop it Side

	}// end loop sensor

  return fClusters;
}

ClassImp(PndLumiSimpleStripClusterFinder);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// double PndLumiSimpleStripClusterFinder::meanColumn(std::vector<PndLumiDigiPixel*>& list){
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
// double PndLumiSimpleStripClusterFinder::errMeanColumn(std::vector<PndLumiDigiPixel*>& list, double pitch){
//
//   // only geometric resolution so far....
//   double errMean = list.size()*pitch/sqrt(12);
//   return errMean;
// }




