#include "data/PndLmdDataFacade.h"
#include "data/PndLmdHistogramData.h"
#include "fit/data/DataStructs.h"

#include "TString.h"
#include "TFile.h"

#include <vector>
#include <iostream>

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

using boost::filesystem::path;

void createPhiSlicedAnalysisLmdData(std::string input_dir_path,
		std::string filelist_path, std::string output_dir_path, const double mom, int num_events, const int verboseLevel = 0) {
	std::cout << "Running LmdFit data reader....\n";

	// A lmd data facade class that helps to construct and fill lmd data objects
	PndLmdDataFacade lmd_data_facade;

	// create data reader instance
	//PndLmdSeperateDataReader data_reader;
	PndLmdCombinedDataReader data_reader;

	// set data reader to be used
	lmd_data_facade.setDataReader(&data_reader);

	// set lab momentum
	lmd_data_facade.lab_momentum = mom;

	LumiFit::LmdDimension primary_selection_dimension_bundle_template;
	primary_selection_dimension_bundle_template.dimension_options.dimension_type =
			LumiFit::PHI_FIRST_LMD_PLANE;
	primary_selection_dimension_bundle_template.dimension_options.track_param_type =
			LumiFit::IP;
	primary_selection_dimension_bundle_template.dimension_options.track_type =
			LumiFit::MC;
	primary_selection_dimension_bundle_template.dimension_range.setRangeLow(
			-TMath::Pi());
	primary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
			TMath::Pi());
	primary_selection_dimension_bundle_template.bins = 10; // 10 modules

	lmd_data_facade.selection_dimension_bundles.push_back(
			primary_selection_dimension_bundle_template);
	lmd_data_facade.createSelectionDimensionCombinations();

	std::string filename_extension("");

	// add input directory to data facade
	if (filelist_path.compare("") == 0)
		lmd_data_facade.addDataDirectory(input_dir_path);
	else {
		lmd_data_facade.addFileList(filelist_path);
		const boost::regex my_filename_filter("filelist_(\\d*).txt",
				boost::regex::extended | boost::regex::icase);

		boost::smatch dwhat;
		if (boost::regex_search(path(filelist_path).filename().string(), dwhat,
				my_filename_filter)) {
			filename_extension = "_";
			filename_extension += std::string(dwhat[1]);
		}
	}

	TString output_filename_resolution_data("");

	// ---- Output file -------------------------------------------------------
	output_filename_resolution_data = output_dir_path + "/lmd_phi_corr_data"
			+ filename_extension + ".root";
	// ------------------------------------------------------------------------

	// set some dimension parameters
	lmd_data_facade.primary_dimension_template.dimension_options.track_type =
			LumiFit::MC;
	lmd_data_facade.primary_dimension_template.dimension_options.dimension_type =
			LumiFit::PHI;
	lmd_data_facade.primary_dimension_template.dimension_options.track_param_type =
			LumiFit::IP;
	lmd_data_facade.primary_dimension_template.bins = 400;
	lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
			-TMath::Pi());
	lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
			TMath::Pi());

	lmd_data_facade.secondary_dimension_template.dimension_options.dimension_type =
			LumiFit::PHI_FIRST_LMD_PLANE;
	lmd_data_facade.secondary_dimension_template.bins = 400;
	lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
			-TMath::Pi());
	lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
			TMath::Pi());

	lmd_data_facade.createHistogramData2D("phi_mc_correlation", num_events);

	lmd_data_facade.secondary_dimension_template.dimension_options.track_type =
			LumiFit::RECO;
	lmd_data_facade.secondary_dimension_template.dimension_options.dimension_type =
			LumiFit::PHI;
	lmd_data_facade.secondary_dimension_template.dimension_options.track_param_type =
			LumiFit::IP;
	lmd_data_facade.secondary_dimension_template.bins = 400;
	lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
			-TMath::Pi());
	lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
			TMath::Pi());

	lmd_data_facade.createHistogramData2D("phi_reco_correlation", num_events);

	// register lmd data objects and fill with data
	lmd_data_facade.fillAll();

	// -----   Finish   -------------------------------------------------------
	// save the lmd data objects

	std::cout << "Saving data....\n";

	std::vector<PndLmdHistogramData> my_lmd_hist_vec =
			lmd_data_facade.getLmdHistogramData();

	if (my_lmd_hist_vec.size() > 0) {
		TFile f(output_filename_resolution_data, "RECREATE");
		for (unsigned int i = 0; i < my_lmd_hist_vec.size(); i++) {
			my_lmd_hist_vec[i].saveToRootFile();
		}
		f.Close();
	}

	std::cout << std::endl << std::endl;
	std::cout << "Application finished successfully." << std::endl;
	std::cout << std::endl;

	// ------------------------------------------------------------------------
}

void displayInfo() {
	// display info
	std::cout << "Required arguments are: " << std::endl;
	std::cout << "-m [pbar momentum]" << std::endl;
	std::cout << "-p [input directory path]" << std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-f [filelist path]" << std::endl;
	std::cout << "-o [output directory path]" << std::endl;
	std::cout << "-n [number of events to process] "
			"(default 0: all data found will be processed)" << std::endl;
	std::cout << std::endl;
}

int main(int argc, char* argv[]) {
	bool is_mom_set = false, is_data_path_set = false, is_filelist_path_set =
			false, is_output_data_path_set = false;
	double momentum = -1.0;
	unsigned int num_events = 0;
	std::string data_path;
	std::string output_dir_path;
	std::string filelist_path("");
	int c;

	while ((c = getopt(argc, argv, "hm:f:o:n:st:p:c:")) != -1) {
		switch (c) {
			case 'm':
				momentum = atof(optarg);
				is_mom_set = true;
				break;
			case 'f':
				filelist_path = optarg;
				is_filelist_path_set = true;
				break;
			case 'o':
				output_dir_path = optarg;
				is_output_data_path_set = true;
				break;
			case 'n':
				num_events = atoi(optarg);
				break;
			case 'p':
				data_path = optarg;
				is_data_path_set = true;
				break;
			case '?':
				if (optopt == 'p' || optopt == 'm' || optopt == 'n')
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

	bool skip_program = false;
	if (!is_output_data_path_set && is_filelist_path_set) {
		std::cerr
				<< "Please specify an output directory via the -o option when using a filelist path as input!"
				<< std::endl;
		skip_program = true;
	}
	if (!(is_mom_set && is_data_path_set))
		skip_program = true;

	if (skip_program)
		displayInfo();
	else {
		if (!is_output_data_path_set)
			output_dir_path = data_path;
		createPhiSlicedAnalysisLmdData(data_path, filelist_path, output_dir_path,
				momentum, num_events);
		return 0;
	}
}

