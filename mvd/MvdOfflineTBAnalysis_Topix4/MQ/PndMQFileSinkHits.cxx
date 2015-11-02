/*
 * File:   PndMQFileSinkHits.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQFileSinkHits::Run() with Boost transport data format
#include "PndMQFileSinkHits.h"

void PndMQFileSinkHits::Run()
{
    if (fHasBoostSerialization)
    {
        int receivedMsgs = 0;

        // store the channel references to avoid traversing the map on every loop iteration
        FairMQChannel& dataInChannel = fChannels.at("data-in").at(0);

        while (CheckCurrentState(RUNNING))
        {
            FairMQMessage* msg = fTransportFactory->CreateMessage();

            if (dataInChannel.Receive(msg) > 0)
            {
                receivedMsgs++;
                string msgStr(static_cast<char*>(msg->GetData()), msg->GetSize());
                istringstream ibuffer(msgStr);
                boost::archive::binary_iarchive InputArchive(ibuffer);
                LOG(INFO) << "Received Message: " << receivedMsgs;
                try
                {
                    InputArchive >> fHitVector;
                }
                catch (boost::archive::archive_exception& e)
                {
                    LOG(ERROR) << e.what();
                }



                bool dataAboveTimeThreshold = false;
                double timeStampThreshold = 30000000000;
                for (auto eventIter : fHitVector){
                	fOutput->Delete();
                	int numInput = eventIter.size();
					for (Int_t i = 0; i < numInput; ++i)
					{
						if (eventIter.at(i).GetTimeStamp() > timeStampThreshold){
							new ((*fOutput)[i]) PndSdsHit(eventIter.at(i));
							dataAboveTimeThreshold = true;
						}
	 //                   LOG(INFO) << "Data: " << i << " " << fHitVector.at(i).GetTimeStamp();
					}
					if (dataAboveTimeThreshold){
						if (fOutput->IsEmpty())
						{
							LOG(ERROR) << "PndMQFileSinkHits::Run(): No Output array!";
						}

						fTree->Fill();
					}
                }
            }

            delete msg;

            if (fHitVector.size() > 0)
            {
                fHitVector.clear();
            }
        }

        LOG(INFO) << "I've received " << receivedMsgs << " messages!";
    }
    else
    {
        LOG(ERROR) << " Boost Serialization not ok";
    }
}
