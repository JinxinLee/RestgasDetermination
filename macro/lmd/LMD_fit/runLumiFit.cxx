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

#include "fit/data/DataStructs.h"
#include "data/PndLmdDataFacade.h"
#include "fit/PndLmdFitFacade.h"
#include "data/PndLmdAngularData.h"
#include "data/PndLmdAcceptance.h"

#include <vector>
#include <iostream>
#include <sstream>

#include "TFile.h"

using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

void runLumiFit(string input_file_dir, string acceptance_file_dir,
		string reference_acceptance_file_dir) {
	cout << "Running LumiFit...." << endl;

	// ============================== BEGIN STEP 1 ============================== //

	// A small helper class that helps to construct lmddata objects
	PndLmdDataFacade lmd_data_facade;

	std::stringstream hs;
	hs << input_file_dir << "/lmd_data.root";

	// ------ get files -------------------------------------------------------
	TFile *fdata = new TFile(hs.str().c_str(), "READ");
	TFile *facc = 0;
	if (acceptance_file_dir != "") {
		hs.str("");
		hs << acceptance_file_dir << "/lmd_acc_data.root";
		facc = new TFile(hs.str().c_str(), "READ");
	}
	TFile *frefacc = 0;
	if (reference_acceptance_file_dir != "") {
		hs.str("");
		hs << reference_acceptance_file_dir << "/lmd_acc_data.root";
		frefacc = new TFile(hs.str().c_str(), "READ");
	}

	// ------------------------------------------------------------------------

	// get lmd data and objects from files
	vector<PndLmdAngularData> my_lmd_data_vec = lmd_data_facade.getDataFromFile<
			PndLmdAngularData>(fdata);

	// get lmd data and objects from files
	/*	vector<PndLmdAngularData> temp_my_lmd_data_vec = lmd_data_facade.getDataFromFile<
	 PndLmdAngularData>(fdata);
	 // choose data do be fitted...
	 vector<PndLmdAngularData> my_lmd_data_vec;
	 for(unsigned int i = 0; i < temp_my_lmd_data_vec.size(); i++) {
	 if(temp_my_lmd_data_vec[i].getSelectorSet().size() == 0) {
	 my_lmd_data_vec.push_back(temp_my_lmd_data_vec[i]);
	 }
	 }*/

	vector<PndLmdAcceptance> my_lmd_acc_vec = lmd_data_facade.getDataFromFile<
			PndLmdAcceptance>(facc);
	vector<PndLmdAcceptance> lmd_ref_acc_vec = lmd_data_facade.getDataFromFile<
			PndLmdAcceptance>(frefacc);

	// output file
	hs.str("");
	hs << input_file_dir << "/lmd_fitted_data.root";
	TFile *ffitteddata = new TFile(hs.str().c_str(), "RECREATE");

	// =============================== END STEP 1 =============================== //

	// ============================== BEGIN STEP 2 ============================== //
	DataStructs::DimensionRange fit_range_theta;
	fit_range_theta.is_active = true;

	fit_range_theta.range_low = 0.002;
	fit_range_theta.range_high = 0.01;

	PndLmdFitFacade lmd_fit_facade;

	// add acceptance data to pools
	// the corresponding acceptances to the data will automatically be taken
	// if not found then this fit is skipped
	lmd_fit_facade.addAcceptencesToPool(my_lmd_acc_vec);
	lmd_fit_facade.addReferenceAcceptancesToPool(lmd_ref_acc_vec);

	if (false) { // standard 1d fitting
		LumiFit::PndLmdFitModelOptions model_opt;

		//model_opt.dpm_elastic_parts = LumiFit::ALL_RHO_B_SIGTOT;
		model_opt.dpm_elastic_parts = LumiFit::ALL;

		model_opt.acceptance_interpolation = LumiFit::SPLINE;

		// specify the resolution parametriztion file which will take care of the smearing
		hs.str("");
		hs << acceptance_file_dir << "/resolution_params_1.root";
		lmd_fit_facade.initializeResolutionModelOptionsFromParametrizationFile(
				model_opt, hs.str());

		lmd_fit_facade.setModelFitOptions(model_opt);

		// perform luminosity fits
		// only luminosity is free parameter
		//lmd_fit_facade.addFreeFitParameter("sigma_tot");
		//lmd_fit_facade.addFreeFitParameter("rho");
		//lmd_fit_facade.addFreeFitParameter("b");

		// lets perform "standard" fits first if possible
		EstimatorOptions est_opt;
		est_opt.setWithIntegralScaling(true);

		est_opt.setFitRangeX(fit_range_theta);
		lmd_fit_facade.setEstimatorOptions(est_opt);
		lmd_fit_facade.doLmdLumiFits(my_lmd_data_vec);
	}

	// special fit: acc corr. model on reco data
	// filter data for all mc acc data
	/*LumiFit::LmdDimensionOptions lmd_dim_opt;
	 lmd_dim_opt.track_type = LumiFit::MC_ACC;
	 LumiFit::Comparisons::data_primary_dimension_options_filter filter(
	 lmd_dim_opt);
	 std::vector<PndLmdAngularData> my_lmd_acc_corr_data_vec =
	 lmd_data_facade.filterData<PndLmdAngularData>(my_lmd_data_vec, filter);

	 std::cout<<"trying to do special fit..."<<std::endl;
	 if (my_lmd_acc_corr_data_vec.size() > 0) {
	 lmd_dim_opt.track_type = LumiFit::RECO;
	 LumiFit::Comparisons::data_primary_dimension_options_filter filter2(
	 lmd_dim_opt);
	 std::vector<PndLmdAngularData> my_lmd_reco_data_vec =
	 lmd_data_facade.filterData<PndLmdAngularData>(my_lmd_data_vec, filter2);
	 const std::vector<PndLmdLumiFitOptions> &fit_options_vec =
	 lmd_fit_facade.createFitOptions(my_lmd_acc_corr_data_vec[0]);

	 for (unsigned int j = 0; j < my_lmd_reco_data_vec.size(); j++) {
	 lmd_fit_facade.fitMultipleElasticPPbar(my_lmd_reco_data_vec[j],
	 fit_options_vec);
	 }
	 }*/

	/*
	 // scan lower acceptance edge
	 for (unsigned int i = 0; i < 13; i++) {
	 fit_range.range_low = (1.0 + 0.25 * i) / 1000.0;
	 fit_range.range_high = 0.01;

	 est_opt.setFitRangeX(fit_range);
	 lmd_fit_facade.setEstimatorOptions(est_opt);
	 lmd_fit_facade.doLmdLumiFits(my_lmd_data_vec);
	 }

	 // scan upper acceptance edge
	 for (unsigned int i = 0; i < 13; i++) {
	 fit_range.range_low = 1.0 / 1000.0;
	 fit_range.range_high = (8.0 + 0.25 * i) / 1000.0;

	 est_opt.setFitRangeX(fit_range);
	 lmd_fit_facade.setEstimatorOptions(est_opt);
	 lmd_fit_facade.doLmdLumiFits(my_lmd_data_vec);
	 }*/

	// 2d fits
	if (true) {
		LumiFit::PndLmdFitModelOptions fit2d_model_opt;
		fit2d_model_opt.dpm_elastic_parts = LumiFit::ALL;
		fit2d_model_opt.fit_dimension = 2;
		fit2d_model_opt.acceptance_interpolation = LumiFit::CONSTANT;
		fit2d_model_opt.divergence_smearing_active = false;

		//fit2d_model_opt.fix_beam_tilts = true;
		fit2d_model_opt.beam_tilt_x = 0.001;
		fit2d_model_opt.beam_tilt_y = 0.001;

		hs.str("");
		hs << acceptance_file_dir << "/lmd_res_data.root";

		fit2d_model_opt.resolution_parametrization_file_url = hs.str();
		hs.str("");
		hs << input_file_dir << "/lmd_data.root";

		fit2d_model_opt.elastic_reco_data_file_url = hs.str();

		lmd_fit_facade.setModelFitOptions(fit2d_model_opt);

		EstimatorOptions est_opt;
		est_opt.setWithIntegralScaling(false);

		fit_range_theta.range_low = 0.0025;
		fit_range_theta.range_high = 0.010;
		est_opt.setFitRangeX(fit_range_theta);
		lmd_fit_facade.setEstimatorOptions(est_opt);

		lmd_fit_facade.doLmdLumiFits(my_lmd_data_vec);
	}

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
	cout << "Application finished successfully." << endl;
	// ------------------------------------------------------------------------
}

void displayInfo() {
	// display info
	cout << "Required arguments are: " << endl;
	cout << "-d [path to data]" << endl;
	cout << "Optional arguments are: " << endl;
	cout << "-s module phi sliced fitting" << endl;
	cout << "-a [path to box gen data] (acceptance)" << endl;
	cout << "-r [path to reference box gen data] (acceptance)" << endl;
}

int main(int argc, char* argv[]) {
	string data_path;
	string acc_path = "";
	string ref_acc_path = "";
	bool is_data_set = false, is_acc_set = false, do_phi_slice_fitting = false;

	int c;

	while ((c = getopt(argc, argv, "hsa:r:d:")) != -1) {
		switch (c) {
			case 's':
				do_phi_slice_fitting = true;
				break;
			case 'a':
				acc_path = optarg;
				is_acc_set = true;
				break;
			case 'r':
				ref_acc_path = optarg;
				break;
			case 'd':
				data_path = optarg;
				is_data_set = true;
				break;
			case '?':
				if (optopt == 's' || optopt == 'd' || optopt == 'a' || optopt == 'r')
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
		runLumiFit(data_path, acc_path, ref_acc_path);
	else
		displayInfo();
	return 0;
}
