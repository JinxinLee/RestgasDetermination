
/// std
#include <PndMQMergerTest.h>
#include <csignal>

/// FairRoot - FairMQ - base/MQ
#include "FairMQLogger.h"
#include "runSimpleMQStateMachine.h"


// 9-PixelDetector example

// ////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    try
    {

    	po::options_description merger_options("Merger options");

        FairMQProgOptions config;
        config.AddToCmdLineOptions(merger_options);

        config.ParseAll(argc, argv);

        PndMQMergerTest testMerger;

	
        runStateMachine(testMerger, config);


    }
    catch (std::exception& e)
    {
        LOG(ERROR)  << "Unhandled Exception reached the top of main: " 
                    << e.what() << ", application will now exit";
        return 1;
    }

    return 0;
}
