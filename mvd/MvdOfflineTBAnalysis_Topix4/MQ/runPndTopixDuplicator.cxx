/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * runExample1Sink.cxx
 *
 * @since 2013-04-23
 * @author D. Klein, A. Rybalchenko
 */

//#include <PndMQTopix4ProcessorTask.h>
#include <PndMQDataDuplicator.h>
#include <iostream>

#include "runSimpleMQStateMachine.h"

#include "boost/program_options.hpp"

#include "FairMQLogger.h"
#include "FairMQParser.h"
#include "FairMQProgOptions.h"
#include "FairMQProcessor.h"
#include "FairMQDevice.h"

#ifdef NANOMSG
#include "FairMQTransportFactoryNN.h"
#else
#include "FairMQTransportFactoryZMQ.h"
#endif

using namespace boost::program_options;

using duplicatorValues = std::vector<std::pair<std::string, std::string> >; //data-out channel, rate

namespace std {
	static inline std::istream& operator>>(std::istream& is, std::pair<std::string, std::string>& into)
	{
		char ch;
		while (is >> ch && ch!='=') into.first += ch;
		return is >> into.second;
	}
}

int main(int argc, char** argv)
{
	PndMQDataDuplicator processor;
    processor.CatchSignals();

    processor.SetRateRatio(1, 1);

    FairMQProgOptions config;
    duplicatorValues dupValues;


    try
    {

    	 po::options_description sampler_options("Sampler options");
    	 sampler_options.add_options()
		("channel-rate", po::value<duplicatorValues>(&dupValues) ->multitoken(), "channel rate");

    	 config.AddToCmdLineOptions(sampler_options);

        config.ParseAll(argc, argv);
        std::string filename = config.GetValue<std::string>("config-json-file");
        std::string id = config.GetValue<std::string>("id");


        config.UserParser<FairMQParser::JSON>(filename, id);

        processor.fChannels = config.GetFairMQMap();

        LOG(INFO) << "PID: " << getpid();
        LOG(INFO) << "ID: " << id ;

#ifdef NANOMSG
        FairMQTransportFactory* transportFactory = new FairMQTransportFactoryNN();
#else
        FairMQTransportFactory* transportFactory = new FairMQTransportFactoryZMQ();
#endif

        processor.SetTransport(transportFactory);

        processor.SetProperty(FairMQDevice::Id, id);

        LOG(INFO) << "Channel Rates size: " << dupValues.size();

        for(int i = 0; i < dupValues.size(); i++){
			LOG(INFO) << "BranchNames: " << dupValues[i].first << "/" << dupValues[i].second;
			processor.SetRateRatio(std::stoi(dupValues[i].first), std::stoi(dupValues[i].second));
		}

        //PndMQTopix4ProcessorTask* task = new PndMQTopix4ProcessorTask();
        //processor.SetTask(task);

        runStateMachine(processor, config);

//        processor.ChangeState("INIT_DEVICE");
//        processor.WaitForEndOfState("INIT_DEVICE");

//        processor.ChangeState("INIT_TASK");
//        processor.WaitForEndOfState("INIT_TASK");

//        processor.ChangeState("RUN");
//        processor.InteractiveStateLoop();
    }
    catch (std::exception& e)
    {
        LOG(ERROR) << e.what();
        LOG(INFO) << "Command line options are the following: ";
        config.PrintHelp();
        return 1;
    }

    return 0;
}
