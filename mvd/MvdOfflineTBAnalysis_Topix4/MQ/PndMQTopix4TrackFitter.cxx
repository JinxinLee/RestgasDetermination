/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQTopix4TrackFitter.cxx
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <PndMapSorterTpl.h>
#include "PndMQTopix4TrackFitter.h"
#include "PndMQStatus.h"

#include "baseMQtools.h"

#include "FairMQLogger.h"

#include <TH2.h>
#include <TCanvas.h>


using namespace std;

PndMQTopix4TrackFitter::PndMQTopix4TrackFitter() : fHasBoostSerialization(false), fEventNr(0), fNtracksPerEvent{ {0,0,0,0,0,0}}
{
	//gSystem->ResetSignal(kSigInterrupt);
	//gSystem->ResetSignal(kSigTermination);

	using namespace baseMQ::tools::resolve;
	// coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
	if (has_BoostSerialization<PndSdsHit, void(boost::archive::binary_iarchive&, const unsigned int)>::value == 1)
		fHasBoostSerialization = true;
}

//void PndMQTopix4TrackFitter::CustomCleanup(void *data, void *object)
//{
//    delete (string*)object;
//}

void PndMQTopix4TrackFitter::Run()
{
	LOG(INFO) << "Boost Serialization "<< fHasBoostSerialization;
	if (fHasBoostSerialization){
		FairMQChannel& dataInChannel = fChannels.at("data-in").at(0);
		FairMQChannel& dataOutChannel = fChannels.at("data-out").at(0);

		int receivedMsgs = 0;
		PndMQStraightLineTrackFinder trackFinder;


		while (CheckCurrentState(RUNNING))
		{
			FairMQMessage* header = fTransportFactory->CreateMessage();
			FairMQMessage* msg = fTransportFactory->CreateMessage();

			if (dataInChannel.Receive(header) > 0)
			{
				int status = *(static_cast<int*>(header->GetData()));

				if (dataInChannel.ExpectsAnotherPart())
				{
					if (dataInChannel.Receive(msg)){
						string msgStr(static_cast<char*>(msg->GetData()), msg->GetSize());
						istringstream ibuffer(msgStr);

						boost::archive::binary_iarchive InputArchive(ibuffer);

						try {
							InputArchive >> fTopixData;
						}
						catch (boost::archive::archive_exception& e)
						{
							LOG(ERROR) << e.what();
						}

						// LOG(INFO) << "TopixData: " << fTopixData.size();
		//				for (auto iter : fTopixData){
							// LOG(INFO) << iter.GetTimeStamp();
		//				}

						if (fTopixData.size() > 0){
							for (auto eventData : fTopixData){
								std::vector<PndSimpleTrack> trackData = trackFinder.FindTracks(eventData, fEventNr);
								if (trackData.size() < 5){
									fNtracksPerEvent[trackData.size()]++;
								} else {
									fNtracksPerEvent[5]++;
								}
								//LOG(INFO) << "TrackData: " << trackData.size();
								//for(auto track : trackData)
								//	LOG(INFO) << "PndMQTopix4TrackFinder Track: " << track.GetTrackCand().GetNHits();
								if (trackData.size() > 0)
									fOutputData.push_back(trackData);
								fEventNr++;
							}
							unique_ptr<FairMQMessage> headerCpy(fTransportFactory->CreateMessage(sizeof(int)));
							headerCpy->Copy(header);
							dataOutChannel.SendPart(headerCpy);


							std::ostringstream obuffer;
							boost::archive::binary_oarchive OutputArchive(obuffer);
							OutputArchive << fOutputData;
							int outputSize = obuffer.str().length();
							unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
							memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
							dataOutChannel.Send(msg2);

							//LOG(INFO) << "Data: " << fTopixData.size() << " " << timeOfLast;
							// LOG(INFO) << "Output: " << fOutputData.size() << " timeOfLast: " << timeOfLast;
		//					for(auto itr : fOutputData)
								// LOG(INFO) << itr.GetTimeStamp();

							fTopixData.clear();
							fOutputData.clear();
						}
						delete(msg);
						delete (header);
					}
				}
				receivedMsgs++;
				if (receivedMsgs % 100 == 0)
					LOG(INFO) << receivedMsgs << " : Number of Tracks per Event: " << fNtracksPerEvent[0]
							<< "/" << fNtracksPerEvent[1] << "/"<< fNtracksPerEvent[2]
							<< "/"<< fNtracksPerEvent[3] << "/"<< fNtracksPerEvent[4] << "/" << fNtracksPerEvent[5];
				// LOG(INFO) << "Received Message: " << receivedMsgs++ << " Size: " << msg->GetSize();
				if (status == PndMQStatus::STOP){
					LOG(INFO) << "STOP-Signal Received!";

					unique_ptr<FairMQMessage> headerCpy(fTransportFactory->CreateMessage(sizeof(int)));
					headerCpy->Copy(header);
					dataOutChannel.Send(headerCpy);
				}
			}
		}
	}
}

PndMQTopix4TrackFitter::~PndMQTopix4TrackFitter()
{
}
