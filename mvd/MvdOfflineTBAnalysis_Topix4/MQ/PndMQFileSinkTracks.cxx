/*
 * File:   PndMQFileSinkTracks.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQFileSinkTracks::Run() with Boost transport data format
#include "PndMQFileSinkTracks.h"
#include "PndMQStatus.h"

void PndMQFileSinkTracks::Run()
{
    if (fHasBoostSerialization)
    {
        int receivedMsgs = 0;

        // store the channel references to avoid traversing the map on every loop iteration
        FairMQChannel& dataInChannel = fChannels.at("data-in").at(0);

        while (CheckCurrentState(RUNNING))
        {
        	std::unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage());
			std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
			if (dataInChannel.Receive(header) > 0)
			{
				int status = *(static_cast<int*>(header->GetData()));

				if (dataInChannel.ExpectsAnotherPart())
				{
					receivedMsgs++;
					if (dataInChannel.Receive(msg)) {
						string msgStr(static_cast<char*>(msg->GetData()), msg->GetSize());
						istringstream ibuffer(msgStr);
						boost::archive::binary_iarchive InputArchive(ibuffer);
						//LOG(INFO) << "Received Message: " << receivedMsgs;
						try
						{
							InputArchive >> fTrackVector;
						}
						catch (boost::archive::archive_exception& e)
						{
							LOG(ERROR) << e.what();
						}

						PndSimpleTrack* myTrack;
						for (auto& eventIter : fTrackVector){
							fOutput->Delete();
							int numData = eventIter.size();
							for (Int_t i = 0; i < numData; ++i)
							{
								//LOG(INFO) << "Received Track with: " << eventIter[i].GetTrackCand().GetNHits();
								myTrack = new ((*fOutput)[fOutput->GetEntriesFast()]) PndSimpleTrack(eventIter[i]);

							}
							if (fOutput->IsEmpty())
							{
								LOG(ERROR) << "PndMQFileSinkTracks::Run(): No Output array!";
							} else {

								fTree->Fill();
							}
						}
						if (receivedMsgs % 1000 == 0 && myTrack != 0){
							LOG(INFO) << receivedMsgs;
						}
					}
				}
				if (status == PndMQStatus::STOP){
					LOG(INFO) << "STOP-Signal Received!";
					fTree->AutoSave();
				    LOG(INFO) << "AutoSave called!";
					fTree->Write();
					fOutFile->Close();
				}

            }

            if (fTrackVector.size() > 0)
            {
                fTrackVector.clear();
            }
        }

        LOG(INFO) << "I've received " << receivedMsgs << " messages!";
    }
    else
    {
        LOG(ERROR) << " Boost Serialization not ok";
    }
}
