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

#include "PndMQHitEventDevice.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>


#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "mrfdata_8b.h"
#include "PndSdsDigiTopix4.h"


using namespace std;

PndMQHitEventDevice::PndMQHitEventDevice() : fHasBoostSerialization(false)
{
	using namespace baseMQ::tools::resolve;
	bool checkOutputClass = false;

	if (is_same<boost::archive::binary_oarchive, boost::archive::binary_oarchive>::value)
	{
		if (has_BoostSerialization<PndSdsDigiTopix4, void(boost::archive::binary_oarchive&, const unsigned int)>::value == 1)
		{
			checkOutputClass = true;
			fHasBoostSerialization = true;
		}
	}
	LOG(INFO) << "HasBoostSerialization: " << fHasBoostSerialization;
}

PndMQHitEventDevice::~PndMQHitEventDevice()
{
}


void PndMQHitEventDevice::Run()
{
	 int numInputs = fChannels.at("data-in").size();

	// store the channel references to avoid traversing the map on every loop iteration
	const FairMQChannel& dataOutChannel = fChannels.at("data-out").at(0);
	FairMQChannel* dataInChannels[fChannels.at("data-in").size()];
	LOG(INFO) << "Number of Input Channels: " << numInputs;
	for (int i = 0; i < numInputs; ++i)
	{
		dataInChannels[i] = &(fChannels.at("data-in").at(i));
	}
	fDataFromChannels.resize(numInputs);

//	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	std::vector<int> fillLevel(numInputs,0);

	while (CheckCurrentState(RUNNING))
	{
		for (int channelNr = 0; channelNr < numInputs; channelNr++){
//			boost::this_thread::sleep(boost::posix_time::milliseconds(200));
//        	LOG(INFO) << "---- Reading channel " << channelNr << " ----";
			if (fillLevel[channelNr] == 0){
				std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
				if (dataInChannels[channelNr]->Receive(msg) > 0){
					std::string msgStr(static_cast<char*>(msg->GetData()), msg->GetSize());
					std::istringstream ibuffer(msgStr);

					boost::archive::binary_iarchive InputArchive(ibuffer);

					try {
						InputArchive >> fHitData;
					}
					catch (boost::archive::archive_exception& e)
					{
						LOG(ERROR) << e.what();
					}
					fDataFromChannels[channelNr].push_back(fHitData);
					//LOG(INFO) << "Data in channel " << fDataFromChannels[channelNr].size();
					fHitData.clear();
				}
			}
		}
		fBuilder.AddData(fDataFromChannels);
		std::vector<std::vector<PndSdsHit> > eventData = fBuilder.GetEvents();
		fillLevel = fBuilder.GetInputDataLevel();
//		LOG(INFO) << "EventData.size() " << eventData.size();
//		for (auto eventIter : eventData){
//			LOG(INFO) << "Event";
//			for (auto dataIter : eventIter){
//				LOG(INFO) << dataIter.GetSensorID() << " " << dataIter.GetTimeStamp();
//			}
//		}

		for (int channelNr = 0; channelNr < fDataFromChannels.size(); channelNr++){
			fDataFromChannels[channelNr].clear();
		}
	}
}

void PndMQHitEventDevice::SetProperty(const int key, const string& value)
{
    switch (key)
    {
        default:
            FairMQDevice::SetProperty(key, value);
            break;
    }
}

string PndMQHitEventDevice::GetProperty(const int key, const string& default_ /*= ""*/)
{
    switch (key)
    {
        default:
            return FairMQDevice::GetProperty(key, default_);
    }
}

void PndMQHitEventDevice::SetProperty(const int key, const int value)
{
    switch (key)
    {
	default:
		FairMQDevice::SetProperty(key, value);
		break;
    }
}

int PndMQHitEventDevice::GetProperty(const int key, const int default_ /*= 0*/)
{
    switch (key)
    {
        default:
            return FairMQDevice::GetProperty(key, default_);
    }
}


