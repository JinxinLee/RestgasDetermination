/*
 * calculateElasticCrossSection.cxx
 *
 *  Created on: Apr 7, 2014
 *      Author: steve
 */

#include "LumiImpl/model/PndLmdModelFactory.h"
#include "LumiImpl/LumiFitStructs.h"

#include <utility>
#include <vector>
#include <iostream>

void calculateElasticCrossSection(double momentum, double lower_bound,
		double upper_bound) {
	PndLmdModelFactory model_factory;

	LumiFit::PndLmdFitModelOptions model_opt;
	model_opt.acceptance_correction_active = false;
	model_opt.momentum_transfer_active = false;
	model_opt.resolution_smearing_active = false;

	shared_ptr<Model1D> model = model_factory.generate1DModel(model_opt,
			momentum);
	std::vector<std::pair<double, double> > integral_ranges;
	integral_ranges.push_back(
			std::make_pair(lower_bound / 1000.0, upper_bound / 1000.0));
	double integral = model->Integral(integral_ranges, 0.001);

	std::cout << "Integrated total elastic cross section in theta range ["
			<< lower_bound << " - " << upper_bound << "] is " << integral << " mb"
			<< std::endl;
}

void displayInfo() {
	// display info
	std::cout << "Required arguments are: " << std::endl;
	std::cout << "-m [pbar momentum]" << std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-l [lower theta boundary in mrad] (default: 3mrad)"
			<< std::endl;
	std::cout << "-u [upper theta boundary in mrad] (default: 8mrad)"
			<< std::endl;
}

int main(int argc, char* argv[]) {
	bool is_mom_set = false;
	double momentum = -1.0;
	double lower_bound = 3.0;
	double upper_bound = 8.0;
	int c;

	while ((c = getopt(argc, argv, "hm:l:u:")) != -1) {
		switch (c) {
			case 'm':
				momentum = atof(optarg);
				is_mom_set = true;
				break;
			case 'l':
				lower_bound = atof(optarg);
				break;
			case 'u':
				upper_bound = atof(optarg);
				break;
			case '?':
				if (optopt == 'm' || optopt == 'l' || optopt == 'u')
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

	if (is_mom_set)
		calculateElasticCrossSection(momentum, lower_bound, upper_bound);
	else
		displayInfo();

	return 0;
}
