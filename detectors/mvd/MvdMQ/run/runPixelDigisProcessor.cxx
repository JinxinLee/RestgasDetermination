
/// std
#include <csignal>

/// FairRoot - FairMQ - base/MQ
#include "FairMQLogger.h"
#include "runSimpleMQStateMachine.h"


// 9-PixelDetector example
#include "PndMQMvdPixelDigiProcessorBursts.h"

// ////////////////////////////////////////////////////////////////////////
namespace po = boost::program_options;

using channelBranches = std::vector<std::pair<std::string, std::string> >;

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
    try
    {

        FairMQProgOptions config;

        config.ParseAll(argc, argv);

        PndMQMvdPixelDigiProcessorBursts processor;


    runStateMachine(processor, config);


    }
    catch (std::exception& e)
    {
        LOG(ERROR)  << "Unhandled Exception reached the top of main: " 
                    << e.what() << ", application will now exit";
        return 1;
    }

    return 0;
}
