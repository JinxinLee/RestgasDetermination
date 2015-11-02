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
#include <boost/archive/binary_iarchive.hpp>
#include <PndMQTopix4DigiToHit.h>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "mrfdata_8b.h"
#include "PndSdsDigiTopix4.h"
#include <iomanip>


using namespace std;

PndMQTopix4DigiToHit::PndMQTopix4DigiToHit() : fHasBoostSerialization(false), fClusterFinder(20, 32, 1.8), fHitProducer(0.01, 0.01, 20, 32), fEventBuilder(50)
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

PndMQTopix4DigiToHit::~PndMQTopix4DigiToHit()
{
}


void PndMQTopix4DigiToHit::Run()
{
	if(fHasBoostSerialization){
		FairMQChannel& dataInChannel = fChannels.at("data-in").at(0);
		FairMQChannel& dataOutChannel = fChannels.at("data-out").at(0);

		while (CheckCurrentState(RUNNING))
		{
			FairMQMessage* msg = fTransportFactory->CreateMessage();
			if (dataInChannel.Receive(msg) > 0)
			{
				string msgStr(static_cast<char*>(msg->GetData()), msg->GetSize());
				istringstream ibuffer(msgStr);

				boost::archive::binary_iarchive InputArchive(ibuffer);

				try {
					InputArchive >> fTopixDigis;
				}
				catch (boost::archive::archive_exception& e)
				{
					LOG(ERROR) << e.what();
				}
			}
//			LOG(INFO) << "InputData: ";
//			for (auto itr : fTopixDigis){
//				LOG(INFO) << std::setw(12) << std::hex << itr.GetTimeStamp();
//			}

			fEventBuilder.FillData(fTopixDigis);
			std::vector<std::vector<PndSdsDigiTopix4> > separatedData = fEventBuilder.GetSeparatedData();

			for (auto & eventIter : separatedData){
				std::vector<PndSdsHit> hits;
				if (eventIter.size() > 1){
					std::vector<std::vector<int> > cluster = fClusterFinder.GetClusters(eventIter);
//					LOG(INFO) << "Multiple Event Found with " << eventIter.size() << " digs and " << cluster.size() << " clusters";
					for (auto clusterIter : cluster){
						std::vector<PndSdsDigiTopix4> clusterDigis;
						for (auto digiIter : clusterIter){
							clusterDigis.push_back((eventIter)[digiIter]);
//							LOG(INFO) << "Digis " << digiIter << " TimeStamp: " << (eventIter)[digiIter].GetTimeStamp();
//							LOG(INFO) << "ClusterDigis: " << clusterDigis.rbegin()->GetTimeStamp();
						}
						//PndSdsHit myHit = fDummy.GetHit(clusterDigis);
						//fHitProducer.GetHit(clusterDigis);
						hits.push_back(fHitProducer.GetHit(clusterDigis));
					}
				} else {
					//PndSdsHit myHit = fDummy.GetHit(eventIter);
					//fHitProducer.GetHit(eventIter);
					hits.push_back(fHitProducer.GetHit(eventIter));
				}
				fTopixHitsEvent.push_back(hits);
			}

			std::ostringstream obuffer;
			boost::archive::binary_oarchive OutputArchive(obuffer);
			OutputArchive << fTopixHitsEvent;
			int outputSize = obuffer.str().length();
			unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
			memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
			dataOutChannel.Send(msg2);

//			LOG(INFO) << "OutputEventBuilding: ";
//			for (auto outerIter : fTopixHitsEvent){
//				LOG(INFO) << "Event: ";
//				for (auto innerIter : outerIter) {
//					LOG(INFO)<< innerIter;
//				}
//			}
			fTopixDigis.clear();
			fTopixHitsEvent.clear();
		}
	}
}
