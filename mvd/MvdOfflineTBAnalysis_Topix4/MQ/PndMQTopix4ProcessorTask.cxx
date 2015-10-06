/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQTopix4Sink.cxx
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <PndMQTopix4ProcessorTask.h>
#include <queue>

#include "FairMQLogger.h"
#include "mrfdata_8b.h"
#include "PndSdsDigiTopix4.h"


using namespace std;

PndMQTopix4ProcessorTask::PndMQTopix4ProcessorTask()
{
}


void PndMQTopix4ProcessorTask::Exec(Option_t* opt)
{
	TMrfData_8b* message = new TMrfData_8b();
	message->setNumWords(fPayload->GetSize());
	memcpy(reinterpret_cast<u_int8_t*>(&message->regdata[0]),fPayload->GetData(), fPayload->GetSize());
//           LOG(INFO) << "Received message: \""
//                     << message->getNumWords() << " " << message->getNumBits()
//                     << "\"";
	std::vector<ULong64_t> rawArray;
	rawArray = fTopixDataReader.GetRawData(message);
	std::vector<std::vector<PndSdsDigiTopix4> > frames = fTopixDataReader.AnalyzeData(rawArray, 50);

	if (frames.size() > 0 && frames.front().size() > 0){
		ostringstream obuffer;
		boost::archive::binary_oarchive OutputArchive(obuffer);
		fPndSdsDigiTopix4Vector = frames.front();
		OutputArchive << fPndSdsDigiTopix4Vector;
		int outputSize = obuffer.str().length();
		fPayload->Rebuild(outputSize);
		memcpy(fPayload->GetData(), obuffer.str().c_str(), outputSize);
		//unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, CustomCleanup, &obuffer));
		//fChannels.at("data-out").at(0).Send(msg);
		LOG(INFO) << "Data: " << frames.front().size() << std::endl;
	}
}

PndMQTopix4ProcessorTask::~PndMQTopix4ProcessorTask()
{
}
