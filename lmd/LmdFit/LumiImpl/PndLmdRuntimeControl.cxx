/*
 * PndLmdRuntimeControl.cxx
 *
 *  Created on: Dec 17, 2014
 *      Author: steve
 */

#include "PndLmdRuntimeControl.h"
#include "data/PndLmdAngularData.h"
#include "data/PndLmdVertexData.h"
#include "data/PndLmdAcceptance.h"

#include <iostream>
#include <vector>

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

using std::cout;
using std::endl;
using std::string;
using boost::filesystem::path;

PndLmdRuntimeControl::PndLmdRuntimeControl() {
}

PndLmdRuntimeControl::~PndLmdRuntimeControl() {
}

void PndLmdRuntimeControl::initRuntimeConfiguration(
		const std::string &config_url) {
	lmd_runtime_config.initFitFacadeFromConfigFile(config_url);
}

void PndLmdRuntimeControl::setFitPaths(const std::string& input_file_dir,
		const std::string& acceptance_file_dir,
		const std::string& reference_acceptance_file_dir) {
	lmd_runtime_config.elastic_data_input_directory = input_file_dir;
	lmd_runtime_config.acceptance_resolution_input_directory =
			acceptance_file_dir;
	lmd_runtime_config.reference_acceptance_resolution_input_directory =
			reference_acceptance_file_dir;
}

void PndLmdRuntimeControl::doLuminosityFits() {
	cout << "Running LumiFit...." << endl;

// ============================== BEGIN STEP 1 ============================== //

	std::stringstream hs;
	hs << lmd_runtime_config.elastic_data_input_directory << "/"
			<< lmd_runtime_config.elastic_data_name;

// ------ get files -------------------------------------------------------
	TFile *fdata = new TFile(hs.str().c_str(), "READ");
	TFile *facc = 0;
	if (lmd_runtime_config.acceptance_resolution_input_directory != "") {
		hs.str("");
		hs << lmd_runtime_config.acceptance_resolution_input_directory << "/"
				<< lmd_runtime_config.acc_data_name;
		facc = new TFile(hs.str().c_str(), "READ");
	}
	TFile *frefacc = 0;
	if (lmd_runtime_config.reference_acceptance_resolution_input_directory
			!= "") {
		hs.str("");
		hs << lmd_runtime_config.reference_acceptance_resolution_input_directory
				<< "/" << lmd_runtime_config.acc_data_name;
		frefacc = new TFile(hs.str().c_str(), "READ");
	}

// ------------------------------------------------------------------------

// get lmd data and objects from files
	vector<PndLmdAngularData> my_lmd_data_vec = lmd_data_facade.getDataFromFile<
			PndLmdAngularData>(fdata);

	vector<PndLmdAcceptance> my_lmd_acc_vec = lmd_data_facade.getDataFromFile<
			PndLmdAcceptance>(facc);
	vector<PndLmdAcceptance> lmd_ref_acc_vec = lmd_data_facade.getDataFromFile<
			PndLmdAcceptance>(frefacc);

// output file
	hs.str("");
	hs << lmd_runtime_config.elastic_data_input_directory << "/"
			<< lmd_runtime_config.fitted_elastic_data_name;
	TFile *ffitteddata = new TFile(hs.str().c_str(), "RECREATE");

// =============================== END STEP 1 =============================== //

// ============================== BEGIN STEP 2 ============================== //

// add acceptance data to pools
// the corresponding acceptances to the data will automatically be taken
// if not found then this fit is skipped
	lmd_fit_facade.addAcceptencesToPool(my_lmd_acc_vec);
	lmd_fit_facade.addReferenceAcceptancesToPool(lmd_ref_acc_vec);

	lmd_fit_facade.setModelFitOptions(lmd_runtime_config.fit_model_options);

	lmd_fit_facade.setEstimatorOptions(lmd_runtime_config.est_opt);
	lmd_fit_facade.doLmdLumiFits(my_lmd_data_vec);

// save fit results by just saving the lmd data objects. They contain both
// the data and the fit results so from this objects the plotting macro can
// construct some nice plots for you!
	cout << "Saving data...." << endl;
	ffitteddata->cd();

	lmd_fit_facade.saveFittedObjectsToFile();

	ffitteddata->Close();
	fdata->Close();
	if (facc)
		facc->Close();

// =============================== END STEP 2 =============================== //

// -----   Finish   -------------------------------------------------------
	cout << endl << endl;
	cout << "Luminosity fitting finished successfully." << endl;
// ------------------------------------------------------------------------
}

