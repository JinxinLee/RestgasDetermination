/*
 * File:   PndMQFileSink.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQFileSink::Run() with Boost transport data format
#include <PndMQFileSink.h>

void PndMQFileSink::Run()
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

                int numInput = fHitVector.size();
                fOutput->Delete();

                for (Int_t i = 0; i < numInput; ++i)
                {
                    new ((*fOutput)[i]) PndSdsDigiTopix4(fHitVector.at(i));
 //                   LOG(INFO) << "Data: " << i << " " << fHitVector.at(i).GetTimeStamp();
                }

                if (fOutput->IsEmpty())
                {
                    LOG(ERROR) << "PndMQFileSink::Run(): No Output array!";
                }

                fTree->Fill();
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
