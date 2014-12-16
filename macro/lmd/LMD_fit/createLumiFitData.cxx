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

#include "data/PndLmdDataFacade.h"
#include "data/PndLmdAngularData.h"
#include "data/PndLmdVertexData.h"
#include "data/PndLmdAcceptance.h"
#include "fit/data/DataStructs.h"

#include "TString.h"
#include "TFile.h"

#include <vector>
#include <iostream>

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

using boost::filesystem::path;

void createLumiFitData(std::string input_dir_path, std::string filelist_path,
		std::string output_dir_path, const double mom, std::string& data_types,
		int num_events, const double total_elastic_cross_section,
		bool is_phi_slicing_enabled, const int verboseLevel = 0) {
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
	/*lmd_data_facade.primary_dimension_template.dimension_options.dimension_type =
	 LumiFit::THETA;
	 lmd_data_facade.primary_dimension_template.bins = 100;
	 lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(0.5);
	 lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(15.0);*/

	lmd_data_facade.primary_dimension_template.dimension_options.dimension_type =
			LumiFit::THETA_X;
	lmd_data_facade.primary_dimension_template.bins = 100;
	lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(-0.015);
	lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(0.015);

	lmd_data_facade.primary_dimension_template.dimension_options.track_param_type =
			LumiFit::IP;

	/*lmd_data_facade.secondary_dimension_template.dimension_options.dimension_type =
	 LumiFit::PHI;
	 lmd_data_facade.secondary_dimension_template.bins = 50;
	 lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
	 -TMath::Pi());
	 lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
	 TMath::Pi());*/
	lmd_data_facade.secondary_dimension_template.dimension_options.dimension_type =
			LumiFit::THETA_Y;
	lmd_data_facade.secondary_dimension_template.bins = 100;
	lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
			-0.015);
	lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
			0.015);
	lmd_data_facade.secondary_dimension_template.dimension_options.track_param_type =
			LumiFit::IP;

	lmd_data_facade.current_reference_luminosity_per_event = 1.0
			/ total_elastic_cross_section;

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

	TString output_filename_angular_data("");
	TString output_filename_acceptance_data("");
	TString output_filename_resolution_data("");
	TString output_filename_vertex_data("");

	if (data_types.find("a") != std::string::npos) {
		// ---- dpm or elastic data ---- //

		// ---- Output file -------------------------------------------------------
		output_filename_angular_data = output_dir_path + "/lmd_data"
				+ filename_extension + ".root";
		// ------------------------------------------------------------------------

		// create angular data object bundle
		lmd_data_facade.create2DAngularDataBundle(num_events);
	}
	if (data_types.find("e") != std::string::npos) {
		// ---- acceptance or box gen data ---- //

		// ---- Output file -------------------------------------------------------
		output_filename_acceptance_data = output_dir_path + "/lmd_acc_data"
				+ filename_extension + ".root";
		// ------------------------------------------------------------------------

		/*lmd_data_facade.primary_dimension_template.bins = 100;
		 lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
		 0.5 + 0.0725);
		 lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
		 15.0 + 0.0725);*/
		lmd_data_facade.primary_dimension_template.bins = 600;
		lmd_data_facade.secondary_dimension_template.bins = 600;

		/*unsigned int max_binning = 1000;
		 unsigned int min_binning = 100;
		 double range_diff = range_high - range_low;
		 double total_binsize_diff = range_diff / min_binning
		 - range_diff / max_binning;
		 unsigned int num_scanpoints = 10;
		 double step_binsize_diff = total_binsize_diff / num_scanpoints;
		 unsigned int binning_previous = min_binning;

		 for (unsigned int i = 0; i < num_scanpoints; i++) {
		 double binning_correction = step_binsize_diff * binning_previous
		 * binning_previous
		 / (range_diff - step_binsize_diff * binning_previous);
		 lmd_data_facade.primary_dimension_template.bins = binning_previous
		 + binning_correction;
		 lmd_data_facade.secondary_dimension_template.bins = binning_previous
		 + binning_correction;
		 std::stringstream name;
		 name << "lmd_acceptance_"
		 << lmd_data_facade.primary_dimension_template.bins << "_"
		 << lmd_data_facade.secondary_dimension_template.bins;
		 std::cout << name.str() << std::endl;
		 // create acceptance
		 lmd_data_facade.createAcceptance2D(name.str(), num_events);
		 binning_previous = binning_previous + binning_correction;
		 }*/

		LumiFit::LmdDimension secondary_selection_dimension;
		secondary_selection_dimension.dimension_options.dimension_type =
				LumiFit::SECONDARY;
		secondary_selection_dimension.dimension_range.setRangeLow(-10000.0); // primarys are negative in the secondary number
		secondary_selection_dimension.dimension_range.setRangeHigh(-0.1);
		secondary_selection_dimension.bins = 1;
		lmd_data_facade.selection_dimension_bundles.push_back(
				secondary_selection_dimension);

		lmd_data_facade.createSelectionDimensionCombinations();

		lmd_data_facade.createAcceptance2D("lmd_acceptance", num_events);

		/*binning_previous = min_binning;
		 for (unsigned int i = 0; i < num_scanpoints; i++) {
		 double binning_correction = step_binsize_diff * binning_previous
		 * binning_previous
		 / (range_diff - step_binsize_diff * binning_previous);
		 lmd_data_facade.primary_dimension_template.bins = binning_previous
		 + binning_correction;
		 lmd_data_facade.secondary_dimension_template.bins = binning_previous
		 + binning_correction;
		 std::stringstream name;
		 name << "lmd_acceptance_"
		 << lmd_data_facade.primary_dimension_template.bins << "_"
		 << lmd_data_facade.secondary_dimension_template.bins;
		 std::cout << name.str() << std::endl;
		 // create acceptance
		 lmd_data_facade.createAcceptance2D(name.str(), num_events);
		 binning_previous = binning_previous + binning_correction;
		 }*/

		//lmd_data_facade.clearSelectionDimensionMap();
	}
	if (data_types.find("r") != std::string::npos) {
		// ---- create lmd resolution objects from box gen data ---- //

		// ---- Output file -------------------------------------------------------
		output_filename_resolution_data = output_dir_path + "/lmd_res_data"
				+ filename_extension + ".root";
		// ------------------------------------------------------------------------

		double sigma = 4.0;
		// simple momentum dependent resolution parameterization (in mrad)
		// exp + const
		double theta_resolution_estimate = 0.001*(0.09 + 1.8 * exp(-0.7 * mom));
		double theta_plot_range_max = sigma * theta_resolution_estimate;
		// this is just guessed from the top formulas above for phi (in rad)
		double phi_resolution_estimate = 0.03 + 0.6 * exp(-0.7 * mom);
		double phi_plot_range_max = sigma * phi_resolution_estimate;

		// 2d overview plots
		if (false) {
			lmd_data_facade.primary_dimension_template.bins = 1000; //200 for 1d and 25 for 2d
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
					-theta_plot_range_max);
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
					theta_plot_range_max);
			lmd_data_facade.primary_dimension_template.dimension_options.track_param_type =
					LumiFit::IP;

			lmd_data_facade.secondary_dimension_template.bins = 1000;
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
					-TMath::Pi());
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
					TMath::Pi());
			lmd_data_facade.secondary_dimension_template.dimension_options.track_param_type =
					LumiFit::IP;

			lmd_data_facade.primary_dimension_template.dimension_options.track_type =
					LumiFit::DIFF_RECO_MC;
			lmd_data_facade.secondary_dimension_template.dimension_options.track_type =
					LumiFit::MC;
			lmd_data_facade.createHistogramData2D("dtheta_vs_phi_ang_res",
					num_events);

			lmd_data_facade.secondary_dimension_template.dimension_options.dimension_type =
					LumiFit::THETA;
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
					0.002);
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
					0.01);
			lmd_data_facade.createHistogramData2D("dtheta_vs_theta_ang_res",
					num_events);

			lmd_data_facade.primary_dimension_template.dimension_options.dimension_type =
					LumiFit::PHI;
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
					-phi_plot_range_max);
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
					phi_plot_range_max);
			lmd_data_facade.createHistogramData2D("dphi_vs_theta_ang_res",
					num_events);

			lmd_data_facade.secondary_dimension_template.dimension_options.dimension_type =
					LumiFit::PHI;
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
					-TMath::Pi());
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
					TMath::Pi());
			lmd_data_facade.createHistogramData2D("dphi_vs_phi_ang_res", num_events);
		}

		// now the resolution parameterization stuff
		if (true) { //2d
			lmd_data_facade.primary_dimension_template.bins = 25; //200 for 1d and 25 for 2d
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
					-theta_plot_range_max);
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
					theta_plot_range_max);
			lmd_data_facade.primary_dimension_template.dimension_options.track_param_type =
					LumiFit::IP;

			lmd_data_facade.secondary_dimension_template.bins = 25;
			/*lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
			 -phi_plot_range_max);
			 lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
			 phi_plot_range_max);*/
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
					-theta_plot_range_max);
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
					theta_plot_range_max);
			lmd_data_facade.secondary_dimension_template.dimension_options.track_param_type =
					LumiFit::IP;

			/*LumiFit::LmdDimension primary_selection_dimension_bundle_template;
			 primary_selection_dimension_bundle_template.dimension_options.dimension_type =
			 LumiFit::THETA;
			 primary_selection_dimension_bundle_template.dimension_options.track_param_type =
			 LumiFit::IP;
			 primary_selection_dimension_bundle_template.dimension_options.track_type =
			 LumiFit::MC;
			 primary_selection_dimension_bundle_template.dimension_range.setRangeLow(
			 0.0025); //rad
			 primary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
			 0.01);
			 primary_selection_dimension_bundle_template.bins = 30; // 100 for 1d and 30 for 2d
			 lmd_data_facade.selection_dimension_bundles.push_back(
			 primary_selection_dimension_bundle_template);

			 LumiFit::LmdDimension secondary_selection_dimension_bundle_template;
			 secondary_selection_dimension_bundle_template.dimension_options.dimension_type =
			 LumiFit::PHI;
			 secondary_selection_dimension_bundle_template.dimension_options.track_param_type =
			 LumiFit::IP;
			 secondary_selection_dimension_bundle_template.dimension_options.track_type =
			 LumiFit::MC;
			 secondary_selection_dimension_bundle_template.dimension_range.setRangeLow(
			 -TMath::Pi()); //rad
			 secondary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
			 TMath::Pi());
			 secondary_selection_dimension_bundle_template.bins = 30;
			 lmd_data_facade.selection_dimension_bundles.push_back(
			 secondary_selection_dimension_bundle_template);*/

			LumiFit::LmdDimension primary_selection_dimension_bundle_template;
			primary_selection_dimension_bundle_template.dimension_options.dimension_type =
					LumiFit::THETA_X;
			primary_selection_dimension_bundle_template.dimension_options.track_param_type =
					LumiFit::IP;
			primary_selection_dimension_bundle_template.dimension_options.track_type =
					LumiFit::MC;
			primary_selection_dimension_bundle_template.dimension_range.setRangeLow(
					-0.01); //rad
			primary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
					0.01);
			primary_selection_dimension_bundle_template.bins = 30; // 100 for 1d and 30 for 2d
			lmd_data_facade.selection_dimension_bundles.push_back(
					primary_selection_dimension_bundle_template);

			LumiFit::LmdDimension secondary_selection_dimension_bundle_template;
			secondary_selection_dimension_bundle_template.dimension_options.dimension_type =
					LumiFit::THETA_Y;
			secondary_selection_dimension_bundle_template.dimension_options.track_param_type =
					LumiFit::IP;
			secondary_selection_dimension_bundle_template.dimension_options.track_type =
					LumiFit::MC;
			secondary_selection_dimension_bundle_template.dimension_range.setRangeLow(
					-0.01); //rad
			secondary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
					0.01);
			secondary_selection_dimension_bundle_template.bins = 30;
			lmd_data_facade.selection_dimension_bundles.push_back(
					secondary_selection_dimension_bundle_template);

			lmd_data_facade.createSelectionDimensionCombinations();

			lmd_data_facade.create2DAngularResolutionDataBundle(num_events);
		}
		if (false) { // 1d
			lmd_data_facade.primary_dimension_template.dimension_options.dimension_type =
					LumiFit::THETA;
			lmd_data_facade.primary_dimension_template.bins = 200; //200 for 1d and 25 for 2d
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
					-theta_plot_range_max);
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
					theta_plot_range_max);
			lmd_data_facade.primary_dimension_template.dimension_options.track_param_type =
					LumiFit::IP;

			LumiFit::LmdDimension primary_selection_dimension_bundle_template;
			primary_selection_dimension_bundle_template.dimension_options.dimension_type =
					LumiFit::THETA;
			primary_selection_dimension_bundle_template.dimension_options.track_param_type =
					LumiFit::IP;
			primary_selection_dimension_bundle_template.dimension_options.track_type =
					LumiFit::MC;
			primary_selection_dimension_bundle_template.dimension_range.setRangeLow(
					0.0015); //rad
			primary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
					0.0125);
			primary_selection_dimension_bundle_template.bins = 110; // 100 for 1d and 30 for 2d
			lmd_data_facade.selection_dimension_bundles.push_back(
					primary_selection_dimension_bundle_template);

			lmd_data_facade.createSelectionDimensionCombinations();

			lmd_data_facade.create1DAngularResolutionDataBundle(num_events);
		}
	}
	if (data_types.find("v") != std::string::npos) {
		// ---- ip position data ---- //
		path sim_params(input_dir_path);

		// get simulation ip distribution properties
		LumiFit::LmdSimIPParameters true_ip_values =
				lmd_data_facade.readSimulationIPParameters(
						sim_params.branch_path().string());
		true_ip_values.print();
		lmd_data_facade.current_simulation_ip_parameters = true_ip_values;

		// ---- Output file -------------------------------------------------------
		output_filename_vertex_data = output_dir_path + "/lmd_vertex_data"
				+ filename_extension + ".root";
		// ------------------------------------------------------------------------

		lmd_data_facade.primary_dimension_template.bins = 300;
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
				-3.0);
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
				3.0);

		// create angular data object bundle and register in data reader
		lmd_data_facade.create1DVertexDataBundle(num_events);
	}

	if (is_phi_slicing_enabled) {
		// clear current bundles in case there are any
		lmd_data_facade.selection_dimension_bundles.clear();

		// set some dimension parameters
		lmd_data_facade.primary_dimension_template.dimension_options.dimension_type =
				LumiFit::THETA;
		lmd_data_facade.primary_dimension_template.bins = 100;
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(0.005);
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
				0.015);
		lmd_data_facade.primary_dimension_template.dimension_options.track_param_type =
				LumiFit::IP;

		lmd_data_facade.secondary_dimension_template.dimension_options.dimension_type =
				LumiFit::PHI;
		lmd_data_facade.secondary_dimension_template.bins = 50;
		lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
				-TMath::Pi());
		lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
				TMath::Pi());
		lmd_data_facade.secondary_dimension_template.dimension_options.track_param_type =
				LumiFit::IP;

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

		if (data_types.find("a") != std::string::npos) {
			// ---- dpm or elastic data ---- //

			// create angular data object bundle
			lmd_data_facade.create2DAngularDataBundle(num_events);
		}
		if (data_types.find("e") != std::string::npos) {
			// ---- acceptance or box gen data ---- //
			/*lmd_data_facade.primary_dimension_template.bins = 100;
			 lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
			 0.5 + 0.0725);
			 lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
			 15.0 + 0.0725);*/
			lmd_data_facade.primary_dimension_template.bins = 500;
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
					0.0);
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
					15.0);
			lmd_data_facade.secondary_dimension_template.bins = 200;
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
					-TMath::Pi());
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
					TMath::Pi());

			// create acceptance
			lmd_data_facade.createAcceptance2D("lmd_acceptance", num_events);

		}
		if (data_types.find("v") != std::string::npos) {
			// ---- ip position data ---- //

			lmd_data_facade.primary_dimension_template.bins = 300;
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
					-3.0);
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
					3.0);

			// create angular data object bundle and register in data reader
			lmd_data_facade.create1DVertexDataBundle(num_events);
		}
		if (data_types.find("r") != std::string::npos) {
			// ---- create lmd resolution objects from box gen data ---- //

			// ---- Output file -------------------------------------------------------
			output_filename_resolution_data = output_dir_path + "/lmd_res_data"
					+ filename_extension + ".root";
			// ------------------------------------------------------------------------

			lmd_data_facade.primary_dimension_template.dimension_options.dimension_type =
					LumiFit::THETA;
			lmd_data_facade.primary_dimension_template.bins = 125;
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
					-2.5);
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
					2.5);
			lmd_data_facade.primary_dimension_template.dimension_options.track_param_type =
					LumiFit::IP;

			lmd_data_facade.secondary_dimension_template.dimension_options.dimension_type =
					LumiFit::PHI;
			lmd_data_facade.secondary_dimension_template.bins = 20;
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
					-0.2);
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
					0.2);
			lmd_data_facade.secondary_dimension_template.dimension_options.track_param_type =
					LumiFit::IP;

			LumiFit::LmdDimension primary_selection_dimension_bundle_template;
			primary_selection_dimension_bundle_template.dimension_options.dimension_type =
					LumiFit::THETA;
			primary_selection_dimension_bundle_template.dimension_options.track_param_type =
					LumiFit::IP;
			primary_selection_dimension_bundle_template.dimension_options.track_type =
					LumiFit::MC;
			primary_selection_dimension_bundle_template.dimension_range.setRangeLow(
					0.001); //mrad
			primary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
					0.015);
			primary_selection_dimension_bundle_template.bins = 140;

			lmd_data_facade.selection_dimension_bundles.push_back(
					primary_selection_dimension_bundle_template);
			lmd_data_facade.createSelectionDimensionCombinations();

			lmd_data_facade.create1DAngularResolutionDataBundle(num_events);
		}
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
	std::vector<PndLmdHistogramData> my_lmd_hist_vec =
			lmd_data_facade.getLmdHistogramData();

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
	std::cout
			<< "-t [type of data to create] (a = angular, e = efficiency, r = resolution, v = vertex)"
			<< std::endl;
	std::cout << "-p [input directory path]" << std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-f [filelist path]" << std::endl;
	std::cout << "-o [output directory path]" << std::endl;
	std::cout << "-s slice data into phi slices according to modules"
			<< std::endl;
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
			false, is_filelist_path_set = false, is_output_data_path_set = false,
			is_phi_slicing_enabled = false;
	double momentum = -1.0;
	std::string data_type = "";
	unsigned int num_events = 0;
	double cross_section = 1.0;
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
			case 's':
				is_phi_slicing_enabled = true;
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
						|| optopt == 's' || optopt == 'c')
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
	if (!(checkDataType(data_type) && is_mom_set && is_data_path_set))
		skip_program = true;

	if (skip_program)
		displayInfo();
	else {
		if (!is_output_data_path_set)
			output_dir_path = data_path;
		createLumiFitData(data_path, filelist_path, output_dir_path, momentum,
				data_type, num_events, cross_section, is_phi_slicing_enabled);

		return 0;
	}
}
