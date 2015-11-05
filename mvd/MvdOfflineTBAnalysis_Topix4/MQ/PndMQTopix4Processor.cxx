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
#include <PndMQTopix4Processor.h>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "mrfdata_8b.h"
#include "PndSdsDigiTopix4.h"
#include "PndMQStatus.h"


using namespace std;

PndMQTopix4Processor::PndMQTopix4Processor() : fHasBoostSerialization(false)
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

PndMQTopix4Processor::~PndMQTopix4Processor()
{
}


void PndMQTopix4Processor::Run()
{
	boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
	while (CheckCurrentState(RUNNING))
	{
		unique_ptr<FairMQMessage> input(fTransportFactory->CreateMessage());
		unique_ptr<FairMQMessage> headerPart(fTransportFactory->CreateMessage());

		if (fChannels.at("data-in").at(0).Receive(headerPart) > 0)
		{
			int status = *(static_cast<int*>(headerPart->GetData()));

			//LOG(INFO) << "Status: " << status;

			if (status == PndMQStatus::RUNNING){
				if (fChannels.at("data-in").at(0).Receive(input) > 0) {

					//LOG(INFO) << "Received data, processing...";
					TMrfData_8b* message = new TMrfData_8b();
					message->setNumWords(input->GetSize());
					memcpy(reinterpret_cast<u_int8_t*>(&message->regdata[0]),input->GetData(), input->GetSize());
				//           LOG(INFO) << "Received message: \""
				//                     << message->getNumWords() << " " << message->getNumBits()
				//                     << "\"";
					std::vector<ULong64_t> rawArray;
					rawArray = fTopixDataReader.GetRawData(message);
					std::vector<std::vector<PndSdsDigiTopix4> > frames = fTopixDataReader.AnalyzeData(rawArray, 50);
					fPndSdsDigiTopix4Vector.clear();
					for (auto frameIter : frames){
						fPndSdsDigiTopix4Vector.insert(fPndSdsDigiTopix4Vector.end(), frameIter.begin(), frameIter.end());
					}
					if (fPndSdsDigiTopix4Vector.size() > 0){

			        	unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage(sizeof(int)));
						memcpy(header->GetData(), &status, sizeof(int));
						fChannels.at("data-out").at(0).SendPart(header);

						ostringstream obuffer;
						boost::archive::binary_oarchive OutputArchive(obuffer);
						//fPndSdsDigiTopix4Vector = frames.front();
						OutputArchive << fPndSdsDigiTopix4Vector;
						int outputSize = obuffer.str().length();
						unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage(outputSize));
						memcpy(msg->GetData(), obuffer.str().c_str(), outputSize);
						//unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, CustomCleanup, &obuffer));
						fChannels.at("data-out").at(0).Send(msg);
		//				LOG(INFO) << "Data: " << frames.front().size() << std::endl;
					}
				}
			} else if (status == PndMQStatus::STOP){
				LOG(INFO) << "Catched STOP signal!";

				unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage(sizeof(int)));
				memcpy(header->GetData(), &status, sizeof(int));
				fChannels.at("data-out").at(0).Send(header);
				//ChangeState("STOP");
			}
		}
	}
}

void PndMQTopix4Processor::SetProperty(const int key, const string& value)
{
    switch (key)
    {
    case TimeCorr:
    	fTimeCorrStr = value;
    	fTimeStampCorrection = std::stod(fTimeCorrStr);
    	fTopixDataReader.SetTimeStampCorrection(fTimeStampCorrection);
    	break;
        default:
            FairMQDevice::SetProperty(key, value);
            break;
    }
}

string PndMQTopix4Processor::GetProperty(const int key, const string& default_ /*= ""*/)
{
    switch (key)
    {
		case TimeCorr:
			return fTimeCorrStr;
			break;
        default:
            return FairMQDevice::GetProperty(key, default_);
    }
}

void PndMQTopix4Processor::SetProperty(const int key, const int value)
{
    switch (key)
    {
    case FE:
    	fFE = value;
    	fTopixDataReader.SetFE(fFE);
    	break;
	default:
		FairMQDevice::SetProperty(key, value);
		break;
    }
}

int PndMQTopix4Processor::GetProperty(const int key, const int default_ /*= 0*/)
{
    switch (key)
    {
    case FE:
        	return fFE;
        default:
            return FairMQDevice::GetProperty(key, default_);
    }
}


