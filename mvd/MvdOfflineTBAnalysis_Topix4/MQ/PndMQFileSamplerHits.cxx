/*
 * File:   PndMQFileSamplerHits.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQFileSamplerHits::Run() with Boost transport data format
#include "PndMQFileSamplerHits.h"

void PndMQFileSamplerHits::Run()
{
    if (fHasBoostSerialization)
    {
    	InitInputFile();
        int sendMsgs = 0;

        // store the channel references to avoid traversing the map on every loop iteration
        FairMQChannel& dataOutChannel = fChannels.at("data-out").at(0);

        int nEvents = fTree->GetEntries();
        do
        {
        	LOG(INFO) << "Number of events: " << nEvents;
            for (int eventNr = 0; eventNr < nEvents; eventNr++){
            	fTree->GetEntry(eventNr);
            	for (int i = 0; i < fInput->GetEntriesFast(); i++){
            		PndSdsHit* hit = static_cast<PndSdsHit*>(fInput->At(i));
            		if (!hit)
            			continue;
            		fHitVector.push_back(*hit);
            	}

            	std::ostringstream obuffer;
				boost::archive::binary_oarchive OutputArchive(obuffer);
				OutputArchive << fHitVector;
				int outputSize = obuffer.str().length();
				unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
				memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
				dataOutChannel.Send(msg2);


            	if (fHitVector.size() > 0)
            		fHitVector.clear();

            	if (!CheckCurrentState(RUNNING))
				{
					break;
				}

            }
        } while (CheckCurrentState(RUNNING));

        LOG(INFO) << "I've send " << sendMsgs << " messages!";
    }
    else
    {
        LOG(ERROR) << " Boost Serialization not ok";
    }
}
