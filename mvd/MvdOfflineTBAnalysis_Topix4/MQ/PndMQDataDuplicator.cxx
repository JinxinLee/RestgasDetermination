/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQDataDuplicator.cpp
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#include <memory> // unique_ptr

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "PndMQDataDuplicator.h"
#include "FairMQLogger.h"
#include "PndMQStatus.h"

PndMQDataDuplicator::PndMQDataDuplicator()
{
	fRates.push_back(1);
}

void PndMQDataDuplicator::Run()
{
    uint64_t counter = 0;

    const FairMQChannel& dataInChannel = fChannels.at("data-in").at(0);

    while(fRates.size() < fChannels.at("data-out").size()){
    	LOG(INFO) << "Rates size " << fRates.size() << " smaller than Data-out size " << fChannels.at("data-out").size();
    	fRates.push_back(1);
    }

    while (CheckCurrentState(RUNNING))
    {
    	std::unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage());
		std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
		int status = PndMQStatus::UNDEFINED;
		if (dataInChannel.Receive(header) > 0)
		{
			status = *(static_cast<int*>(header->GetData()));

			if (dataInChannel.ExpectsAnotherPart())
			{
				if (dataInChannel.Receive(msg) > 0)
				{
					counter++;
					if (fChannels.at("data-out").size() > 1)
					{
						for (int i = 1; i < fChannels.at("data-out").size(); ++i)
						{
							if (i < fRates.size()){
								//LOG(INFO) << "Channel: " << i;
								if ( counter%fRates[i] == 0 ){
									//LOG(INFO) << "SendMessage";
									std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage());
									headerCopy->Copy(header);
									fChannels.at("data-out").at(i).SendPart(headerCopy);
									std::unique_ptr<FairMQMessage> msgCopy(fTransportFactory->CreateMessage());
									msgCopy->Copy(msg);
									fChannels.at("data-out").at(i).Send(msgCopy);
								}
							}
						}
						std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage());
						headerCopy->Copy(header);
						fChannels.at("data-out").at(0).SendPart(headerCopy);
						std::unique_ptr<FairMQMessage> msgCopy(fTransportFactory->CreateMessage());
						msgCopy->Copy(msg);
						fChannels.at("data-out").at(0).Send(msgCopy);
					}
					else
					{
						std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage());
						headerCopy->Copy(header);
						fChannels.at("data-out").at(0).SendPart(headerCopy);
						std::unique_ptr<FairMQMessage> msgCopy(fTransportFactory->CreateMessage());
						msgCopy->Copy(msg);
						fChannels.at("data-out").at(0).Send(msgCopy);
					}
				}
			} else {
				std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage());
				headerCopy->Copy(header);
				fChannels.at("data-out").at(0).Send(headerCopy);
			}
			if (status == PndMQStatus::STOP)
				LOG(INFO) << "STOP-Signal Received!";
		}
    }
}

PndMQDataDuplicator::~PndMQDataDuplicator()
{
}
