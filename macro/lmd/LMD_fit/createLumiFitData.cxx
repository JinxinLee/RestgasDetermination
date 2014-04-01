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
#include "PndLmdAngularData.h"
#include "PndLmdVertexData.h"
#include "PndLmdResolution.h"
#include "PndLmdAcceptance.h"
#include "DataStructs.h"

#include "TString.h"
#include "TFile.h"

#include <vector>
#include <iostream>

void createLumiFitData(std::string input_file_dir, const double mom,
		std::string& data_types, int num_events,
		const double total_elastic_cross_section, const int verboseLevel = 0) {
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

	// set some dimension parameters
	lmd_data_facade.primary_dimension_template.dimension_range.setUnitPrefix(
			LumiFit::MILLI);
	lmd_data_facade.primary_dimension_template.dimension_options.dimension_type =
			LumiFit::THETA;
	lmd_data_facade.primary_dimension_template.bins = 100;
	lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(0.5);
	lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(15.0);
	lmd_data_facade.primary_dimension_template.dimension_options.track_param_type =
			LumiFit::IP;

	lmd_data_facade.current_reference_luminosity_per_event = 1.0
			/ total_elastic_cross_section;

	// add input directory to data facade
	lmd_data_facade.addDataDirectory(input_file_dir);

	TString output_filename_angular_data("");
	TString output_filename_acceptance_data("");
	TString output_filename_resolution_data("");
	TString output_filename_vertex_data("");

	if (data_types.find("a") != std::string::npos) {
		// ---- dpm or elastic data ---- //

		// ---- Output file -------------------------------------------------------
		output_filename_angular_data = input_file_dir + "/lmd_data.root";
		// ------------------------------------------------------------------------

		// create angular data object bundle
		lmd_data_facade.create1DAngularDataBundle(num_events);
	}
	if (data_types.find("e") != std::string::npos) {
		// ---- acceptance or box gen data ---- //
		lmd_data_facade.primary_dimension_template.bins = 100;
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
				0.5 + 0.0725);
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
				15.0 + 0.0725);

		// ---- Output file -------------------------------------------------------
		output_filename_acceptance_data = input_file_dir + "/lmd_acc_data.root";
		// ------------------------------------------------------------------------

		// create acceptance
		lmd_data_facade.createAcceptance1D(num_events);
	}
	if (data_types.find("r") != std::string::npos) {
		// ---- create lmd resolution objects from box gen data ---- //

		// ---- Output file -------------------------------------------------------
		output_filename_resolution_data = input_file_dir + "/lmd_res_data.root";
		// ------------------------------------------------------------------------

		lmd_data_facade.primary_dimension_template.bins = 400;
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
				-2.0);
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
				2.0);
		lmd_data_facade.primary_dimension_template.dimension_options.track_param_type =
				LumiFit::IP;

		lmd_data_facade.secondary_dimension_template.dimension_options.dimension_type =
				LumiFit::PHI;
		lmd_data_facade.secondary_dimension_template.bins = 20;
		lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
				-0.2);
		lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
				0.2);

		lmd_data_facade.primary_selection_dimension_bundle_template.dimension_options.track_type =
				LumiFit::MC;
		lmd_data_facade.primary_selection_dimension_bundle_template.dimension_range.setRangeLow(
				1.0); //mrad
		lmd_data_facade.primary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
				15.0);
		lmd_data_facade.secondary_selection_dimension_bundle_template.dimension_options.track_type =
				LumiFit::MC;
		lmd_data_facade.secondary_selection_dimension_bundle_template.dimension_range.setRangeLow(
				-TMath::Pi());
		lmd_data_facade.secondary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
				TMath::Pi());
		lmd_data_facade.primary_selection_dimension_bundle_template.bins = 280;
		lmd_data_facade.secondary_selection_dimension_bundle_template.bins = 1;

		lmd_data_facade.create1DAngularResolutionDataBundle(num_events);
	}
	if (data_types.find("v") != std::string::npos) {
		// ---- ip position data ---- //

		// get simulation ip distribution properties
		LumiFit::LmdSimIPParameters true_ip_values =
				lmd_data_facade.readSimulationIPParameters(input_file_dir);
		true_ip_values.print();
		lmd_data_facade.current_simulation_ip_parameters = true_ip_values;

		// ---- Output file -------------------------------------------------------
		output_filename_vertex_data = input_file_dir + "/lmd_vertex_data.root";
		// ------------------------------------------------------------------------

		lmd_data_facade.primary_dimension_template.dimension_range.setUnitPrefix(
				LumiFit::NONE);
		lmd_data_facade.primary_dimension_template.bins = 200;
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
				-2.0);
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
				2.0);

		// create angular data object bundle and register in data reader
		lmd_data_facade.create1DVertexDataBundle(num_events);
	}

	// register lmd data objects and fill with data
	lmd_data_facade.fillAll();

	// -----   Finish   -------------------------------------------------------
	// save the lmd data objects

	std::cout << "Saving data....\n";

	std::vector<PndLmdAngularData> my_lmd_data_vec =
			lmd_data_facade.getLmdAngularData();
	std::vector<PndLmdVertexData> my_lmd_vertex_data_vec =
			lmd_data_facade.getLmdVertexData();
	std::vector<PndLmdAcceptance> my_lmd_acc_vec =
			lmd_data_facade.getLmdAcceptances();
	std::vector<PndLmdResolution> my_lmd_res_vec =
			lmd_data_facade.getLmdResolutions();

	if (my_lmd_data_vec.size() > 0) {
		TFile f(output_filename_angular_data, "RECREATE");
		for (unsigned int i = 0; i < my_lmd_data_vec.size(); i++) {
			my_lmd_data_vec[i].saveToRootFile();
		}
		f.Close();
	}
	if (my_lmd_vertex_data_vec.size() > 0) {
		TFile f(output_filename_vertex_data, "RECREATE");
		for (unsigned int i = 0; i < my_lmd_vertex_data_vec.size(); i++) {
			my_lmd_vertex_data_vec[i].saveToRootFile();
		}
		f.Close();
	}
	if (my_lmd_acc_vec.size() > 0) {
		TFile f(output_filename_acceptance_data, "RECREATE");
		for (unsigned int i = 0; i < my_lmd_acc_vec.size(); i++) {
			my_lmd_acc_vec[i].saveToRootFile();
		}
		f.Close();
	}
	if (my_lmd_res_vec.size() > 0) {
		TFile f(output_filename_resolution_data, "RECREATE");
		for (unsigned int i = 0; i < my_lmd_res_vec.size(); i++) {
			my_lmd_res_vec[i].saveToRootFile();
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
	std::cout
			<< "-t [type of data to create] (a = angular, e = efficiency, r = resolution, v = vertex)"
			<< std::endl;
	std::cout << "-p [path to data]" << std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-n [number of events to process] "
			"(default 0: all data found will be processed)" << std::endl;
	std::cout << "-c [total elastic cross section]" << std::endl;
	std::cout << std::endl;
	std::cout
			<< "Note: The type value is specified as a string, in which the 4 letters\n"
					"a, e, r, v can be concatenated freely. For just a single data type\n"
					"the values should be a, e, r, v. For a combination i.e\n"
					"angular vertex data, one should use av or va. Other additional\n"
					"characters will be ignored, so can be specified without any effect!\n"
					"\n"
					"the parameter -c is the total elastic cross section to estimate\n"
					"the generated luminosity. In case you do NOT specify this value it\n"
					"will be set to -1.0 and there is no performance validation possible,\n"
					"but only luminosity determination. This should be the case only for\n"
					"real data!" << std::endl;
}

bool checkDataType(std::string& data_type) {
	bool is_valid = false;
	if (data_type.find("a") != std::string::npos) {
		is_valid = true;
	} else if (data_type.find("e") != std::string::npos) {
		is_valid = true;
	} else if (data_type.find("r") != std::string::npos) {
		is_valid = true;
	} else if (data_type.find("v") != std::string::npos) {
		is_valid = true;
	}
	return is_valid;
}

int main(int argc, char* argv[]) {
	bool is_mom_set = false, is_cross_section_set = false, is_data_path_set =
			false;
	double momentum = -1.0;
	std::string data_type = "";
	unsigned int num_events = 0;
	double cross_section = 1.0;
	std::string data_path;
	int c;

	while ((c = getopt(argc, argv, "hm:n:t:p:c:")) != -1) {
		switch (c) {
			case 'm':
				momentum = atof(optarg);
				is_mom_set = true;
				break;
			case 'n':
				num_events = atoi(optarg);
				break;
			case 'c':
				cross_section = atof(optarg);
				is_cross_section_set = true;
				break;
			case 't':
				data_type = optarg;
				break;
			case 'p':
				data_path = optarg;
				is_data_path_set = true;
				break;
			case '?':
				if (optopt == 't' || optopt == 'p' || optopt == 'm' || optopt == 'n'
						|| optopt == 'c')
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

	if (checkDataType(data_type) && is_mom_set && is_data_path_set)
		createLumiFitData(data_path, momentum, data_type, num_events,
				cross_section);
	else
		displayInfo();

	return 0;
}
