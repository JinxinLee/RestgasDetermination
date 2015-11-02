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

PndMQDataDuplicator::PndMQDataDuplicator()
{
	fRates.push_back(0);
	fRates.push_back(200);
}

void PndMQDataDuplicator::Run()
{
    uint64_t counter = 0;

    const FairMQChannel& dataInChannel = fChannels.at("data-in").at(0);

    while (CheckCurrentState(RUNNING))
    {
    	std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());

		if (dataInChannel.Receive(msg) > 0)
		{
			counter++;
			//LOG(INFO) << "Counter: " << counter;
			std::string msgStr(static_cast<char*>(msg->GetData()), msg->GetSize());
			std::istringstream ibuffer(msgStr);

			boost::archive::binary_iarchive InputArchive(ibuffer);

			try {
				InputArchive >> fTopixData;
			}
			catch (boost::archive::archive_exception& e)
			{
				LOG(ERROR) << e.what();
			}

			if (fChannels.at("data-out").size() > 1)
			{
				for (int i = 1; i < fChannels.at("data-out").size(); ++i)
				{
					if (i < fRates.size()){
						//LOG(INFO) << "Channel: " << i;
						if ( counter%fRates[i] == 0 ){
							//LOG(INFO) << "SendMessage";
							std::unique_ptr<FairMQMessage> msgCopy(fTransportFactory->CreateMessage());
							msgCopy->Copy(msg);
							fChannels.at("data-out").at(i).Send(msgCopy);
						}
					}
				}
				fChannels.at("data-out").at(0).Send(msg);
			}
			else
			{
				fChannels.at("data-out").at(0).Send(msg);
			}
		}
    }
}

PndMQDataDuplicator::~PndMQDataDuplicator()
{
}
