/*
 * This is the application which should be used for luminosity fitting (luminosity extraction procedure)
 * General information about the individual classes of the LmdFit framework can be
 * found in the doxygen manual
 * Run it like this:
 * 
 * ./runLumiFit beam momentum path_to_lmd_data path_to_lmd_acceptance_data
 *
 * This macro is basically a 3 step process and each step is explained in more detail
 * below:
 * step1: get LmdData
 * step2: create fit options that specify every detail about the fit
 * step3: perform the fit and store the results
 */

#include "PndLmdLumiHelper.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"
#include "PndLmdLumiFitOptions.h"

#include <vector>

#include "TFile.h"
#include "TString.h"

void runLumiFit(const double mom, TString input_file_dir,
		TString acceptance_file_dir) {
	std::cout << "Running LumiFit....\n";

	// ============================== BEGIN STEP 1 ============================== //

	// A small helper class that helps to construct lmddata objects
	PndLmdLumiHelper lumifit_helper;

	// ------ get files -------------------------------------------------------
	TFile *fdata = new TFile(input_file_dir + "/lmd_data.root", "UPDATE");
	TFile *facc = new TFile(acceptance_file_dir + "/lmd_acc_data.root", "OPEN");
	// ------------------------------------------------------------------------

	// get lmd data and objects from files
	std::vector<PndLmdData*> my_lmd_data_vec = lumifit_helper.getDataFromFile(
			fdata);
	std::vector<PndLmdAcceptance*> my_lmd_acc_vec =
			lumifit_helper.getAcceptanceFromFile(facc);

	// =============================== END STEP 1 =============================== //

	// ============================== BEGIN STEP 2 ============================== //
	/* construct fit options:
	 * Simply construct PndLmdLumiFitOptions objects which has the following constructor
	 * parameters (there are also constructors with less parameters, then you have to use
	 * the setter functions for all of the options!)
	 * param 1: bit_flag_options is a number in which each digit represents a binary flag @see PndLmdLumiFitOptions(int)
	 * ((
	 * bit 1 turns on resolution smearing,
	 * bit 2 turns on acceptance correction,
	 * bit 3 turns on 2d fit
	 * bit 4 turns on raw fit (momentum transfer)
	 * example: 1d angular fit with acceptance correction but no resolution smearing
	 * -> 0 * 2^0 + 1 * 2^1 + 0 * 2^2 + 0 * 2^3  = 2
	 * ))
	 * param 2: free_parameters_code_ is a bitcode specifying which parameter is free in the fit procedure
	 * ((
	 * luminosity will always be fitted,
	 * bit 1 turns on sigma_total as a fit parameter,
	 * bit 2 turns on rho as a fit parameter,
	 * bit 3 turns on b as a fit parameter
	 * example: to fit luminosity, sigma_total and b
	 * -> 1 * 2^0 + 0 * 2^1 + 1 * 2^2 = 5
	 * ))
	 * param 3: smearing_type_ specifies which model is used for the smearing (0 = gaussian, 1 = double gaussian)
	 * param 4: acc_intpol_type_ specifies the interpolation type of the acceptance (0 =
	 * constant, 1 = linear-spline, 2 = cubic-spline)
	 * param 5: plab
	 * param 6: theta_fit_range_low_ used as the new lower fit range for theta
	 * param 7: theta_fit_range_high_ used as the new upper fit range for theta
	 * param 8: phi_fit_range_low_ used as the new lower fit range for phi <-- optional
	 * param 9: phi_fit_range_high_ used as the new upper fit range for phi <-- optional
	 */
	std::vector<PndLmdLumiFitOptions*> fit_options_vec;
	PndLmdLumiFitOptions *temp_fit_opt;

	double fit_range_low = 4.0;
	double fit_range_high = 8.0;

	TString resolution_parametrization_file_url = acceptance_file_dir
			+ "/resolution_params_1.root";

	LumiFit::LmdBinaryFitOptions bit_fit_opt(0);
	bit_fit_opt.setFitAsRaw(true);
	temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom,
			fit_range_low, fit_range_high);
	temp_fit_opt->setResolutionParametrizationFileUrl(
			resolution_parametrization_file_url);
	fit_options_vec.push_back(temp_fit_opt);

	bit_fit_opt.setFitAsRaw(false);
	temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom,
			fit_range_low, fit_range_high);
	temp_fit_opt->setResolutionParametrizationFileUrl(
			resolution_parametrization_file_url);
	fit_options_vec.push_back(temp_fit_opt);

	bit_fit_opt.setAcceptanceCorrMode(true);
	temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom,
			fit_range_low, fit_range_high);
	temp_fit_opt->setResolutionParametrizationFileUrl(
			resolution_parametrization_file_url);
	fit_options_vec.push_back(temp_fit_opt);

	temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom,
			fit_range_low, fit_range_high);
	temp_fit_opt->setResolutionParametrizationFileUrl(
			resolution_parametrization_file_url);
	temp_fit_opt->getDataBinaryOptions().setSmearingMode(true);
	fit_options_vec.push_back(temp_fit_opt);
	std::cout << temp_fit_opt->getDataBinaryOptions().getBinaryOptions()
			<< std::endl;

	bit_fit_opt.setSmearingMode(true);
	temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom,
			fit_range_low, fit_range_high);
	temp_fit_opt->setResolutionParametrizationFileUrl(
			resolution_parametrization_file_url);
	fit_options_vec.push_back(temp_fit_opt);

	// =============================== END STEP 2 =============================== //

	// ============================== BEGIN STEP 3 ============================== //

	// perform luminosity fits
	// in this case we do all combinations of data, acceptances and fit options...
	// you probably do NOT want this so change it according to your needs
	std::cout << "\nPerforming Luminosity Fits....\n";
	for (int i = 0; i < my_lmd_data_vec.size(); i++) {
		for (int j = 0; j < my_lmd_acc_vec.size(); j++) {
			for (int k = 0; k < fit_options_vec.size(); k++) {
				my_lmd_data_vec[i]->Fit(my_lmd_acc_vec[j], fit_options_vec[k]);
			}
		}
	}

	// save fit results by just saving the lmd data objects. They contain both
	// the data and the fit results so from this objects the plotting macro can
	// construct some nice plots for you!
	std::cout << "Saving data....\n";
	for (int i = 0; i < my_lmd_data_vec.size(); i++) {
		my_lmd_data_vec[i]->saveToRootFile(fdata);
	}

	// =============================== END STEP 3 =============================== //

	// -----   Finish   -------------------------------------------------------
	std::cout << std::endl << std::endl;
	std::cout << "Application finished successfully." << std::endl;

	fdata->Close();
	facc->Close();
	// ------------------------------------------------------------------------
}

void displayInfo() {
	// display info
	std::cout << "Required arguments are: " << std::endl;
	std::cout << "-m [pbar momentum]" << std::endl;
	std::cout << "-d [path to data]" << std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-a [path to box gen data] (acceptance)" << std::endl;
}

int main(int argc, char* argv[]) {
	bool is_mom_set = false, is_data_set = false, is_acc_set = false;
	double momentum = -1.0;
	TString data_path;
	TString acc_path = "";
	int c;

	while ((c = getopt(argc, argv, "hm:a:d:")) != -1) {
		switch (c) {
		case 'm':
			momentum = atof(optarg);
			is_mom_set = true;
			break;
		case 'a':
			acc_path = optarg;
			is_acc_set = true;
			break;
		case 'd':
			data_path = optarg;
			is_data_set = true;
			break;
		case '?':
			if (optopt == 't' || optopt == 'p' || optopt == 'm' || optopt == 'n'
					|| optopt == 'g')
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

	if (is_mom_set && is_data_set)
		runLumiFit(momentum, data_path, acc_path);
	else
		displayInfo();
	return 0;
}
