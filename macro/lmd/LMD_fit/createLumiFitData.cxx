/*
 * This is the application that generates the Lmd data objects, which can be used to determine the
 * luminosity. General information about the individual classes of the LmdFit framework can be
 * found in the doxygen manual
 * Run it with argument -h to get running help:
 * 
 * ./createLumiFitData -h
 *
 * This macro is basically a 2 step process and each step is explained in more detail
 * below:
 * step1: create the appropriate data objects and register these objects in a lumi helper instance
 * step2: fill the registered data objects with data from the given path
 */

#include "PndLmdLumiHelper.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"

#include "TString.h"
#include "TFile.h"

#include <vector>
#include <iostream>

void createLumiFitData(std::string input_file_dir, const double mom, int mode,
		int nEvents = -1, const double generated_luminosity_per_event = -1.0,
		const int verboseLevel = 0) {
	std::cout << "Running LumiFit....\n";

	// A small helper class that helps to construct lmd data objects
	PndLmdLumiHelper lumifit_helper;

	// ============================== BEGIN STEP 1 ============================== //
	/**
	 * There are two types of data classes PndLmdData (elastic data) and PndLmdAcceptance (boxgen data).
	 * These objects can be easily filled with data using the helper class
	 * (PndLmdLumiHelper using registerData and fillHists()).
	 * The idea is to construct empty data objects (meaning no data are inside yet) at first and register
	 * them within in the helper instance. Of course each data object has to be unique
	 * from its options (such as number of events, data range, binning etc).
	 * Once you have all of your data objects created that you want to work with simply
	 * run fillHists() of the helper instance and all of the registerd data objects will
	 * be filled in a single swoop!
	 */

	double data_range_low = 0.5;
	double data_range_high = 20.0;

	TFile *f;

	// create a temporary vector to store the data objects
	std::vector<PndLmdData*> my_lmd_data_vec;

	// same procedure as for the "real" data
	std::vector<PndLmdAcceptance*> my_lmd_acc_vec;

	if (mode == 0) {
		// ---- dpm or elastic data ---- //

		// ---- Output file -------------------------------------------------------
		TString out = input_file_dir + "/lmd_data.root";
		f = new TFile(out, "RECREATE");
		// ------------------------------------------------------------------------

		// in this case the loop was used for different binnings, but here only a single
		// binning is actually created...
		for (int i = 10; i < 11; i = i + 1) {
			// create data object and push it into the vector
			PndLmdFit::lmd_dimension theta(i * 10, data_range_low, data_range_high); // theta dimension
			PndLmdFit::lmd_dimension phi(50, -TMath::Pi(), TMath::Pi()); // phi dimension
			PndLmdData *data = new PndLmdData(nEvents, mom, theta, phi,
					generated_luminosity_per_event);
			my_lmd_data_vec.push_back(data);
		}

		//register data objects in helper instance
		lumifit_helper.registerData(my_lmd_data_vec);

		// fill all elastic scattering histograms that are registered in the gamma helper object
		lumifit_helper.fillData(mom, input_file_dir, PndLmdLumiHelper::DATA,
				PndLmdLumiHelper::FULL);
	} else if (mode == 1) {
		// ---- acceptance or box gen data ---- //

		// ---- Output file -------------------------------------------------------
		TString out = input_file_dir + "/lmd_acc_data.root";
		f = new TFile(out, "RECREATE");
		// ------------------------------------------------------------------------

		for (int i = 10; i < 11; i = i + 1) {
			PndLmdFit::lmd_dimension theta(i * 20, data_range_low, data_range_high); // theta dimension
			PndLmdFit::lmd_dimension phi(50, -TMath::Pi(), TMath::Pi()); // phi dimension

			PndLmdAcceptance *acc = new PndLmdAcceptance(nEvents, mom, theta, phi,
					0);
			my_lmd_acc_vec.push_back(acc);
		}

		//register data objects in helper instance
		lumifit_helper.registerAcceptances(my_lmd_acc_vec);

		// fill all box gen histograms (for acceptance) that are registered in the gamma helper object
		// note: 3rd argument identifies specifies which kind of data is being read in
		lumifit_helper.fillData(mom, input_file_dir, PndLmdLumiHelper::ACCEPTANCE,
				PndLmdLumiHelper::FULL);
	}

	// -----   Finish   -------------------------------------------------------
	// save fit results by just saving the lmd data objects. They contain both
	// the data and the fit results so from this objects the plotting macro can
	// construct some nice plots for you!
	std::cout << "Saving data....\n";
	for (int i = 0; i < my_lmd_data_vec.size(); i++) {
		my_lmd_data_vec[i]->saveToRootFile(f);
	}
	for (int i = 0; i < my_lmd_acc_vec.size(); i++) {
		my_lmd_acc_vec[i]->saveToRootFile(f);
	}

	std::cout << std::endl << std::endl;
	std::cout << "Application finished successfully." << std::endl;
	std::cout << std::endl;

	if (f)
		f->Close();
	// ------------------------------------------------------------------------
}

void displayInfo() {
	// display info
	std::cout << "Required arguments are: " << std::endl;
	std::cout << "-m [pbar momentum]" << std::endl;
	std::cout << "-t [type of data] (0 = data, 1 = acc)" << std::endl;
	std::cout << "-p [path to data]" << std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-n [number of events to process] "
			"(if not specified all data found data will be processed)" << std::endl;
	std::cout << "-g [generated luminosity]" << std::endl;
	std::cout << std::endl;
	std::cout
			<< "Note: the parameter -g is the generated luminosity. In case you do NOT \n"
					"specify this value it will be set to -1.0 and there is no performance \n"
					"validation possible, but only luminsity determination. This should be \n"
					"the case only for real data!" << std::endl;
}

int main(int argc, char* argv[]) {
	int data_type_flag = -1;

	bool is_mom_set = false, is_gen_lumi_set = false, is_data_path_set = false;
	double momentum = -1.0;
	int num_events = -1;
	double gen_lumi = -1.0;
	std::string data_path;
	int c;

	while ((c = getopt(argc, argv, "hm:n:t:p:g:")) != -1) {
		switch (c) {
		case 'm':
			momentum = atof(optarg);
			is_mom_set = true;
			break;
		case 'n':
			num_events = atoi(optarg);
			break;
		case 'g':
			gen_lumi = atof(optarg);
			is_gen_lumi_set = true;
			break;
		case 't':
			data_type_flag = atoi(optarg);
			break;
		case 'p':
			data_path = optarg;
			is_data_path_set = true;
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

	if (is_mom_set && is_data_path_set)
		createLumiFitData(data_path, momentum, data_type_flag, num_events,
				gen_lumi);
	else
		displayInfo();

	return 0;
}
