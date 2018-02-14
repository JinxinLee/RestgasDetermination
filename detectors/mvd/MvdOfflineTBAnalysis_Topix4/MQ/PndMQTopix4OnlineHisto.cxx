/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQTopix4OnlineHisto.cxx
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <PndMQTopix4OnlineHisto.h>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "mrfdata_8b.h"
#include "PndSdsDigiTopix4.h"
#include "TSystem.h"

#include <TH2.h>
#include <TCanvas.h>


using namespace std;

PndMQTopix4OnlineHisto::PndMQTopix4OnlineHisto() : fHasBoostSerialization(false)
{
	gSystem->ResetSignal(kSigInterrupt);
	gSystem->ResetSignal(kSigTermination);

	using namespace baseMQ::tools::resolve;
	// coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
	if (has_BoostSerialization<PndSdsDigiTopix4, void(boost::archive::binary_iarchive&, const unsigned int)>::value == 1)
		fHasBoostSerialization = true;
}

//void PndMQTopix4OnlineHisto::CustomCleanup(void *data, void *object)
//{
//    delete (string*)object;
//}

void PndMQTopix4OnlineHisto::Run()
{
	LOG(INFO) << "Boost Serialization "<< fHasBoostSerialization;
	if (fHasBoostSerialization){
		FairMQChannel& dataInChannel = fChannels.at("data-in").at(0);
		int receivedMsgs = 0;

		TH2* h2 = new TH2D("h2","h2", 21, -0.5, 20.5, 33, -0.5, 32.5);
		TCanvas *c1 = new TCanvas("c1", "Dynamic Filling Example", 500, 100, 700, 500);
		c1->Show();
		while (CheckCurrentState(RUNNING))
		{
			FairMQMessage* msg = fTransportFactory->CreateMessage();

			if (dataInChannel.Receive(msg) > 0)
			{
				LOG(INFO) << "Received Message: ";
				LOG(INFO) << receivedMsgs++;
				LOG(INFO) << msg->GetSize();

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

				LOG(INFO) << "TopixData: " << fTopixData.size();

				if (fTopixData.size() > 0){
					for (auto itr : fTopixData){
						h2->Fill(itr.GetPixelColumn(), itr.GetPixelRow());
					}
				}
				delete(msg);

				const int kUPDATE = 100;
				if (receivedMsgs){
					if (receivedMsgs == kUPDATE) h2->Draw("colz");
					c1->Modified();
					c1->Update();
				}

				if (fTopixData.size() > 0)
					fTopixData.clear();
			}
		}
	}
}

PndMQTopix4OnlineHisto::~PndMQTopix4OnlineHisto()
{
}
