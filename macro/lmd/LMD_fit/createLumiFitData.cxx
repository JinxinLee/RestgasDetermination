/*
 * This is the application that generates the Lmd data objects, which can be
 * used to determine the luminosity. This application primarily uses the
 * #PndLmdDataFacade class to easily create lmd fit data objects and fill them
 * with data. General information about the individual classes of the LmdFit
 * framework can be found in the doxygen manual.
 * Run it with argument -h to get running help:
 * 
 * ./createLumiFitData -h
 */

#include "PndLmdDataFacade.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"
#include "DataStructs.h"

#include "TString.h"
#include "TFile.h"

#include <vector>
#include <iostream>

void createLumiFitData(std::string input_file_dir, const double mom, int mode,
		int num_events, const double generated_luminosity_per_event,
		const int verboseLevel = 0) {
	std::cout << "Running LmdFit data reader....\n";

	// A lmd data facade class that helps to construct and fill lmd data objects
	PndLmdDataFacade lmd_data_facade;

	// create data reader instance
	//PndLmdSeperateDataReader data_reader;
	PndLmdCombinedDataReader data_reader;

	// set data reader to be used
	lmd_data_facade.setDataReader(&data_reader);

	// set lab momentum
	lmd_data_facade.setLabMomentum(mom);

	// set some dimension parameters
	lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setUnitPrefix(
			LumiFit::MILLI);
	lmd_data_facade.getPrimaryDimensionTemplate().dimension_options.dimension_type =
			LumiFit::THETA;
	lmd_data_facade.getPrimaryDimensionTemplate().bins = 100;
	lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setRangeLow(
			0.5);
	lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setRangeHigh(
			15.0);
	lmd_data_facade.getPrimaryDimensionTemplate().dimension_options.track_param_type =
			LumiFit::IP;

	lmd_data_facade.setCurrentReferenceLuminosityPerEvent(
			generated_luminosity_per_event);

	// add input directory to data facade
	lmd_data_facade.addDataDirectory(input_file_dir);

	TString out;

	if (mode == 0) {
		// ---- dpm or elastic data ---- //

		// ---- Output file -------------------------------------------------------
		out = input_file_dir + "/lmd_data.root";
		// ------------------------------------------------------------------------

		// create angular data object bundle
		lmd_data_facade.create1DAngularDataBundle(num_events);
	} else if (mode == 1) {
		// ---- acceptance or box gen data ---- //
		lmd_data_facade.getPrimaryDimensionTemplate().bins = 100;
		lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setRangeLow(
				0.5 + 0.0725);
		lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setRangeHigh(
				15.0 + 0.0725);

		// ---- Output file -------------------------------------------------------
		out = input_file_dir + "/lmd_acc_data.root";
		// ------------------------------------------------------------------------

		// create acceptance
		lmd_data_facade.createAcceptance1D(num_events);
	} else if (2 == mode) {
		// ---- create lmd resolution objects from box gen data ---- //

		// ---- Output file -------------------------------------------------------
		out = input_file_dir + "/lmd_res_data.root";
		// ------------------------------------------------------------------------

		lmd_data_facade.getPrimaryDimensionTemplate().bins = 400;
		lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setRangeLow(
				-2.0);
		lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setRangeHigh(
				2.0);
		lmd_data_facade.getPrimaryDimensionTemplate().dimension_options.track_param_type =
				LumiFit::IP;

		lmd_data_facade.getSecondaryDimensionTemplate().dimension_options.dimension_type =
				LumiFit::PHI;
		lmd_data_facade.getSecondaryDimensionTemplate().bins = 20;
		lmd_data_facade.getSecondaryDimensionTemplate().dimension_range.setRangeLow(
				-0.2);
		lmd_data_facade.getSecondaryDimensionTemplate().dimension_range.setRangeHigh(
				0.2);

		lmd_data_facade.getPrimarySelectionDimensionBundleTemplate().dimension_options.track_type =
				LumiFit::MC;
		lmd_data_facade.getPrimarySelectionDimensionBundleTemplate().dimension_range.setRangeLow(
				1.0); //mrad
		lmd_data_facade.getPrimarySelectionDimensionBundleTemplate().dimension_range.setRangeHigh(
				15.0);
		lmd_data_facade.getSecondarySelectionDimensionBundleTemplate().dimension_range.setRangeLow(
				-TMath::Pi());
		lmd_data_facade.getSecondarySelectionDimensionBundleTemplate().dimension_range.setRangeHigh(
				TMath::Pi());
		lmd_data_facade.getPrimarySelectionDimensionBundleTemplate().bins = 280;
		lmd_data_facade.getSecondarySelectionDimensionBundleTemplate().bins = 1;

		lmd_data_facade.create1DAngularResolutionDataBundle(num_events);
	} else if (3 == mode) {
		// ---- ip position data ---- //

		// ---- Output file -------------------------------------------------------
		out = input_file_dir + "/lmd_vertex_data.root";
		// ------------------------------------------------------------------------

		lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setUnitPrefix(
				LumiFit::NONE);
		lmd_data_facade.getPrimaryDimensionTemplate().bins = 200;
		lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setRangeLow(
				-2.0);
		lmd_data_facade.getPrimaryDimensionTemplate().dimension_range.setRangeHigh(
				2.0);

		// create angular data object bundle and register in data reader
		lmd_data_facade.create1DVertexDataBundle(num_events);
	}

	// register lmd data objects and fill with data
	lmd_data_facade.fillAll();

	// -----   Finish   -------------------------------------------------------
	// save the lmd data objects

	TFile f(out, "RECREATE");

	std::cout << "Saving data....\n";

	std::vector<PndLmdData> my_lmd_data_vec = lmd_data_facade.getLmdDatas();
	std::vector<PndLmdAcceptance> my_lmd_acc_vec =
			lmd_data_facade.getLmdAcceptances();
	std::vector<PndLmdResolution> my_lmd_res_vec =
			lmd_data_facade.getLmdResolutions();

	for (unsigned int i = 0; i < my_lmd_data_vec.size(); i++) {
		my_lmd_data_vec[i].saveToRootFile();
	}
	for (unsigned int i = 0; i < my_lmd_acc_vec.size(); i++) {
		my_lmd_acc_vec[i].saveToRootFile();
	}
	for (unsigned int i = 0; i < my_lmd_res_vec.size(); i++) {
		my_lmd_res_vec[i].saveToRootFile();
	}

	std::cout << std::endl << std::endl;
	std::cout << "Application finished successfully." << std::endl;
	std::cout << std::endl;

	f.Close();
	// ------------------------------------------------------------------------
}

void displayInfo() {
	// display info
	std::cout << "Required arguments are: " << std::endl;
	std::cout << "-m [pbar momentum]" << std::endl;
	std::cout << "-t [type of data] (0 = data, 1 = acc, 2 = res, 3 = ip)"
			<< std::endl;
	std::cout << "-p [path to data]" << std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-n [number of events to process] "
			"(default 0: all data found will be processed)" << std::endl;
	std::cout << "-g [generated luminosity]" << std::endl;
	std::cout << std::endl;
	std::cout
			<< "Note: the parameter -g is the generated luminosity. In case you do NOT \n"
					"specify this value it will be set to -1.0 and there is no performance \n"
					"validation possible, but only luminosity determination. This should be \n"
					"the case only for real data!" << std::endl;
}

int main(int argc, char* argv[]) {
	int data_type_flag = -1;

	bool is_mom_set = false, is_gen_lumi_set = false, is_data_path_set = false;
	double momentum = -1.0;
	unsigned int num_events = 0;
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
