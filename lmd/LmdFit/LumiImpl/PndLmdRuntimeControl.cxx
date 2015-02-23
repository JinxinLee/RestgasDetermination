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

PndLmdRuntimeControl::PndLmdRuntimeControl() :
		num_events(0) {
}

PndLmdRuntimeControl::~PndLmdRuntimeControl() {
}

void PndLmdRuntimeControl::applySelections() {
	lmd_data_facade.appendSelectionDimensions(
			lmd_runtime_config.selection_dimensions);
}

void PndLmdRuntimeControl::applyAutomaticResolutionDimensionRanges() {
	double sigma = 4.0;
	// simple momentum dependent resolution parameterization (in mrad)
	// exp + const
	double theta_resolution_estimate = 0.001
			* (0.09 + 1.8 * exp(-0.7 * momentum));
	double theta_plot_range_max = sigma * theta_resolution_estimate;
	// this is just guessed from the top formulas above for phi (in rad)
	double phi_resolution_estimate = 0.03 + 0.6 * exp(-0.7 * momentum);
	double phi_plot_range_max = sigma * phi_resolution_estimate;

	if (lmd_runtime_config.resolution_primary_dimension.is_active) {
		lmd_data_facade.primary_dimension_template.bins =
				lmd_runtime_config.resolution_primary_dimension.bins;
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
				lmd_runtime_config.resolution_primary_dimension.dimension_range.getRangeLow());
		lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
				lmd_runtime_config.resolution_primary_dimension.dimension_range.getRangeHigh());
	} else {
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeLow(
					-theta_plot_range_max);
			lmd_data_facade.primary_dimension_template.dimension_range.setRangeHigh(
					theta_plot_range_max);
	}

	if (lmd_runtime_config.resolution_secondary_dimension.is_active) {
		lmd_data_facade.secondary_dimension_template.bins =
				lmd_runtime_config.resolution_secondary_dimension.bins;
		lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
				lmd_runtime_config.resolution_secondary_dimension.dimension_range.getRangeLow());
		lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
				lmd_runtime_config.resolution_secondary_dimension.dimension_range.getRangeHigh());
	} else {
		if (lmd_runtime_config.secondary_dimension.dimension_options.dimension_type
				== LumiFit::PHI) {
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
					-phi_plot_range_max);
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
					phi_plot_range_max);
		} else {
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeLow(
					-theta_plot_range_max);
			lmd_data_facade.secondary_dimension_template.dimension_range.setRangeHigh(
					theta_plot_range_max);
		}
	}
}

void PndLmdRuntimeControl::createAngularData() {
	lmd_data_facade.primary_dimension_template =
			lmd_runtime_config.primary_dimension;
	lmd_data_facade.secondary_dimension_template =
			lmd_runtime_config.secondary_dimension;

	applySelections();

	// create angular data object bundle
	lmd_data_facade.create2DAngularDataBundle(num_events);
}

void PndLmdRuntimeControl::createEfficiencies() {
	lmd_data_facade.primary_dimension_template =
			lmd_runtime_config.primary_dimension;
	lmd_data_facade.secondary_dimension_template =
			lmd_runtime_config.secondary_dimension;

	lmd_data_facade.primary_dimension_template.bins =
			lmd_runtime_config.efficiency_primary_dimension.bins;
	lmd_data_facade.secondary_dimension_template.bins =
			lmd_runtime_config.efficiency_secondary_dimension.bins;

	applySelections();

	lmd_data_facade.createAcceptance2D("lmd_acceptance", num_events);
}

void PndLmdRuntimeControl::createResolutions() {
	lmd_data_facade.primary_dimension_template =
			lmd_runtime_config.primary_dimension;
	lmd_data_facade.secondary_dimension_template =
			lmd_runtime_config.secondary_dimension;

	applyAutomaticResolutionDimensionRanges();

	LumiFit::LmdDimension primary_selection_dimension_bundle_template =
			lmd_runtime_config.primary_dimension;
	primary_selection_dimension_bundle_template.dimension_options.track_type =
			LumiFit::MC;
	primary_selection_dimension_bundle_template.bins =
			lmd_runtime_config.resolution_primary_selection_dimension.bins;
	primary_selection_dimension_bundle_template.dimension_range.setRangeLow(
			lmd_runtime_config.resolution_primary_selection_dimension.dimension_range.getRangeLow());
	primary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
			lmd_runtime_config.resolution_primary_selection_dimension.dimension_range.getRangeHigh());

	lmd_data_facade.selection_dimension_bundles.push_back(
			primary_selection_dimension_bundle_template);

	if (lmd_runtime_config.secondary_dimension.is_active) {
		LumiFit::LmdDimension secondary_selection_dimension_bundle_template =
				lmd_runtime_config.secondary_dimension;
		secondary_selection_dimension_bundle_template.dimension_options.track_type =
				LumiFit::MC;
		secondary_selection_dimension_bundle_template.bins =
				lmd_runtime_config.resolution_secondary_selection_dimension.bins;
		secondary_selection_dimension_bundle_template.dimension_range.setRangeLow(
				lmd_runtime_config.resolution_secondary_selection_dimension.dimension_range.getRangeLow());
		secondary_selection_dimension_bundle_template.dimension_range.setRangeHigh(
				lmd_runtime_config.resolution_secondary_selection_dimension.dimension_range.getRangeHigh());

		lmd_data_facade.selection_dimension_bundles.push_back(
				secondary_selection_dimension_bundle_template);
	}

	lmd_data_facade.createSelectionDimensionCombinations();

	applySelections();

	lmd_data_facade.create2DAngularResolutionDataBundle(num_events);
}

void PndLmdRuntimeControl::createVertexData() {
	lmd_data_facade.primary_dimension_template =
			lmd_runtime_config.primary_dimension;
	lmd_data_facade.secondary_dimension_template =
			lmd_runtime_config.secondary_dimension;

	applySelections();

	// create angular data object bundle and register in data reader
	lmd_data_facade.create1DVertexDataBundle(num_events);
}

void PndLmdRuntimeControl::initFitRuntimeConfiguration(
		const std::string &config_url) {
	lmd_runtime_config.initFitFacadeFromConfigFile(lmd_fit_facade, config_url);
}

void PndLmdRuntimeControl::initDataRuntimeConfiguration(
		const std::string &config_url) {
	lmd_runtime_config.initDataFacadeFromConfigFile(lmd_data_facade, config_url);
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

void PndLmdRuntimeControl::setDataPaths(const std::string &input_dir_path,
		const std::string &filelist_path, const std::string &output_dir_path) {
	lmd_runtime_config.raw_data_directory = input_dir_path;
	lmd_runtime_config.raw_data_filelist_path = filelist_path;
	lmd_runtime_config.data_output_directory = output_dir_path;
}

void PndLmdRuntimeControl::createData(const std::string &data_types) {
	// create data reader instance
	//PndLmdSeperateDataReader data_reader;
	PndLmdCombinedDataReader data_reader;

	// set data reader to be used
	lmd_data_facade.setDataReader(&data_reader);

	// set lab momentum
	lmd_data_facade.lab_momentum = momentum;

	lmd_data_facade.current_reference_luminosity_per_event = 1.0
			/ total_elastic_cross_section;

	std::string filename_extension("");

	// add input directory to data facade
	if (lmd_runtime_config.raw_data_filelist_path.compare("") == 0)
		lmd_data_facade.addDataDirectory(lmd_runtime_config.raw_data_directory);
	else {
		lmd_data_facade.addFileList(lmd_runtime_config.raw_data_filelist_path);
		const boost::regex my_filename_filter("filelist_(\\d*).txt",
				boost::regex::extended | boost::regex::icase);

		boost::smatch dwhat;
		if (boost::regex_search(
				path(lmd_runtime_config.raw_data_filelist_path).filename().string(),
				dwhat, my_filename_filter)) {
			filename_extension = "_";
			filename_extension += std::string(dwhat[1]);
		}
	}

	std::string output_filename_angular_data(
			lmd_runtime_config.elastic_data_name);
	std::string output_filename_acceptance_data(lmd_runtime_config.acc_data_name);
	std::string output_filename_resolution_data(lmd_runtime_config.res_data_name);
	std::string output_filename_vertex_data(lmd_runtime_config.vertex_data_name);

	if (filename_extension != "") {
		boost::filesystem::path p(output_filename_angular_data);
		output_filename_angular_data = p.stem().string() + filename_extension
				+ p.extension().string();
		p = output_filename_acceptance_data;
		output_filename_acceptance_data = p.stem().string() + filename_extension
				+ p.extension().string();
		p = output_filename_resolution_data;
		output_filename_resolution_data = p.stem().string() + filename_extension
				+ p.extension().string();
		p = output_filename_vertex_data;
		output_filename_vertex_data = p.stem().string() + filename_extension
				+ p.extension().string();
	}

	if (data_types.find("a") != std::string::npos) {
		// ---- dpm or elastic data ---- //

		// ---- Output file -------------------------------------------------------
		output_filename_angular_data = lmd_runtime_config.data_output_directory
				+ "/" + output_filename_angular_data;
		// ------------------------------------------------------------------------
		createAngularData();
	}
	if (data_types.find("e") != std::string::npos) {
		// ---- acceptance or box gen data ---- //

		// ---- Output file -------------------------------------------------------
		output_filename_acceptance_data = lmd_runtime_config.data_output_directory
				+ "/" + output_filename_acceptance_data;
		// ------------------------------------------------------------------------
		createEfficiencies();
	}
	if (data_types.find("r") != std::string::npos) {
		// ---- create lmd resolution objects from box gen data ---- //

		// ---- Output file -------------------------------------------------------
		output_filename_resolution_data = lmd_runtime_config.data_output_directory
				+ "/" + output_filename_resolution_data;
		// ------------------------------------------------------------------------
		createResolutions();
	}
	if (data_types.find("v") != std::string::npos) {
		// ---- ip position data ---- //
		path sim_params(lmd_runtime_config.raw_data_directory);

		// get simulation ip distribution properties
		LumiFit::LmdSimIPParameters true_ip_values =
				lmd_data_facade.readSimulationIPParameters(
						sim_params.branch_path().string());
		true_ip_values.print();
		lmd_data_facade.current_simulation_ip_parameters = true_ip_values;

		// ---- Output file -------------------------------------------------------
		output_filename_vertex_data = lmd_runtime_config.data_output_directory + "/"
				+ output_filename_vertex_data;
		// ------------------------------------------------------------------------
		createVertexData();
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
		TFile f(output_filename_angular_data.c_str(), "RECREATE");
		for (unsigned int i = 0; i < my_lmd_data_vec.size(); i++) {
			my_lmd_data_vec[i].saveToRootFile();
		}
		f.Close();
	}
	if (my_lmd_vertex_data_vec.size() > 0) {
		TFile f(output_filename_vertex_data.c_str(), "RECREATE");
		for (unsigned int i = 0; i < my_lmd_vertex_data_vec.size(); i++) {
			my_lmd_vertex_data_vec[i].saveToRootFile();
		}
		f.Close();
	}
	if (my_lmd_acc_vec.size() > 0) {
		TFile f(output_filename_acceptance_data.c_str(), "RECREATE");
		for (unsigned int i = 0; i < my_lmd_acc_vec.size(); i++) {
			my_lmd_acc_vec[i].saveToRootFile();
		}
		f.Close();
	}
	if (my_lmd_hist_vec.size() > 0) {
		TFile f(output_filename_resolution_data.c_str(), "RECREATE");
		for (unsigned int i = 0; i < my_lmd_hist_vec.size(); i++) {
			my_lmd_hist_vec[i].saveToRootFile();
		}
		f.Close();
	}

	lmd_data_facade.clearSelectionDimensionMap();
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

