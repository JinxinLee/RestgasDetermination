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
#include <boost/serialization/export.hpp>

#include "FairMQLogger.h"

#include "FairMQMessage.h"
#include "TMessage.h"
#include "FairEventHeader.h"

#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"

using namespace std;

BOOST_CLASS_EXPORT_GUID(PndSdsDigiPixel, "PndSdsDigiPixel");
BOOST_CLASS_EXPORT_GUID(PndSdsDigiStrip, "PndSdsDigiStrip");

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

	if (tmpBuilder != 0)
		fBurstBuilder[branchName] = tmpBuilder;
}

// helper function to clean up the object holding the data after it is transported.
void free_tmessage3(void* /*data*/, void *hint)
{
    delete (TMessage*)hint;
}

void PndMvdMQFileSamplerBursts::Run() {
	int eventCounter = 0;

	// Check if we are still in the RUNNING state.
	while (CheckCurrentState(RUNNING)) {
		if (eventCounter == fMaxIndex){

			break;
		}
		Int_t readEventReturn = fSource->ReadEvent(eventCounter);

		if (readEventReturn != 0)
			break;

		if (fEventHeader != 0)
			LOG(INFO) << "EventHeader: " << fEventHeader->GetRunId() << " " << fEventHeader->GetEventTime() << std::endl;

		for (auto branchItr : fInputBranches){
			LOG(INFO) << "ProcessingData: " << branchItr.first;

			if (branchItr.first.find(".") == std::string::npos){
				std::vector<std::vector< FairTimeStamp* > > data;
				TClonesArray* tmpArray = (TClonesArray*)branchItr.second;
				LOG(INFO) << tmpArray->GetEntries() << std::endl;
				if (fBurstBuilder.count(branchItr.first) > 0){
					fOutputData[branchItr.first] = fBurstBuilder[branchItr.first]->ProcessData(tmpArray);
					LOG(INFO) << "BurstVector: " << std::endl;
					for (int i = 0; i < fOutputData[branchItr.first].size(); i++){
						LOG(INFO) << i << " : " << fOutputData[branchItr.first][i].size() << std::endl;
	//					for (int j = 0; j < data[i].size(); j++){
	//						LOG(INFO) << i << "/" << j << ": " << data[i][j]->GetTimeStamp();
	//					}
					}
				}
			}
		}

		for (auto portIt = fPorts.begin(); portIt != fPorts.end(); portIt++){
			FairMQParts parts;
			for (auto branchIt = fPortBranchNameMap.lower_bound(*portIt); branchIt != fPortBranchNameMap.upper_bound(*portIt); ++branchIt){
				for (auto dataIt = fOutputData[branchIt->second].begin(); dataIt != fOutputData[branchIt->second].end(); ++dataIt){
					if (dataIt->size() > 0){
						BurstData bData;
						bData.fData = *dataIt;
						bData.fHeader.fBranchName = branchIt->second;
						bData.fHeader.fRunID = fEventHeader->GetRunId();
						LOG(INFO) << "Called1";
						std::ostringstream obuffer;
						boost::archive::binary_oarchive OutputArchive(obuffer);
//						OutputArchive << *dataIt;
						OutputArchive << bData;
						LOG(INFO) << "Called2";
						char* myData = const_cast<char*>(obuffer.str().data());
						parts.AddPart(NewMessage(myData, obuffer.str().length(), free_tmessage3));
						LOG(INFO) << "Called3";
					}
				}
			}
			if (parts.Size() > 0){
				Send(parts, *portIt);
			}
		}


//		std::ostringstream obuffer;
//		boost::archive::binary_oarchive OutputArchive(obuffer);
//		//fPndSdsDigiTopix4Vector = frames.front();
//		OutputArchive << fEventData;
//		int outputSize = obuffer.str().length();
//		unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
//		memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
//		//unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, CustomCleanup, &obuffer));
//		dataOutChannel.Send(msg2);

//		int messageIter = 0;
//		for (std::set<std::string>::iterator portIt = fPorts.begin(); portIt != fPorts.end(); portIt++){
//			FairMQParts parts;
//			TMessage* message[1000];
//			for (std::multimap<std::string, TObject*>::iterator dataIt = fInputObjects.lower_bound(*portIt); dataIt != fInputObjects.upper_bound(*portIt); ++dataIt){
//				TNamed* data = (TNamed*)(dataIt->second);
//				LOG(INFO) << *portIt << " : " << dataIt->second << " " << dataIt->second->ClassName() << " " << dataIt->second->GetName();
//				if ( strcmp(dataIt->second->ClassName(),"FairEventHeader") == 0 )
//					LOG(INFO) << "RunNumber: " << ((FairEventHeader*)dataIt->second)->GetRunId();
//
//
//				message[messageIter] = new TMessage(kMESS_OBJECT);
//				message[messageIter]->WriteObject(dataIt->second);
//				parts.AddPart(NewMessage(message[messageIter]->Buffer(), message[messageIter]->BufferSize(), free_tmessage2, message[messageIter]));
//				messageIter++;
//			}
//			LOG(INFO) << "Send!";
//			Send(parts, *portIt);
//		}

		eventCounter++;
	}

	LOG(INFO) << "Going out of RUNNING state.";
}

PndMvdMQFileSamplerBursts::~PndMvdMQFileSamplerBursts()
{
}
