/*
 * PndLmdDataFacade.cxx
 *
 *  Created on: Aug 26, 2013
 *      Author: steve
 */

#include "PndLmdDataFacade.h"
#include "PndLmdAngularData.h"
#include "PndLmdVertexData.h"
#include "PndLmdAcceptance.h"
#include "fit/data/DataStructs.h"
#include "PndLmdLumiHelper.h"

#include <sstream>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "boost/regex.hpp"

using boost::property_tree::ptree;

PndLmdDataFacade::PndLmdDataFacade() {
	// TODO Auto-generated constructor stub
}

PndLmdDataFacade::~PndLmdDataFacade() {
	// TODO Auto-generated destructor stub
}

void PndLmdDataFacade::setDataReader(PndLmdDataReader* data_reader_) {
	data_reader = data_reader_;
}

std::vector<PndLmdAcceptance> PndLmdDataFacade::getLmdAcceptances() const {
	return lmd_acceptances;
}

std::vector<PndLmdAngularData> PndLmdDataFacade::getLmdAngularData() const {
	return lmd_angular_data;
}

std::vector<PndLmdVertexData> PndLmdDataFacade::getLmdVertexData() const {
	return lmd_vertex_data;
}

std::vector<PndLmdHistogramData> PndLmdDataFacade::getLmdHistogramData() const {
	return lmd_hist_data;
}

void PndLmdDataFacade::addDataDirectory(TString directory) {
	// scan which data reader has to create this object
	data_reader->addFilePath(directory + "/Lumi_TrksQA*.root");
}

void PndLmdDataFacade::addFileList(std::string filelist) {
	// scan which data reader has to create this object
	std::ifstream input(filelist.c_str());
	std::string line;

	while (std::getline(input, line)) {
		std::cout << line << std::endl;
		data_reader->addFilePath(line);
	}
}

LumiFit::LmdDimension PndLmdDataFacade::constructPrimaryDimension() const {
	return primary_dimension_template.clone();
}

LumiFit::LmdDimension PndLmdDataFacade::constructSecondaryDimension() const {
	return secondary_dimension_template.clone();
}

void PndLmdDataFacade::clearSelectionDimensionMap() {
	selections_map.clear();
}

void PndLmdDataFacade::appendSelectionDimensions(
		const std::pair<std::string, std::vector<LumiFit::LmdDimension> > &selection_dimensions) {

	if (selections_map.size() == 0) {
		selections_map[selection_dimensions.first] = selection_dimensions.second;
	} else {
		std::map<std::string, std::vector<LumiFit::LmdDimension> > new_selections_map;

		std::map<std::string, std::vector<LumiFit::LmdDimension> >::iterator current_selection_map_it =
				selections_map.begin();
		while (current_selection_map_it != selections_map.end()) {
			std::stringstream ss;
			ss << current_selection_map_it->first << "_"
					<< selection_dimensions.first;
			new_selections_map[ss.str()] = current_selection_map_it->second;
			new_selections_map[ss.str()].insert(new_selections_map[ss.str()].end(),
					selection_dimensions.second.begin(),
					selection_dimensions.second.end());
			++current_selection_map_it;
		}

		selections_map = new_selections_map;
	}
}

void PndLmdDataFacade::createSelectionDimensionCombinations() {
	// create a map of selection dimension lists
	// one entry in the map for each combination
	selections_map.clear();
	std::map<std::string, std::vector<LumiFit::LmdDimension> > temp_selections_map;

	LumiFit::LmdDimension primary_selection_dimension;

	// for the first selection dimension bundle just fill the map with all different combinations
	for (unsigned int i = 0; i < selection_dimension_bundles.size(); i++) {
		selection_dimension_bundles[i].calculateBinSize();
		if (0 == i) {
			primary_selection_dimension = selection_dimension_bundles[i];

			primary_selection_dimension.bins = 1;

			for (unsigned int slice_index = 0;
					slice_index < selection_dimension_bundles[i].bins; slice_index++) {
				primary_selection_dimension.dimension_range.setRangeLow(
						selection_dimension_bundles[i].dimension_range.getRangeLow()
								+ selection_dimension_bundles[i].bin_size * slice_index);
				primary_selection_dimension.dimension_range.setRangeHigh(
						selection_dimension_bundles[i].dimension_range.getRangeLow()
								+ selection_dimension_bundles[i].bin_size * (slice_index + 1));
				primary_selection_dimension.calculateBinSize();
				std::stringstream ss;
				ss << "_" << slice_index;
				temp_selections_map[ss.str()].push_back(primary_selection_dimension);
			}
		} else {
			temp_selections_map = extendSelectionsMapByDimensionBundle(
					temp_selections_map, selection_dimension_bundles[i]);
		}
	}
	selections_map = temp_selections_map;
	std::cout << "successfully created selection dimension combination map!"
			<< std::endl;
}

std::map<std::string, std::vector<LumiFit::LmdDimension> > PndLmdDataFacade::extendSelectionsMapByDimensionBundle(
		std::map<std::string, std::vector<LumiFit::LmdDimension> > &current_selections_map,
		LumiFit::LmdDimension &selection_dimension_bundle) const {

	std::map<std::string, std::vector<LumiFit::LmdDimension> > new_selections_map;
// for the remaining selection dimensions
// (atm its just the secondary, but it could be more in principle):
// loop over them, and for each entry in the map

	LumiFit::LmdDimension selection_dimension = selection_dimension_bundle;

	selection_dimension.bins = 1;

	for (unsigned int slice_index = 0;
			slice_index < selection_dimension_bundle.bins; slice_index++) {
		selection_dimension.dimension_range.setRangeLow(
				selection_dimension_bundle.dimension_range.getRangeLow()
						+ selection_dimension_bundle.bin_size * slice_index);
		selection_dimension.dimension_range.setRangeHigh(
				selection_dimension_bundle.dimension_range.getRangeLow()
						+ selection_dimension_bundle.bin_size * (slice_index + 1));

		selection_dimension.calculateBinSize();
		// loop over current map and for each entry create a new entry with addition suffix
		std::map<std::string, std::vector<LumiFit::LmdDimension> >::iterator current_selection_map_it =
				current_selections_map.begin();
		while (current_selection_map_it != current_selections_map.end()) {
			std::stringstream ss;
			ss << current_selection_map_it->first << "_" << slice_index;
			new_selections_map[ss.str()] = current_selection_map_it->second;
			new_selections_map[ss.str()].push_back(selection_dimension);
			++current_selection_map_it;
		}
	}

	return new_selections_map;
}

void PndLmdDataFacade::initialize1DData(std::string name,
		PndLmdAbstractData & data) const {
	data.setName(name);
	data.setLabMomentum(lab_momentum);
	data.setPrimaryDimension(constructPrimaryDimension());
}

void PndLmdDataFacade::initialize2DData(std::string name,
		PndLmdAbstractData & data) const {
	data.setName(name);
	data.setLabMomentum(lab_momentum);
	data.setPrimaryDimension(constructPrimaryDimension());
	data.setSecondaryDimension(constructSecondaryDimension());
}

void PndLmdDataFacade::addMultipleInstancesBasedOnSelections(
		const PndLmdAngularData & data) {
	if (selections_map.size() > 0) {
		std::map<std::string, std::vector<LumiFit::LmdDimension> >::iterator selections_map_iter =
				selections_map.begin();
		while (selections_map_iter != selections_map.end()) {
			// clone the data object
			PndLmdAngularData data_clone(data);
			//rename
			data_clone.setName(data.getName() + selections_map_iter->first);
			for (unsigned int i = 0; i < selections_map_iter->second.size(); i++) {
				data_clone.addSelectionDimension(selections_map_iter->second[i]);
			}
			lmd_angular_data.push_back(data_clone);
			++selections_map_iter;
		}
	} else
		lmd_angular_data.push_back(data);
}

void PndLmdDataFacade::addMultipleInstancesBasedOnSelections(
		const PndLmdAcceptance & data) {
	if (selections_map.size() > 0) {
		std::cout << "adding " << selections_map.size()
				<< " different acceptance instances with different selections!"
				<< std::endl;
		std::map<std::string, std::vector<LumiFit::LmdDimension> >::iterator selections_map_iter =
				selections_map.begin();
		while (selections_map_iter != selections_map.end()) {
			// clone the data object
			PndLmdAcceptance data_clone(data);
			data_clone.cloneData(data);
			//rename
			data_clone.setName(data.getName() + selections_map_iter->first);
			for (unsigned int i = 0; i < selections_map_iter->second.size(); i++) {
				data_clone.addSelectionDimension(selections_map_iter->second[i]);
			}
			lmd_acceptances.push_back(data_clone);
			++selections_map_iter;
		}
	} else
		lmd_acceptances.push_back(data);
}

void PndLmdDataFacade::addMultipleInstancesBasedOnSelections(
		const PndLmdHistogramData & data) {
	if (selections_map.size() > 0) {
		std::cout << "adding " << selections_map.size()
				<< " different histogram data instances with different selections!"
				<< std::endl;
		std::map<std::string, std::vector<LumiFit::LmdDimension> >::iterator selections_map_iter =
				selections_map.begin();
		while (selections_map_iter != selections_map.end()) {
			// clone the data object
			PndLmdHistogramData data_clone(data);
			//rename
			data_clone.setName(data.getName() + selections_map_iter->first);
			for (unsigned int i = 0; i < selections_map_iter->second.size(); i++) {
				data_clone.addSelectionDimension(selections_map_iter->second[i]);
			}
			lmd_hist_data.push_back(data_clone);
			++selections_map_iter;
		}
	} else
		lmd_hist_data.push_back(data);
}

void PndLmdDataFacade::addMultipleInstancesBasedOnSelections(
		const PndLmdVertexData & data) {
	if (selections_map.size() > 0) {
		std::map<std::string, std::vector<LumiFit::LmdDimension> >::iterator selections_map_iter =
				selections_map.begin();
		while (selections_map_iter != selections_map.end()) {
			// clone the data object
			PndLmdVertexData data_clone(data);
			//rename
			data_clone.setName(data.getName() + selections_map_iter->first);
			for (unsigned int i = 0; i < selections_map_iter->second.size(); i++) {
				data_clone.addSelectionDimension(selections_map_iter->second[i]);
			}
			lmd_vertex_data.push_back(data_clone);
			++selections_map_iter;
		}
	} else
		lmd_vertex_data.push_back(data);
}

void PndLmdDataFacade::createAcceptance1D(std::string name,
		unsigned int num_events) {
	PndLmdAcceptance acc;
	acc.setNumEvents(num_events);
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	initialize1DData(name, acc);

	addMultipleInstancesBasedOnSelections(acc);
}

void PndLmdDataFacade::createAcceptance2D(std::string name,
		unsigned int num_events) {
	PndLmdAcceptance acc;
	acc.setNumEvents(num_events);
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	secondary_dimension_template.dimension_options.track_type = LumiFit::MC;
	initialize2DData(name, acc);

	addMultipleInstancesBasedOnSelections(acc);
}

void PndLmdDataFacade::createData1D(std::string name, unsigned int num_events) {
	PndLmdAngularData data;
	data.setNumEvents(num_events);
	initialize1DData(name, data);
	data.setReferenceLuminosityPerEvent(current_reference_luminosity_per_event);

	addMultipleInstancesBasedOnSelections(data);
}

void PndLmdDataFacade::createData2D(std::string name, unsigned int num_events) {
	PndLmdAngularData data;
	data.setNumEvents(num_events);
	initialize2DData(name, data);
	data.setReferenceLuminosityPerEvent(current_reference_luminosity_per_event);

	addMultipleInstancesBasedOnSelections(data);
}

void PndLmdDataFacade::createVertexData1D(std::string name,
		unsigned int num_events) {
	PndLmdVertexData data;
	data.setNumEvents(num_events);
	initialize1DData(name, data);
	data.setSimulationIPParameters(current_simulation_ip_parameters);

	addMultipleInstancesBasedOnSelections(data);
}

void PndLmdDataFacade::createVertexData2D(std::string name,
		unsigned int num_events) {
	PndLmdVertexData data;
	data.setNumEvents(num_events);
	initialize2DData(name, data);
	data.setSimulationIPParameters(current_simulation_ip_parameters);

	addMultipleInstancesBasedOnSelections(data);
}

void PndLmdDataFacade::createHistogramData1D(std::string name,
		unsigned int num_events) {
	PndLmdHistogramData res;
	res.setNumEvents(num_events);
	initialize1DData(name, res);

	addMultipleInstancesBasedOnSelections(res);
}

void PndLmdDataFacade::createHistogramData2D(std::string name,
		unsigned int num_events) {
	PndLmdHistogramData res;
	res.setNumEvents(num_events);
	initialize2DData(name, res);

	addMultipleInstancesBasedOnSelections(res);
}

void PndLmdDataFacade::create1DVertexDataBundle(unsigned int num_events) {
	primary_dimension_template.dimension_options.dimension_type = LumiFit::X;
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	primary_dimension_template.dimension_options.track_param_type = LumiFit::IP;

// we are actually using and filling cm here as it is the standard length unit
//primary_dimension_template.unit_factor.unit_prefix = LumiFit::CENTI;

	createVertexData1D("mc_x", num_events);

	primary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	createVertexData1D("reco_x", num_events);

	primary_dimension_template.dimension_options.dimension_type = LumiFit::Y;
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	createVertexData1D("mc_y", num_events);

	primary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	createVertexData1D("reco_y", num_events);

	primary_dimension_template.dimension_options.dimension_type = LumiFit::Z;
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	createVertexData1D("mc_z", num_events);

	primary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	createVertexData1D("reco_z", num_events);
}

void PndLmdDataFacade::create1DAngularDataBundle(unsigned int num_events) {
	LumiFit::LmdDimensionRange theta_dimension_range =
			primary_dimension_template.dimension_range;

	PndLmdLumiHelper helper;
	LumiFit::LmdDimensionRange t_dimension_range;
	t_dimension_range.setRangeLow(
			helper.getMomentumTransferFromTheta(lab_momentum,
					primary_dimension_template.dimension_range.getRangeLow()));
	t_dimension_range.setRangeHigh(
			helper.getMomentumTransferFromTheta(lab_momentum,
					primary_dimension_template.dimension_range.getRangeHigh()));

	primary_dimension_template.dimension_options.dimension_type = LumiFit::T;
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	primary_dimension_template.dimension_range = t_dimension_range;
	createData1D("mc_t", num_events);

	primary_dimension_template.dimension_range = theta_dimension_range;
	primary_dimension_template.dimension_options.dimension_type = LumiFit::THETA;
	createData1D("mc_th", num_events);

	primary_dimension_template.dimension_options.track_type = LumiFit::MC_ACC;
	createData1D("mc_acc", num_events);

	primary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	createData1D("reco", num_events);
}

void PndLmdDataFacade::create2DAngularDataBundle(unsigned int num_events) {
	primary_dimension_template.is_active = true;
	secondary_dimension_template.is_active = true;

	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	secondary_dimension_template.dimension_options.track_type = LumiFit::MC;
	createData2D("mc_th", num_events);

	primary_dimension_template.dimension_options.track_type = LumiFit::MC_ACC;
	secondary_dimension_template.dimension_options.track_type = LumiFit::MC_ACC;
	createData2D("mc_acc", num_events);

	primary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	secondary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	createData2D("reco", num_events);

	if (selections_map.size() > 0) {
		selections_map.clear();

		createData2D("reco", num_events);
	}
}

void PndLmdDataFacade::create1DAngularResolutionDataBundle(
		unsigned int num_events) {
	primary_dimension_template.dimension_options.track_type =
			LumiFit::DIFF_RECO_MC;
	secondary_dimension_template.dimension_options.track_type =
			LumiFit::DIFF_RECO_MC;

	createHistogramData1D("ang_res", num_events);
}

void PndLmdDataFacade::create2DAngularResolutionDataBundle(
		unsigned int num_events) {
	primary_dimension_template.dimension_options.track_type =
			LumiFit::DIFF_RECO_MC;
	secondary_dimension_template.dimension_options.track_type =
			LumiFit::DIFF_RECO_MC;

	createHistogramData2D("ang_res", num_events);
}

void PndLmdDataFacade::fillAll() {
	data_reader->registerAcceptances(lmd_acceptances);
	data_reader->registerData(lmd_angular_data);
	data_reader->registerData(lmd_vertex_data);
	data_reader->registerData(lmd_hist_data);

	data_reader->read();
}

std::vector<std::string> PndLmdDataFacade::findFilesByName(
		const boost::filesystem::path &top_dir_path_to_search,
		const std::string dir_name_filter, const std::string file_name) {

	const boost::regex my_filter(dir_name_filter,
			boost::regex::extended | boost::regex::icase);
	const boost::regex my_filename_filter(file_name,
			boost::regex::extended | boost::regex::icase);

	std::vector<std::string> all_matching_files;

	if (exists(top_dir_path_to_search)) {
		// try to find directory pattern
		bool dirname_ok(false);
		boost::smatch what;
		std::cout << "trying find " << dir_name_filter << " within "
				<< top_dir_path_to_search.string() << std::endl;
		if (boost::regex_search(top_dir_path_to_search.string(), what, my_filter)) {
			dirname_ok = true;
			std::cout << "This directory matches the filter " << dir_name_filter
					<< std::endl;
		}

		std::cout << top_dir_path_to_search.string()
				<< " exists... Looping over this directory.." << std::endl;
		boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
		for (boost::filesystem::directory_iterator itr(top_dir_path_to_search);
				itr != end_itr; ++itr) {
			// if the current entry is a file, just test for the filename
			if (boost::filesystem::is_regular_file(itr->status())) {
				if (dirname_ok) {
					// File matches, check if fit_result.root file resides in this directory
					boost::smatch fwhat;

					// Skip if no match
					if (!boost::regex_search(itr->path().string(), fwhat,
							my_filename_filter))
						continue;

					all_matching_files.push_back(itr->path().string());
				}
			}

			else if (boost::filesystem::is_directory(itr->status())) {
				std::vector<std::string> found_files = findFilesByName(itr->path(),
						dir_name_filter, file_name);
				all_matching_files.insert(all_matching_files.end(), found_files.begin(),
						found_files.end());
			}
		}
	}
	std::cout << "Found a total of " << all_matching_files.size()
			<< " matching files!" << std::endl;
	return all_matching_files;
}

LumiFit::LmdSimIPParameters PndLmdDataFacade::readSimulationIPParameters(
		std::string dir_path) {
	LumiFit::LmdSimIPParameters bp;
// Create an empty property tree object
	ptree pt;

// read the config file
	read_ini(dir_path + "/sim_beam_prop.config", pt);

	bp.offset_x_mean = pt.get<double>("ip_offset_x");
	bp.offset_y_mean = pt.get<double>("ip_offset_y");
	bp.offset_z_mean = pt.get<double>("ip_offset_z");
	bp.offset_x_width = pt.get<double>("ip_spread_x");
	bp.offset_y_width = pt.get<double>("ip_spread_y");
	bp.offset_z_width = pt.get<double>("ip_spread_z");
	bp.tilt_x_mean = pt.get<double>("beam_gradient_x");
	bp.tilt_y_mean = pt.get<double>("beam_gradient_y");
	bp.tilt_x_width = pt.get<double>("beam_emittance_x");
	bp.tilt_y_width = pt.get<double>("beam_emittance_y");

	return bp;
}

std::map<LumiFit::LmdSimIPParameters,
		std::map<LumiFit::LmdSimIPParameters,
				std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > > PndLmdDataFacade::clusterVertexData(
		std::vector<PndLmdVertexData> &lmd_vertex_vec) {
	std::map<LumiFit::LmdSimIPParameters,
			std::map<LumiFit::LmdSimIPParameters,
					std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > > return_map;

	for (unsigned int i = 0; i < lmd_vertex_vec.size(); i++) {
		LumiFit::LmdSimIPParameters sim_ip_params =
				lmd_vertex_vec[i].getSimulationIPParameters();

		sim_ip_params.offset_x_mean = 0.0; // set the insensitive value to 0.0
		LumiFit::LmdSimIPParameters dependency_ip_params;
		dependency_ip_params.offset_x_mean = 1.0; // set the dependency value to 1.0
		return_map[dependency_ip_params][sim_ip_params][lmd_vertex_vec[i].getPrimaryDimension().dimension_options.dimension_type].push_back(
				lmd_vertex_vec[i]);

		sim_ip_params = lmd_vertex_vec[i].getSimulationIPParameters();
		sim_ip_params.offset_y_mean = 0.0; // set the insensitive value to 0.0
		dependency_ip_params.reset();
		dependency_ip_params.offset_y_mean = 1.0; // set the dependency value to 1.0
		return_map[dependency_ip_params][sim_ip_params][lmd_vertex_vec[i].getPrimaryDimension().dimension_options.dimension_type].push_back(
				lmd_vertex_vec[i]);

		sim_ip_params = lmd_vertex_vec[i].getSimulationIPParameters();
		sim_ip_params.offset_z_mean = 0.0; // set the insensitive value to 0.0
		dependency_ip_params.reset();
		dependency_ip_params.offset_z_mean = 1.0; // set the dependency value to 1.0
		return_map[dependency_ip_params][sim_ip_params][lmd_vertex_vec[i].getPrimaryDimension().dimension_options.dimension_type].push_back(
				lmd_vertex_vec[i]);

		sim_ip_params = lmd_vertex_vec[i].getSimulationIPParameters();
		sim_ip_params.offset_x_width = 0.0; // set the insensitive value to 0.0
		dependency_ip_params.reset();
		dependency_ip_params.offset_x_width = 1.0; // set the dependency value to 1.0
		return_map[dependency_ip_params][sim_ip_params][lmd_vertex_vec[i].getPrimaryDimension().dimension_options.dimension_type].push_back(
				lmd_vertex_vec[i]);

		sim_ip_params = lmd_vertex_vec[i].getSimulationIPParameters();
		sim_ip_params.offset_y_width = 0.0; // set the insensitive value to 0.0
		dependency_ip_params.reset();
		dependency_ip_params.offset_y_width = 1.0; // set the dependency value to 1.0
		return_map[dependency_ip_params][sim_ip_params][lmd_vertex_vec[i].getPrimaryDimension().dimension_options.dimension_type].push_back(
				lmd_vertex_vec[i]);

		sim_ip_params = lmd_vertex_vec[i].getSimulationIPParameters();
		sim_ip_params.offset_z_width = 0.0; // set the insensitive value to 0.0
		dependency_ip_params.reset();
		dependency_ip_params.offset_z_width = 1.0; // set the dependency value to 1.0
		return_map[dependency_ip_params][sim_ip_params][lmd_vertex_vec[i].getPrimaryDimension().dimension_options.dimension_type].push_back(
				lmd_vertex_vec[i]);
	}

	return return_map;
}
