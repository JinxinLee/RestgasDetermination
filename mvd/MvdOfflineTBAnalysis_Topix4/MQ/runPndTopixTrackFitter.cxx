/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * runExample1fitter.cxx
 *
 * @since 2013-04-23
 * @author D. Klein, A. Rybalchenko
 */

#include <PndMQTopix4TrackFitter.h>
#include <iostream>
#include <TApplication.h>

#include "boost/program_options.hpp"

#include "FairMQLogger.h"
#include "FairMQParser.h"
#include "FairMQProgOptions.h"

#ifdef NANOMSG
#include "FairMQTransportFactoryNN.h"
#else
#include "FairMQTransportFactoryZMQ.h"
#endif

using namespace boost::program_options;

int main(int argc, char** argv)
{
    PndMQTopix4TrackFitter fitter;
    fitter.CatchSignals();

    FairMQProgOptions config;

    try
    {
        config.ParseAll(argc, argv);

        fitter.fChannels = config.GetFairMQMap();

        LOG(INFO) << "PID: " << getpid();

#ifdef NANOMSG
        FairMQTransportFactory* transportFactory = new FairMQTransportFactoryNN();
#else
        FairMQTransportFactory* transportFactory = new FairMQTransportFactoryZMQ();
#endif

        fitter.SetTransport(transportFactory);

        fitter.ChangeState("INIT_DEVICE");
        fitter.WaitForEndOfState("INIT_DEVICE");

        fitter.ChangeState("INIT_TASK");
        fitter.WaitForEndOfState("INIT_TASK");

        fitter.ChangeState("RUN");
        fitter.InteractiveStateLoop();

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
