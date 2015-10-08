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
#include <PndMQSorterDistributor.h>
#include <iostream>

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

int main(int argc, char** argv)
{
	PndMQSorterDistributor processor;
    processor.CatchSignals();

    FairMQProgOptions config;

    int fe;

    options_description samplerOptions("Sampler options");
	samplerOptions.add_options()
		("FE", value<int>(&fe)->default_value(-1), "Front-End ID");

	config.AddToCmdLineOptions(samplerOptions);

    try
    {
        if (config.ParseAll(argc, argv))
        {
            return 0;
        }

        std::string filename = config.GetValue<std::string>("config-json-file");
        std::string id = config.GetValue<std::string>("id");


        config.UserParser<FairMQParser::JSON>(filename, id);

        processor.fChannels = config.GetFairMQMap();

        LOG(INFO) << "PID: " << getpid();
        LOG(INFO) << "ID: " << id ;
        LOG(INFO) << "FE: " << fe;
        LOG(INFO) << "Processor::Id Kes: " << FairMQDevice::Id;
        processor.ListProperties();

#ifdef NANOMSG
        FairMQTransportFactory* transportFactory = new FairMQTransportFactoryNN();
#else
        FairMQTransportFactory* transportFactory = new FairMQTransportFactoryZMQ();
#endif

        processor.SetTransport(transportFactory);

        processor.SetProperty(FairMQDevice::Id, id);

        //PndMQTopix4ProcessorTask* task = new PndMQTopix4ProcessorTask();
        //processor.SetTask(task);

        processor.ChangeState("INIT_DEVICE");
        processor.WaitForEndOfState("INIT_DEVICE");

        processor.ChangeState("INIT_TASK");
        processor.WaitForEndOfState("INIT_TASK");

        processor.ChangeState("RUN");
        processor.InteractiveStateLoop();
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
