#include "PndLmdDataFacade.h"
#include "PndLmdFitFacade.h"
#include "LumiImpl/LumiFitStructs.h"
#include "PndLmdVertexData.h"

#include <iostream>
#include <sstream>
#include <string>

#include "TFile.h"

void determineBeamOffset(std::string data_path, std::string sim_config_path) {
	// facade for opening lmd files
	PndLmdDataFacade lmd_data_facade;

	// then open the vertex distribution root file and get the lmd data objects
	std::stringstream hs;
	hs << data_path << "/lmd_vertex_data.root";

	TFile *f = new TFile(hs.str().c_str(), "READ");
	std::vector<PndLmdVertexData> data_vec =
			lmd_data_facade.getDataFromFile<PndLmdVertexData>(f);

	PndLmdFitFacade lmd_fit_facade;

	// construct model options
	LumiFit::PndLmdFitModelOptions model_options;
	model_options.vertex_model = LumiFit::GAUSSIAN;
	lmd_fit_facade.setModelFitOptions(model_options);
	// construct estimator options
	EstimatorOptions est_options;
	DataStructs::DimensionRange fit_range(-1.0, 1.0);
	est_options.setFitRangeX(fit_range);
	lmd_fit_facade.setEstimatorOptions(est_options);

	// do fits
	lmd_fit_facade.fitVertexData(data_vec);

	// save data
	std::cout << "Saving data...." << std::endl;
	// output file
	hs.str("");
	hs << data_path << "/lmd_fitted_vertex_data.root";
	TFile *ffitteddata = new TFile(hs.str().c_str(), "RECREATE");

	for (std::vector<PndLmdVertexData>::iterator lmd_data_iter =
			data_vec.begin(); lmd_data_iter != data_vec.end();
			lmd_data_iter++) {
		if (lmd_data_iter->getFitResults().size() > 0)
			lmd_data_iter->saveToRootFile();
	}

	ffitteddata->Close();
}

void displayInfo() {
	// display info
	std::cout << "Required arguments are: " << std::endl;
	std::cout << "-p [path to data]" << std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-s [path to simulation config file] (defaul: data path)" << std::endl;
	std::cout << std::endl;
}

int main(int argc, char* argv[]) {
	bool is_data_path_set = false;
	bool is_sim_config_path_set = false;
	std::string data_path;
	std::string sim_config_path;
	int c;

	while ((c = getopt(argc, argv, "hp:s:")) != -1) {
		switch (c) {
			case 'p':
				data_path = optarg;
				is_data_path_set = true;
				break;
			case 's':
				sim_config_path = optarg;
				is_sim_config_path_set = true;
				break;
			case '?':
				if (optopt == 'p')
					std::cerr << "Option -" << optopt << " requires an argument."
							<< std::endl;
				else if (isprint(optopt))
					std::cerr << "Unknown option -" << optopt << "." << std::endl;
				else
					std::cerr << "Unknown option character" << optopt << "." << std::endl;
				return 1;
			case 'h':
				displayInfo();
				return 1;
			default:
				return 1;
		}
	}

	if(!is_sim_config_path_set)
		sim_config_path = data_path;

	if (is_data_path_set)
		determineBeamOffset(data_path, sim_config_path);
	else
		displayInfo();

	return 0;
}
