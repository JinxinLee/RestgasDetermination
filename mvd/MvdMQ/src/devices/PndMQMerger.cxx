/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQMerger.cxx
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "PndMQMerger.h"
#include "FairMQPoller.h"

using namespace std;

PndMQMerger::PndMQMerger() : fHasBoostSerialization(false)
{
	using namespace baseMQ::tools::resolve;
	// coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
	if (has_BoostSerialization<FairTimeStamp*, void(boost::archive::binary_iarchive&, const unsigned int)>::value == 1)
		fHasBoostSerialization = true;
}

PndMQMerger::~PndMQMerger()
{
}

void PndMQMerger::free_string(void* data, void *hint)
{
	delete static_cast<std::string*>(hint);
}

void PndMQMerger::Run()
{
    int direction = 0;
    int numInputs = fChannels.at("data-in").size();
//    fRunningStatus.resize(numInputs);

    LOG(INFO) << "Number of Input Channels: " << numInputs;

//    boost::this_thread::sleep(boost::posix_time::milliseconds(10000));

    std::unique_ptr<FairMQPoller> poller(fTransportFactory->CreatePoller(fChannels, {"data-in"}));

    int nMessages;
    while (CheckCurrentState(RUNNING))
    {
    	poller->Poll(-1);
        for (int channelNr = 0; channelNr < numInputs; channelNr++){
        	LOG(INFO) << "---- Reading channel " << channelNr << " ----";
        	if (poller->CheckInput("data-in", channelNr)){
				std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());

				nMessages++;

				LOG(INFO) << "Get Data for Channel: " << channelNr;

				if (Receive(msg, "data-in", channelNr) >= 0){
					LOG(INFO) << "---Data Received--- " << msg->GetSize();

					std::string msgStr(static_cast<char*>(msg->GetData()), msg->GetSize());
//					LOG(INFO) << msgStr;
					std::istringstream ibuffer(msgStr);

					try {
						boost::archive::binary_iarchive InputArchive(ibuffer);
						InputArchive >> fInputData;
					}
					catch (boost::archive::archive_exception& e)
					{
						LOG(ERROR) << e.what();
//						continue;
					}
					LOG(INFO) << "Data: " << fInputData.fHeader.fBurstID << " " << fInputData.fHeader.fBranchName;
//					LOG(INFO) << "DataSize: " << fInputData.fData.size() << " " << fInputData.fData[0].size();
//					for (auto gapItr : fInputData.fData){
//						for (auto dataItr : gapItr)
//							LOG(INFO) << "TimeStamp: " << dataItr->GetTimeStamp();
//					}

					fInputMap[fInputData.fHeader.fBurstID][fInputData.fHeader.fBranchName] = fInputData;
					LOG(INFO) << "InputMap for BurstID: " << fInputData.fHeader.fBurstID << " " << fInputMap[fInputData.fHeader.fBurstID].size();

					if (fInputMap[fInputData.fHeader.fBurstID].size() == numInputs){		// all input channels have delivered data for this burstID
						//std::map<std::string, BurstData> dataToProcess = fInputMap[fInputData.fHeader.fBurstID];
	//					fInputMap.erase(fInputMap.find(fInputData.fHeader.fBurstID));
						ProcessData(fInputMap[fInputData.fHeader.fBurstID]);

	//					if (fOutputData != 0){
	//						std::ostringstream obuffer;
	//						boost::archive::binary_oarchive OutputArchive(obuffer);
	//						OutputArchive << fOutputData;
	//						std::string* strOutputMsg = new std::string(obuffer.str());
	//						unique_ptr<FairMQMessage> msgOut(NewMessage(const_cast<char*>(strOutputMsg.c_str()), strOutputMsg->length(), free_string, strOutputMsg));
	//						Send(msgOut, "data-out");
	//					}
					}
				}
        	}
		}
	}
}
