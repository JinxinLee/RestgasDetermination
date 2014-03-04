#include "PndLmdDataFacade.h"
#include "PndLmdFitFacade.h"

#include <iostream>
#include <string>

#include "TFile.h"

void determineBeamOffset(std::string data_path) {
	// facade for opening lmd files
	PndLmdDataFacade lmd_data_facade;

	// get simulation ip distribution properties
	LumiFit::LmdSimIPParameters true_ip_values =
			lmd_data_facade.readSimulationIPParameters(data_path);
	if (true_ip_values.parameters_exist)
		true_ip_values.print();

	// then open the vertex distribution root file and get the lmd data objects

	TFile *f = new TFile((data_path + "/lmd_vertex_data.root").c_str(), "READ");
	std::vector<PndLmdData*> data_vec = lmd_data_facade.getDataFromFile<
			PndLmdData*>(f);

	// construct dimension object
	LumiFit::LmdDimensionOptions lmd_dim;
	lmd_dim.dimension_type = LumiFit::X;

	//std::vector<PndLmdData*> data_vec = data_map[lmd_dim];

	PndLmdFitFacade lmd_fit_facade;

	for (unsigned int i = 0; i < data_vec.size(); i++) {
		//lmd_fit_facade.;
		//data_vec[i]->;
	}
}

void displayInfo() {
	// display info
	std::cout << "Required arguments are: " << std::endl;
	std::cout << "-p [path to data]" << std::endl;
	std::cout << std::endl;
}

int main(int argc, char* argv[]) {
	bool is_data_path_set = false;
	std::string data_path;
	int c;

	while ((c = getopt(argc, argv, "hp:")) != -1) {
		switch (c) {
			case 'p':
				data_path = optarg;
				is_data_path_set = true;
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

	if (is_data_path_set)
		determineBeamOffset(data_path);
	else
		displayInfo();

	return 0;
}
