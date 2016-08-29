/*
 * File:   PndMQMvdPixelDigiProcessorBursts.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQMvdPixelDigiProcessorBursts::Run() with Boost transport data format
#include "PndMQMvdPixelDigiProcessorBursts.h"



void PndMQMvdPixelDigiProcessorBursts::SetParameters()
{
	fGeoPar->GetGeometry();
	fDigiPar = (PndSdsPixelDigiPar*)fParCList->FindObject("MVDPixelDigiPar");
	fTotPar = (PndSdsTotDigiPar*)fParCList->FindObject("MVDPixelTotDigiPar");
    fSensorPar = (PndSensorNamePar*)fParCList->FindObject("PndSensorNamePar");
    fSensorPar->FillMap();
    if (fGeoHandler == 0){
		fGeoHandler = new PndGeoHandling(fSensorPar);
	} else {
		fGeoHandler->SetSensorNamePar(fSensorPar);
	}

	LOG(INFO) << "Conversion Method: " << *fDigiPar;
	LOG(INFO) << "Tot Info: " << *fTotPar;
	if (fPixelMapping == 0){
		fPixelMapping = new PndMQMvdChargeWeightedPixelMapping(fGeoHandler, fDigiPar, fTotPar);
	} else {
		fPixelMapping->SetDigiPar(fDigiPar);
		fPixelMapping->SetTotPar(fTotPar);
		fPixelMapping->SetGeoHandling(fGeoHandler);
	}
}


void PndMQMvdPixelDigiProcessorBursts::ProcessData()
{

	fBurstDataOut.Reset();
	for (auto dataItr : fBurstDataIn.fData)
		fMapSorter.AddElements(dataItr);
	 fMapSorter.WriteOutAll();
	 std::vector<FairTimeStamp*> sortedData = fMapSorter.GetOutputData();

//	 for (auto dataItr: sortedData)
//		 LOG(INFO) << "DataAfterSorter " << dataItr->GetTimeStamp() ;


	 fEventBuilder.FillData(sortedData);
	 std::vector<std::vector<FairTimeStamp*> > gapData = fEventBuilder.GetSeparatedData();
	 std::vector<std::vector<FairTimeStamp*> > lastData = fEventBuilder.GetLastData();
	 gapData.insert(gapData.end(), lastData.begin(), lastData.end());
//	 for (auto eventItr : gapData)
//		 for (auto dataItr : eventItr)
//			 LOG(INFO) << "DataAfterEventBuilder: " << dataItr->GetTimeStamp();
	 if (fBurstDataIn.fHeader.fBranchName == "MVDPixelDigis"){
		 std::vector<std::vector<FairTimeStamp*> > hitsInGaps;
		 for (auto gapIter : gapData){
			 std::vector<PndSdsDigiPixel*> digiPixel;
			 std::vector<FairTimeStamp*> hits;
			 if (gapIter.size() > 0){
				// LOG(INFO) << "GapSize > 0 " << gapIter.size();
				 for (auto dataIter : gapIter){				//iterate hits in gap
					 digiPixel.push_back((PndSdsDigiPixel*) dataIter);
//								 LOG(INFO) << "DataInGap: " << *(PndSdsDigiPixel*) dataIter;
				 }
				 std::vector<std::vector< Int_t> > clusterInts = fClusterFinder.GetClusters(digiPixel);

				 for (auto clusterIter : clusterInts)		//iterate all found cluster
				 {
					 //LOG(INFO) << "ClusterSize: " << clusterIter.size();
					 std::vector<PndSdsDigiPixel*> digiInCluster;
					 for (auto digiInClusterItr : clusterIter){		//iterate digisInt in one cluster
						 int pos = digiInClusterItr;
						 digiInCluster.push_back(digiPixel[pos]);
					 }
//					 LOG(INFO) << "DigisInCluster: ";
					 if (digiInCluster.size() > 0){
//						 for (auto digiIter : digiInCluster)
//							 LOG(INFO) << digiIter->GetTimeStamp();
						 PndSdsHit* hit = new PndSdsHit(fPixelMapping->GetCluster(digiInCluster));
 						 hits.push_back(hit);
//						 LOG(INFO) << "Hit: " << hit->GetTimeStamp();
						 digiInCluster.clear();
						// LOG(INFO) << "Hit: " << *(PndSdsHit*)(*hits.rbegin());
					 }
				 }
				 hitsInGaps.push_back(hits);
			 }
		 }
		 fBurstDataOut.fHeader.fBranchName = "MVDHitsPixel";
		 fBurstDataOut.fData = hitsInGaps;
	 }
}
