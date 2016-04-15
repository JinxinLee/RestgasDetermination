/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMvdMQFileSampler.cpp
 *
 * @since 2016-03-08
 * @author R. Karabowicz
 */

#include <PndMvdMQFileSampler.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "FairMQLogger.h"

#include "FairMQMessage.h"
#include "TMessage.h"

using namespace std;

PndMvdMQFileSampler::PndMvdMQFileSampler()
  : FairMQDevice()
  , fRunAna(NULL)
  , fSource(NULL)
  , fInputObjects()
  , fNObjects(0)
  , fMaxIndex(-1)
  , fBranchNames()
  , fFileNames()
{
}

void PndMvdMQFileSampler::InitTask()
{
  fRunAna = new FairRunAna();
  if ( fFileNames.size() > 0 ) {
    fSource = new FairFileSource(fFileNames.at(0).c_str());
    for ( unsigned int ifile = 1 ; ifile < fFileNames.size() ; ifile++ ) 
      fSource->AddFile(fFileNames.at(ifile));
  }
  fSource->Init();
  LOG(INFO) << "Going to request " << fBranchNames.size() << "  branches:";
  for ( unsigned int ibrn = 0 ; ibrn < fBranchNames.size() ; ibrn++ ) {
    LOG(INFO) << " requesting branch \"" << fBranchNames[ibrn].second << "\"";
    TObject* temp = 0;
    int branchStat = fSource->ActivateObject((TObject**)&temp,fBranchNames[ibrn].second.c_str()); // should check the status...
    LOG(INFO) << "BranchStat: " << branchStat;
    if ( temp ) {
    	fInputObjects.insert(std::pair<std::string, TObject*>(fBranchNames[ibrn].first, temp));
      LOG(INFO) << "Activated object \"" << temp << "\" with name \"" << fBranchNames[ibrn].second << " for channel " << fBranchNames[ibrn].first <<"/ (" << branchStat << ")";
      fNObjects++;
    }
  }
  if ( fMaxIndex < 0 )
    fMaxIndex = fSource->CheckMaxEventNo();
  LOG(INFO) << "Input source has " << fMaxIndex << " events.";
}

// helper function to clean up the object holding the data after it is transported.
void free_tmessage2(void* /*data*/, void *hint)
{
    delete (TMessage*)hint;
}

void PndMvdMQFileSampler::Run() {
	int eventCounter = 0;

	// Check if we are still in the RUNNING state.
	while (CheckCurrentState(RUNNING)) {
		if (eventCounter == fMaxIndex)
			break;

		Int_t readEventReturn = fSource->ReadEvent(eventCounter);

		if (readEventReturn != 0)
			break;

		int messageIter = 0;
		for (std::set<std::string>::iterator portIt = fPorts.begin(); portIt != fPorts.end(); portIt++){
			FairMQParts parts;
			TMessage* message[1000];
			for (std::multimap<std::string, TObject*>::iterator dataIt = fInputObjects.lower_bound(*portIt); dataIt != fInputObjects.upper_bound(*portIt); ++dataIt){
				TNamed* data = (TNamed*)(dataIt->second);
				data->SetName(dataIt->first.c_str());
				LOG(INFO) << *portIt << " : " << dataIt->second->GetName();
				message[messageIter] = new TMessage(kMESS_OBJECT);
				message[messageIter]->WriteObject(dataIt->second);
				parts.AddPart(NewMessage(message[messageIter]->Buffer(), message[messageIter]->BufferSize(), free_tmessage2, message[messageIter]));
				messageIter++;
			}
			LOG(INFO) << "Send!";
			Send(parts, *portIt);
		}

		eventCounter++;
	}

	LOG(INFO) << "Going out of RUNNING state.";
}

PndMvdMQFileSampler::~PndMvdMQFileSampler()
{
}
