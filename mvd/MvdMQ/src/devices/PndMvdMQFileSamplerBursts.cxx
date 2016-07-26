/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMvdMQFileSamplerBursts.cpp
 *
 * @since 2016-03-08
 * @author R. Karabowicz
 */

#include <PndMvdMQFileSamplerBursts.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "FairMQLogger.h"

#include "FairMQMessage.h"
#include "TMessage.h"
#include "FairEventHeader.h"

#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"
#include "PndSttHit.h"

using namespace std;

BOOST_CLASS_EXPORT_GUID(PndSdsDigiPixel, "PndSdsDigiPixel");
BOOST_CLASS_EXPORT_GUID(PndSdsDigiStrip, "PndSdsDigiStrip");
BOOST_CLASS_EXPORT_GUID(PndSttHit, "PndSttHit");

PndMvdMQFileSamplerBursts::PndMvdMQFileSamplerBursts()
  : FairMQDevice()
  , fRunAna(NULL)
  , fSource(NULL)
  , fOutputData()
  , fNObjects(0)
  , fMaxIndex(-1)
  , fBranchNames()
  , fFileNames()
  , fEventHeader(0)
{
}

PndMvdMQFileSamplerBursts::~PndMvdMQFileSamplerBursts()
{
	for (auto itr : fBurstBuilder)
		delete(itr.second);
	delete(fSource);
	delete(fRunAna);
}

void PndMvdMQFileSamplerBursts::InitTask()
{
  fRunAna = new FairRunAna();
  if ( fFileNames.size() > 0 ) {
    fSource = new FairFileSource(fFileNames.at(0).c_str());
    for ( unsigned int ifile = 1 ; ifile < fFileNames.size() ; ifile++ ) 
      fSource->AddFile(fFileNames.at(ifile));
  }
  fSource->Init();
  LOG(INFO) << "Going to request " << fBranchNames.size() << "  branches:";

  fSource->ActivateObject((TObject**)&fEventHeader, "EventHeader.");

   for ( unsigned int ibrn = 0 ; ibrn < fBranchNames.size() ; ibrn++ ) {

    LOG(INFO) << " requesting branch \"" << fBranchNames[ibrn].second << "\"";
    std::string branchName = fBranchNames[ibrn].second;
   	int branchStat = fSource->ActivateObject((TObject**)&fInputBranches[branchName], branchName.c_str()); // should check the status...
   	LOG(INFO) << "BranchStat: " << branchStat;
    InitBurstBuilder(branchName);
    LOG(INFO) << "Activated object \"" << fInputBranches[branchName] << "\" with name \"" << fBranchNames[ibrn].second << " for channel " << fBranchNames[ibrn].first <<"/ (" << branchStat << ")";
    fNObjects++;
  }

  if ( fMaxIndex < 0 )
    fMaxIndex = fSource->CheckMaxEventNo();
  LOG(INFO) << "Input source has " << fMaxIndex << " events.";
}

void PndMvdMQFileSamplerBursts::InitBurstBuilder(std::string branchName)
{
	PndBurstVectorBuilderBase* tmpBuilder = 0;
	if (branchName == "MVDPixelDigis")
		tmpBuilder = new PndBurstVectorBuilderT<PndSdsDigiPixel>;
	else if (branchName == "MVDStripDigis")
		tmpBuilder = new PndBurstVectorBuilderT<PndSdsDigiStrip>;
	else if (branchName == "STTHit")
		tmpBuilder = new PndBurstVectorBuilderT<PndSttHit>;

	if (tmpBuilder != 0)
		fBurstBuilder[branchName] = tmpBuilder;
}

// helper function to clean up the object holding the data after it is transported.
void free_tmessage3(void* data, void *hint)
{
 //  delete static_cast<BurstData*>(data);
}

void PndMvdMQFileSamplerBursts::Run() {
	int eventCounter = 0;

	// Check if we are still in the RUNNING state.
//	boost::this_thread::sleep(boost::posix_time::milliseconds(100000));
	while (CheckCurrentState(RUNNING)) {
		if (eventCounter != fMaxIndex){

			Int_t readEventReturn = fSource->ReadEvent(eventCounter);

			if (readEventReturn != 0)
				break;

			if (fEventHeader != 0)
				LOG(INFO) << "EventHeader: " << fEventHeader->GetRunId() << " " << fEventHeader->GetEventTime() << std::endl;

			for (auto branchItr : fInputBranches){
	//			LOG(INFO) << "ProcessingData: " << branchItr.first;

				if (branchItr.first.find(".") == std::string::npos){
					std::vector<std::vector< FairTimeStamp* > > data;
					TClonesArray* tmpArray = (TClonesArray*)branchItr.second;
//					LOG(INFO) << "Data from TCA for branch: " << branchItr.first;
//					for (int i = 0; i < tmpArray->GetEntries(); i++){
//						FairTimeStamp* tcaData = (FairTimeStamp*)tmpArray->At(i);
//						LOG(INFO) << i << " : " << tcaData->GetTimeStamp();
//					}
	//				LOG(INFO) << tmpArray->GetEntries() << std::endl;
					if (fBurstBuilder.count(branchItr.first) > 0){
						fOutputData[branchItr.first] = fBurstBuilder[branchItr.first]->ProcessData(tmpArray);
//						LOG(INFO) << "BurstBuilder: " << branchItr.first << " " << fOutputData[branchItr.first].size();
					}
				}
			}

			for (auto portIt = fPorts.begin(); portIt != fPorts.end(); portIt++){
				//FairMQParts parts;
				for (auto branchIt = fPortBranchNameMap.lower_bound(*portIt); branchIt != fPortBranchNameMap.upper_bound(*portIt); ++branchIt){
					for (auto dataIt = fOutputData[branchIt->second].begin(); dataIt != fOutputData[branchIt->second].end(); ++dataIt){
						if (dataIt->size() > 0){

//							LOG(INFO) << "SendData: Port " << branchIt->first << " Branch: " << branchIt->second << " data: " << dataIt->size() << std::endl;
//							LOG(INFO) << "data to send:";
//							for (auto itr : *dataIt)
//								LOG(INFO) << itr->GetTimeStamp();
							BurstData bData;// = new BurstData;
							bData.fData = *dataIt;
							bData.fHeader.fBranchName = branchIt->second;
							bData.fHeader.fRunID = fEventHeader->GetRunId();
							std::ostringstream obuffer;
							boost::archive::binary_oarchive OutputArchive(obuffer);
							OutputArchive << bData;
							int outputSize = obuffer.str().length();
							//unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage(outputSize));
							//memcpy(msg->GetData(), obuffer.str().c_str(), outputSize);
							unique_ptr<FairMQMessage> msg(NewMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, free_tmessage3));
							Send(msg, *portIt);
						}
					}
				}
			}
			eventCounter++;
		} else {
			LOG(INFO) << "FinishRun called!";
			for (auto branchItr : fInputBranches){
				if (branchItr.first.find(".") == std::string::npos){
					if (fBurstBuilder.count(branchItr.first) > 0){
						fOutputData[branchItr.first] = fBurstBuilder[branchItr.first]->GetLastData();
//						LOG(INFO) << "BurstBuilder: " << branchItr.first << " " << fOutputData[branchItr.first].size();
					}
				}
			}

			for (auto portIt = fPorts.begin(); portIt != fPorts.end(); portIt++){
				//FairMQParts parts;
				for (auto branchIt = fPortBranchNameMap.lower_bound(*portIt); branchIt != fPortBranchNameMap.upper_bound(*portIt); ++branchIt){
					for (auto dataIt = fOutputData[branchIt->second].begin(); dataIt != fOutputData[branchIt->second].end(); ++dataIt){
						if (dataIt->size() > 0){

//							LOG(INFO) << "SendData: Port " << branchIt->first << " Branch: " << branchIt->second << " data: " << dataIt->size() << std::endl;
//							LOG(INFO) << "data to send:";
//							for (auto itr : *dataIt)
//								LOG(INFO) << itr->GetTimeStamp();
							BurstData* bData = new BurstData;
							bData->fData = *dataIt;
							bData->fHeader.fBranchName = branchIt->second;
							bData->fHeader.fRunID = fEventHeader->GetRunId();
							std::ostringstream obuffer;
							boost::archive::binary_oarchive OutputArchive(obuffer);
							OutputArchive << *bData;
							int outputSize = obuffer.str().length();
							//unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage(outputSize));
							//memcpy(msg->GetData(), obuffer.str().c_str(), outputSize);
							unique_ptr<FairMQMessage> msg(NewMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, free_tmessage3));
							Send(msg, *portIt);
						}
					}
				}
			}
			break;
		}
	}

	LOG(INFO) << "Going out of RUNNING state.";
}

