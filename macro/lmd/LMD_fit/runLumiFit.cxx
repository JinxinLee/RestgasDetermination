/*
 * This is the application which should be used for luminosity fitting (luminosity extraction procedure)
 * General information about the individual classes of the LmdFit framework can be
 * found in the doxygen manual
 * For running help use -h flag.
 *
 * This macro is basically a 2 step process and each step is explained in more detail
 * below:
 * step1: read in lmddata objects (via data facade)
 * step2: create fit options that specify every detail about the fit (via fit facade)
 *        and perform the fit and store the results (via fit facade)
 */

#include "PndLmdDataFacade.h"
#include "PndLmdFitFacade.h"
#include "DataStructs.h"

#include <vector>
#include <iostream>

#include "TFile.h"
#include "TString.h"

using std::vector;
using std::cout;
using std::cerr;
using std::endl;

void runLumiFit(TString input_file_dir, TString acceptance_file_dir) {
	cout << "Running LumiFit...." << endl;

	// ============================== BEGIN STEP 1 ============================== //

	// A small helper class that helps to construct lmddata objects
	PndLmdDataFacade lmd_data_facade;

	// ------ get files -------------------------------------------------------
	TFile *fdata = new TFile(input_file_dir + "/lmd_data.root", "READ");
	TFile *facc = 0;
	if (acceptance_file_dir != "")
		facc = new TFile(acceptance_file_dir + "/lmd_acc_data.root", "READ");
	// ------------------------------------------------------------------------

	// get lmd data and objects from files
	std::vector<PndLmdData> my_lmd_data_vec = lmd_data_facade.getDataFromFile<
			PndLmdData>(fdata);
	std::vector<PndLmdAcceptance> my_lmd_acc_vec =
			lmd_data_facade.getDataFromFile<PndLmdAcceptance>(facc);

	// output file
	TFile *ffitteddata = new TFile(input_file_dir + "/lmd_fitted_data.root",
			"RECREATE");

	// =============================== END STEP 1 =============================== //

	// ============================== BEGIN STEP 2 ============================== //

	PndLmdFitFacade lmd_fit_facade;

	// specify the resolution parametriztion file which will take care of the smearing
	lmd_fit_facade.getFitOptionTemplate().setResolutionParametrizationFileUrl(
			acceptance_file_dir + "/resolution_params_1.root");

	lmd_fit_facade.getFitOptionTemplate().setAcceptanceInterpolationType(2);

	// we will just take the first theta ip acceptance
	// (usually you would only have a single one)
	// otherwise just choose the one you want to use
	LumiFit::LmdDimensionOptions acc_options;
	acc_options.dimension_type = LumiFit::THETA;
	acc_options.track_param_type = LumiFit::IP;
	acc_options.track_type = LumiFit::MC;
	if (my_lmd_acc_vec.size() > 0)
		lmd_fit_facade.getFitOptionTemplate().setAcceptance(&my_lmd_acc_vec[0]);

	// perform luminosity fits
	// only luminosity is free parameter
	lmd_fit_facade.getFitOptionTemplate().setFreeParametersCode(0);
	// lets perform "standard" fits first if possible
	LumiFit::LmdDimensionRange fit_range;
	fit_range.setUnitPrefix(DataStructs::MILLI);

	for (unsigned int i = 0; i < 11; i++) {
		fit_range.setRangeLow(2.0 + 0.2 * i);
		fit_range.setRangeHigh(10.0);
		lmd_fit_facade.getFitOptionTemplate().setPrimaryDimensionFitRange(
				fit_range);
		lmd_fit_facade.doLmdLumiFits(my_lmd_data_vec);
	}

	// save fit results by just saving the lmd data objects. They contain both
	// the data and the fit results so from this objects the plotting macro can
	// construct some nice plots for you!
	cout << "Saving data...." << endl;
	ffitteddata->cd();
	for (std::vector<PndLmdData>::iterator lmd_data_iter =
			my_lmd_data_vec.begin(); lmd_data_iter != my_lmd_data_vec.end();
			lmd_data_iter++) {
		if (lmd_data_iter->getFitResults().size() > 0)
			lmd_data_iter->saveToRootFile();
	}

	// =============================== END STEP 2 =============================== //

	// -----   Finish   -------------------------------------------------------
	cout << endl << endl;
	cout << "Application finished successfully." << endl;

	fdata->Close();
	if (facc)
		facc->Close();
	// ------------------------------------------------------------------------
}

void displayInfo() {
	// display info
	cout << "Required arguments are: " << endl;
	cout << "-d [path to data]" << endl;
	cout << "Optional arguments are: " << endl;
	cout << "-a [path to box gen data] (acceptance)" << endl;
}

int main(int argc, char* argv[]) {
	bool is_data_set = false, is_acc_set = false;
	TString data_path;
	TString acc_path = "";
	int c;

	while ((c = getopt(argc, argv, "ha:d:")) != -1) {
		switch (c) {
			case 'a':
				acc_path = optarg;
				is_acc_set = true;
				break;
			case 'd':
				data_path = optarg;
				is_data_set = true;
				break;
			case '?':
				if (optopt == 'd' || optopt == 'a')
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

	if (is_data_set)
		runLumiFit(data_path, acc_path);
	else
		displayInfo();
	return 0;
}
