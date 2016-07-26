/*
 * File:   PndMQFileSinkBursts.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQFileSinkBursts::Run() with Boost transport data format
#include "PndMQFileSinkBursts.h"
#include "PndMQGapEventBuilder.h"
#include "PndMapSorter.h"

void PndMQFileSinkBursts::Run()
{
	PndMQGapEventBuilder eventBuilder;
	PndMapSorter mapSorter;
    if (fHasBoostSerialization)
    {
        while (CheckCurrentState(RUNNING))
        {
        	 FairMQParts parts;
        	 std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
        	 if (Receive(msg, "data-in") >= 0)
        	 {
 //       		 LOG(INFO) << "Received data!" << std::endl;
				 string msgStr(static_cast<char*>(msg->GetData()), msg->GetSize());
				 istringstream ibuffer(msgStr);
				 boost::archive::binary_iarchive InputArchive(ibuffer);
				// InputArchive >> fHitVector;
				// LOG(INFO) << i << " : " << fHitVector.size() << std::endl;
				 InputArchive >> fBurstData;
				 LOG(INFO) << "BurstData size: " << fBurstData.fData.size();
				 for (auto dataItr : fBurstData.fData)
					 LOG(INFO) << dataItr->GetTimeStamp();
				 mapSorter.AddElements(fBurstData.fData);
				 mapSorter.WriteOutAll();
				 std::vector<FairTimeStamp*> sortedData = mapSorter.GetOutputData();
//				 for (auto i : sortedData){
//					 LOG(INFO) << i->GetTimeStamp() << std::endl;
//				 }
				 eventBuilder.FillData(sortedData);
				 std::vector<std::vector<FairTimeStamp*> > gapData = eventBuilder.GetSeparatedData();
//				 LOG(INFO) << "Gaps:  " << gapData.size() << std::endl;
				 mapSorter.DeleteOutputData();
        	 }
        }

    }
    else
    {
        LOG(ERROR) << " Boost Serialization not ok";
    }
}
