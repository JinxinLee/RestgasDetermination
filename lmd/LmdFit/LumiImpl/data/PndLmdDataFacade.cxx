/*
 * PndLmdDataFacade.cxx
 *
 *  Created on: Aug 26, 2013
 *      Author: steve
 */

#include "PndLmdDataFacade.h"
#include "DataStructs.h"
#include "PndLmdLumiHelper.h"

#include <sstream>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

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

double PndLmdDataFacade::getLabMomentum() const {
	return lab_momentum;
}

void PndLmdDataFacade::setLabMomentum(double lab_momentum_) {
	lab_momentum = lab_momentum_;
	data_reader->setBeam(lab_momentum);
}

LumiFit::LmdDimension & PndLmdDataFacade::getPrimaryDimensionTemplate() {
	return primary_dimension_template;
}

LumiFit::LmdDimension & PndLmdDataFacade::getSecondaryDimensionTemplate() {
	return secondary_dimension_template;
}

LumiFit::LmdDimension & PndLmdDataFacade::getPrimarySelectionDimensionTemplate() {
	return primary_selection_dimension_template;
}

LumiFit::LmdDimension & PndLmdDataFacade::getSecondarySelectionDimensionTemplate() {
	return secondary_selection_dimension_template;
}

LumiFit::LmdDimension & PndLmdDataFacade::getPrimarySelectionDimensionBundleTemplate() {
	return primary_selection_dimension_bundle_template;
}

LumiFit::LmdDimension & PndLmdDataFacade::getSecondarySelectionDimensionBundleTemplate() {
	return secondary_selection_dimension_bundle_template;
}

double PndLmdDataFacade::getCurrentReferenceLuminosityPerEvent() const {
	return current_reference_luminosity_per_event;
}

void PndLmdDataFacade::setCurrentReferenceLuminosityPerEvent(
		double current_reference_luminosity_per_event_) {
	current_reference_luminosity_per_event =
			current_reference_luminosity_per_event_;
}

std::vector<PndLmdAcceptance> PndLmdDataFacade::getLmdAcceptances() const {
	return lmd_acceptances;
}

std::vector<PndLmdData> PndLmdDataFacade::getLmdDatas() const {
	return lmd_datas;
}

std::vector<PndLmdResolution> PndLmdDataFacade::getLmdResolutions() const {
	return lmd_resolutions;
}

void PndLmdDataFacade::addDataDirectory(TString directory) {
	// scan which data reader has to create this object
	data_reader->addDataPath(directory);
}

LumiFit::LmdDimension PndLmdDataFacade::constructPrimaryDimension() const {
	return primary_dimension_template.clone();
}

LumiFit::LmdDimension PndLmdDataFacade::constructSecondaryDimension() const {
	return secondary_dimension_template.clone();
}

void PndLmdDataFacade::initialize1DData(PndLmdAbstractData & data) const {
	data.setPrimaryDimension(constructPrimaryDimension());
}

void PndLmdDataFacade::initialize2DData(PndLmdAbstractData & data) const {
	data.setPrimaryDimension(constructPrimaryDimension());
	data.setSecondaryDimension(constructSecondaryDimension());
}

void PndLmdDataFacade::createAcceptance1D(unsigned int num_events) {
	PndLmdAcceptance acc;
	acc.setNumEvents(num_events);
	acc.setLabMomentum(lab_momentum);
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	primary_dimension_template.dimension_options.dimension_type = LumiFit::THETA;
	initialize1DData(acc);
	lmd_acceptances.push_back(acc);
}

void PndLmdDataFacade::createAcceptance2D(unsigned int num_events) {
	PndLmdAcceptance acc;
	acc.setNumEvents(num_events);
	acc.setLabMomentum(lab_momentum);
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	primary_dimension_template.dimension_options.dimension_type = LumiFit::THETA;
	secondary_dimension_template.dimension_options.track_type = LumiFit::MC;
	secondary_dimension_template.dimension_options.dimension_type = LumiFit::PHI;
	initialize2DData(acc);
	lmd_acceptances.push_back(acc);
}

void PndLmdDataFacade::createData1D(unsigned int num_events) {
	PndLmdData data;
	data.setNumEvents(num_events);
	data.setLabMomentum(lab_momentum);
	initialize1DData(data);
	data.setReferenceLuminosityPerEvent(current_reference_luminosity_per_event);
	lmd_datas.push_back(data);
}

void PndLmdDataFacade::createData2D(unsigned int num_events) {
	PndLmdData data;
	data.setNumEvents(num_events);
	initialize2DData(data);
	data.setReferenceLuminosityPerEvent(current_reference_luminosity_per_event);
	lmd_datas.push_back(data);
}

void PndLmdDataFacade::createResolution1D(unsigned int num_events) {
	PndLmdResolution res;
	res.setNumEvents(num_events);
	res.setLabMomentum(lab_momentum);
	res.setPrimarySelectionDimension(primary_selection_dimension_template);
	res.setSecondarySelectionDimension(secondary_selection_dimension_template);
	initialize1DData(res);
	lmd_resolutions.push_back(res);
}

void PndLmdDataFacade::createResolution2D(unsigned int num_events) {
	PndLmdResolution res;
	res.setNumEvents(num_events);
	res.setLabMomentum(lab_momentum);
	res.setPrimarySelectionDimension(primary_selection_dimension_template);
	res.setSecondarySelectionDimension(secondary_selection_dimension_template);
	initialize2DData(res);
	lmd_resolutions.push_back(res);
}

void PndLmdDataFacade::create1DVertexDataBundle(unsigned int num_events) {
	primary_dimension_template.dimension_options.dimension_type = LumiFit::X;
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	primary_dimension_template.dimension_options.track_param_type = LumiFit::IP;

	// we are actually using and filling cm here as it is the standard length unit
	//primary_dimension_template.unit_factor.unit_prefix = LumiFit::CENTI;

	createData1D(num_events);
	lmd_datas[0].setName("mc_x");

	primary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	createData1D(num_events);
	lmd_datas[1].setName("reco_x");

	primary_dimension_template.dimension_options.dimension_type = LumiFit::Y;
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	createData1D(num_events);
	lmd_datas[2].setName("mc_y");

	primary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	createData1D(num_events);
	lmd_datas[3].setName("reco_y");

	primary_dimension_template.dimension_options.dimension_type = LumiFit::Z;
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;
	createData1D(num_events);
	lmd_datas[4].setName("mc_z");

	primary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	createData1D(num_events);
	lmd_datas[5].setName("reco_z");
}

void PndLmdDataFacade::create1DAngularDataBundle(unsigned int num_events) {
	LumiFit::LmdDimensionRange temp_dimension_range =
			primary_dimension_template.dimension_range;

	PndLmdLumiHelper helper;
	primary_dimension_template.dimension_range.setRangeLow(
			helper.getMomentumTransferFromTheta(lab_momentum,
					primary_dimension_template.dimension_range.getRangeLow()));
	primary_dimension_template.dimension_range.setRangeHigh(
			helper.getMomentumTransferFromTheta(lab_momentum,
					primary_dimension_template.dimension_range.getRangeHigh()));
	primary_dimension_template.dimension_range.setUnitPrefix(LumiFit::NONE);

	primary_dimension_template.dimension_options.dimension_type = LumiFit::T;
	primary_dimension_template.dimension_options.track_type = LumiFit::MC;

	createData1D(num_events);
	lmd_datas[lmd_datas.size() - 1].setName("mc_t");

	primary_dimension_template.dimension_range = temp_dimension_range;
	primary_dimension_template.dimension_options.dimension_type = LumiFit::THETA;
	createData1D(num_events);
	lmd_datas[lmd_datas.size() - 1].setName("mc_th");

	primary_dimension_template.dimension_options.track_type = LumiFit::MC_ACC;
	createData1D(num_events);
	lmd_datas[lmd_datas.size() - 1].setName("mc_acc_th");

	primary_dimension_template.dimension_options.track_type = LumiFit::RECO;
	createData1D(num_events);
	lmd_datas[lmd_datas.size() - 1].setName("reco");
}

void PndLmdDataFacade::create1DAngularResolutionDataBundle(
		unsigned int num_events) {
	primary_selection_dimension_bundle_template.dimension_range.setUnitPrefix(
			LumiFit::MILLI);
	primary_selection_dimension_bundle_template.dimension_options.dimension_type =
			LumiFit::THETA;
	primary_selection_dimension_bundle_template.dimension_options.track_param_type =
			LumiFit::IP;
	secondary_selection_dimension_bundle_template.dimension_options.dimension_type =
			LumiFit::PHI;
	secondary_selection_dimension_bundle_template.dimension_options.track_param_type =
			LumiFit::IP;

	primary_selection_dimension_bundle_template.calculateBinSize();
	secondary_selection_dimension_bundle_template.calculateBinSize();

	primary_selection_dimension_template =
			primary_selection_dimension_bundle_template;
	primary_selection_dimension_template.dimension_range.setUnitPrefix(
			LumiFit::NONE);

	secondary_selection_dimension_template =
			secondary_selection_dimension_bundle_template;

	primary_selection_dimension_template.bins = 1;
	secondary_selection_dimension_template.bins = 1;

	for (unsigned int index_slice_theta = 0;
			index_slice_theta < primary_selection_dimension_bundle_template.bins;
			index_slice_theta++) {
		for (unsigned int index_slice_phi = 0;
				index_slice_phi < secondary_selection_dimension_bundle_template.bins;
				index_slice_phi++) {
			// create and register lmd resolution objects
			// 1d resolution is saved in the same object as well
			primary_selection_dimension_template.dimension_range.setRangeLow(
					primary_selection_dimension_bundle_template.dimension_range.getRangeLow()
							+ primary_selection_dimension_bundle_template.bin_size
									* index_slice_theta);
			primary_selection_dimension_template.dimension_range.setRangeHigh(
					primary_selection_dimension_bundle_template.dimension_range.getRangeLow()
							+ primary_selection_dimension_bundle_template.bin_size
									* (index_slice_theta + 1));
			secondary_selection_dimension_template.dimension_range.setRangeLow(
					secondary_selection_dimension_bundle_template.dimension_range.getRangeLow()
							+ secondary_selection_dimension_bundle_template.bin_size
									* index_slice_phi);
			secondary_selection_dimension_template.dimension_range.setRangeHigh(
					secondary_selection_dimension_bundle_template.dimension_range.getRangeLow()
							+ secondary_selection_dimension_bundle_template.bin_size
									* (index_slice_phi + 1));

			primary_selection_dimension_template.calculateBinSize();
			secondary_selection_dimension_template.calculateBinSize();
			createResolution1D(num_events);
			std::stringstream ss;
			ss << "ang_res_" << index_slice_theta << "_" << index_slice_phi;
			lmd_resolutions[lmd_resolutions.size() - 1].setName(ss.str());
		}
	}
}

void PndLmdDataFacade::fillAll() {
	data_reader->registerAcceptances(lmd_acceptances);
	data_reader->registerData(lmd_datas);
	data_reader->registerResolutions(lmd_resolutions);

	data_reader->read();
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

