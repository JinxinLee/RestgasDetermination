#include "PndLmdResultPlotter.h"
#include "data/PndLmdDataFacade.h"
#include "data/PndLmdResolution.h"

#include "TStyle.h"
#include "TFile.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

void plotResolutionParametrization(TString input_file_dir,
		unsigned int parametrization_level, TString input_ref_file_dir = "") {
	std::cout << "Running resolution plotter macro....\n";

	// create an instance of PndLmdResultPlotter the plotting helper class
	PndLmdResultPlotter plotter;
	// A small helper class that helps to construct lmddata objects
	PndLmdDataFacade data_facade;

	// ================================ BEGIN CONFIG ================================ //
	// PndLmdResultPlotter sets default pad margins etc that should be fine for most cases
	// you can fine tune it and overwrite the default values
	gStyle->SetPadTopMargin(0.06);
	//gStyle->SetPadBottomMargin(0.12);
	gStyle->SetPadLeftMargin(0.13);
	gStyle->SetPadRightMargin(0.02);

	gStyle->SetTitleX(0.3);
	//gStyle->SetTitleY();
	//gStyle->SetTitleW();
	//gStyle->SetTitleH();

	// overwrite the default theta plot range if possible (if its larger than the max
	// plot range then it has no effect)
	//plotter.setThetaPlotRange(-6.0, 6.0);

	// The plotter has more options for text positioning and tex sizes for which you can
	// overwrite the default values here
	plotter.setTextLeftPos(0.25);
	plotter.setTextTopPos(0.92);
	//plotter.setTextSpacing(0.08);
	plotter.setTextSize(0.06);
	plotter.setLabelSize(0.06);

	//plotter.setLabelOffsetX(0.007);
	//plotter.setLabelOffsetY(0.007);
	//plotter.setTitleOffsetX(1.0);
	plotter.setTitleOffsetY(1.13);
	// ================================= END CONFIG ================================= //

	// =============================== BEGIN PLOTTING =============================== //

	char input_filename[50];
	sprintf(input_filename, "/resolution_params_%u.root", parametrization_level);
	TString input_filename_url = input_file_dir + input_filename;
	TFile *infile = new TFile(input_filename_url, "READ");

	switch (parametrization_level) {
		case 0: {
			// read in data from a root file
			std::vector<PndLmdResolution> res_vec = data_facade.getDataFromFile<
					PndLmdResolution>(infile);
			// first lets create a booky of all resolutions with the fitted resolutions
			plotter.makeResolutionBooky(res_vec, "resolution");
			if (!input_ref_file_dir.EqualTo("")) {
				char ref_input_filename[50];
				sprintf(ref_input_filename, "/resolution_params_%u.root",
						parametrization_level);
				TString ref_input_filename_url = input_ref_file_dir + input_filename;
				TFile *ref_infile = new TFile(ref_input_filename_url, "READ");

				// read in data from a root file
				std::vector<PndLmdResolution> ref_res_vec = data_facade.getDataFromFile<
						PndLmdResolution>(ref_infile);

				plotter.makeResolutionDifferencesBooky(res_vec, ref_res_vec);
			}
			break;
		}
		case 1: {
			plotter.makeResolutionSummaryPlots(infile);
			break;
		}
		default: {
			std::cout << "Error: The requested parametrization level "
					<< parametrization_level << " does not exist. The highest level is 3."
					<< std::endl;

			break;
		}
	}

	// ================================ END PLOTTING ================================ //
}

void displayInfo() {
	// display info
	cout << "Required arguments are: " << endl;
	cout << "-d [path to data]" << endl;
	cout << "-l [parametrization level] (0 or 1)" << endl;
	cout << "-r [path to reference data]" << endl;
}

int main(int argc, char* argv[]) {
	bool is_data_set = false, is_data_ref_set = false, is_par_level_set = false;
	TString data_path;
	TString data_ref_path;
	int parametrization_level = 0;
	int c;

	while ((c = getopt(argc, argv, "hl:d:r:")) != -1) {
		switch (c) {
			case 'l':
				parametrization_level = atoi(optarg);
				is_par_level_set = true;
				break;
			case 'd':
				data_path = optarg;
				is_data_set = true;
				break;
			case 'r':
				data_ref_path = optarg;
				is_data_ref_set = true;
				break;
			case '?':
				if (optopt == 'd' || optopt == 'r' || optopt == 'l')
					cerr << "Option -" << optopt << " requires an argument." << endl;
				else if (isprint(optopt))
					cerr << "Unknown option -" << optopt << "." << endl;
				else
					cerr << "Unknown option character" << optopt << "." << endl;
				return 1;
			case 'h':
				displayInfo();
				return 1;
			default:
				return 1;
		}
	}

	if (is_data_set && is_par_level_set)
		plotResolutionParametrization(data_path, parametrization_level,
				data_ref_path);
	else
		displayInfo();
	return 0;
}

