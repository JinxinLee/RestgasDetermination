/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQSorterMerger.cxx
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "PndMQSorterMerger.h"

using namespace std;

PndMQSorterMerger::PndMQSorterMerger() : fHasBoostSerialization(false)
{
	using namespace baseMQ::tools::resolve;
	// coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
	if (has_BoostSerialization<PndSdsDigiTopix4, void(boost::archive::binary_iarchive&, const unsigned int)>::value == 1)
		fHasBoostSerialization = true;
}

PndMQSorterMerger::~PndMQSorterMerger()
{
}

void PndMQSorterMerger::Run()
{
    int direction = 0;
    int numInputs = fChannels.at("data-in").size();

    // store the channel references to avoid traversing the map on every loop iteration
    const FairMQChannel& dataOutChannel = fChannels.at("data-out").at(0);
    FairMQChannel* dataInChannels[fChannels.at("data-in").size()];
    LOG(INFO) << "Number of Input Channels: " << numInputs;
    for (int i = 0; i < numInputs; ++i)
    {
        dataInChannels[i] = &(fChannels.at("data-in").at(i));
    }
    fData.resize(numInputs);
    int activeChannel = 0;
    bool switchChannel = false;
    bool channelSwitched = false;

    double oldTS = -1;


    while (CheckCurrentState(RUNNING))
    {
        for (int channelNr = 0; channelNr < numInputs; channelNr++){
 //       	LOG(INFO) << "---- Reading channel " << channelNr << " ----";
        	std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
        	if (dataInChannels[channelNr]->Receive(msg) > 0){
        		std::string msgStr(static_cast<char*>(msg->GetData()), msg->GetSize());
				std::istringstream ibuffer(msgStr);

				boost::archive::binary_iarchive InputArchive(ibuffer);

				try {
					InputArchive >> fInputData;
				}
				catch (boost::archive::archive_exception& e)
				{
					LOG(ERROR) << e.what();
				}

				fData[channelNr].insert(fData[channelNr].end(), fInputData.begin(), fInputData.end() );

	        	fInputData.clear();

//				LOG(INFO) << "fData size for channel " << channelNr << " is " << fData[channelNr].size();
//				for (auto data : fData[channelNr])
//					LOG(INFO) << data.GetTimeStamp();
				if (activeChannel == channelNr){
//					LOG(INFO) << "--- Writing channel " << activeChannel << " ---";
					if (fData[channelNr].size() > 0){
						for (std::vector<PndSdsDigiTopix4>::iterator data = fData[channelNr].begin(); data != fData[channelNr].end(); data++){
							if (data->GetTimeStamp() < 0){
								fOutputData.insert(fOutputData.end(), fData[channelNr].begin(), data);
								switchChannel = true;
								channelSwitched = true;
								fData[channelNr].erase(fData[channelNr].begin(), ++data);
//								LOG(INFO) << "Negative TS in " << channelNr << " new Data size " << fData[channelNr].size();
								break;
							}
						}
						if (switchChannel == false){
							fOutputData = fData[channelNr];
							fData[channelNr].clear();
						}

						std::ostringstream obuffer;
						boost::archive::binary_oarchive OutputArchive(obuffer);
						OutputArchive << fOutputData;
						int outputSize = obuffer.str().length();
						unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
						memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
						dataOutChannel.Send(msg2);
//						LOG(INFO) << "fOutputData.size: " << fOutputData.size();
						for (auto info : fOutputData){
//							LOG(INFO) << info.GetTimeStamp();
							if (info.GetTimeStamp() > 0 && oldTS > info.GetTimeStamp()){
								LOG(INFO) << "++++ SortingError ++++ " << oldTS << " > " << info.GetTimeStamp();
							}
							oldTS = info.GetTimeStamp();
						}
						fOutputData.clear();
						if (switchChannel == true){
							activeChannel++;
							if (activeChannel >= numInputs)
							{
								activeChannel = 0;
							}
//							LOG(INFO) << "Switch active channel to " << activeChannel;
							switchChannel = false;
						}
					}
				}
        	}
        }
    }
}
