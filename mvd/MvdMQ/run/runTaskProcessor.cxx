
/// std
#include <PndMvdMQTaskProcessor.h>
#include <csignal>

/// FairRoot - FairMQ - base/MQ
#include "FairMQLogger.h"
#include "GenericProcessor.h"
#include "runSimpleMQStateMachine.h"
#include "BoostSerializer.h"
#include "RootSerializer.h"
#include "PndMvdPixelClusterTask.h"
#include "PndMvdStripClusterTask.h"

// 9-PixelDetector example

using PixelHitFinder   = PndMvdMQTaskProcessor<PndMvdPixelClusterTask>;
using StripHitFinder   = PndMvdMQTaskProcessor<PndMvdStripClusterTask>;
//using TrackFinder = PndMvdMQTaskProcessor<PixelFindTracks>;
//using TrackFitter = PndMvdMQTaskProcessor<PixelFitTracks>;

// ////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	try {
		std::string taskname;
		std::string keepdata;

		namespace po = boost::program_options;
		po::options_description processor_options("Processor options");
		processor_options.add_options()("task-name",
				po::value<std::string>(&taskname)->required(),
				"Name of task to run")("keep-data",
				po::value<std::string>(&keepdata),
				"Name of data to keep in stream");

		FairMQProgOptions config;
		config.AddToCmdLineOptions(processor_options);

		if (config.ParseAll(argc, argv))
			return 1;

		if (strcmp(taskname.c_str(), "PndMvdPixelClusterTask") == 0) {
			PixelHitFinder processor;
			processor.SetDataToKeep(keepdata);
			runStateMachine(processor, config);
		} else if (strcmp(taskname.c_str(), "PndMvdStripClusterTask") == 0) {
			StripHitFinder processor;
			processor.SetDataToKeep(keepdata);
			runStateMachine(processor, config);
		} else {
			LOG(INFO) << "TASK \"" << taskname << "\" UNKNOWN!!! " << strcmp(taskname.c_str(), "PndMvdStripClusterTask");
		}


	} catch (std::exception& e) {
		LOG(ERROR) << "Unhandled Exception reached the top of main: "
								<< e.what() << ", application will now exit";
		return 1;
	}

	return 0;
}
