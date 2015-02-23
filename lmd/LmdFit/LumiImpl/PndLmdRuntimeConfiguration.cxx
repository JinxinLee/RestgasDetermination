#include "PndLmdRuntimeConfiguration.h"

#include "data/PndLmdDataFacade.h"
#include "fit/PndLmdFitFacade.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <iostream>

using boost::property_tree::ptree;

PndLmdRuntimeConfiguration::PndLmdRuntimeConfiguration() :
		elastic_data_name("lmd_data.root"), acc_data_name("lmd_acc_data.root"), res_data_name(
				"lmd_res_data.root"), res_param_data_name("resolution_params_1.root"), raw_data_directory(
				""), fitted_elastic_data_name("lmd_fitted_data.root"), vertex_data_name(
				"lmd_vertex_data.root"), data_output_directory("") {
	selection_dimensions.first = "";
	selection_dimensions.second = std::vector<LumiFit::LmdDimension>();
}

PndLmdRuntimeConfiguration::~PndLmdRuntimeConfiguration() {
}

void PndLmdRuntimeConfiguration::initFitFacadeFromConfigFile(
		PndLmdFitFacade &lmd_fit_facade, const std::string &file_url) {
	// Create an empty property tree object
	boost::property_tree::ptree pt;
	// read the config file
	read_json(file_url, pt);

	// estimator options

	EstimatorOptions est_opt;
	LumiFit::PndLmdFitModelOptions fit_model_options;

	lmd_fit_facade.setEstimatorType(
			LumiFit::StringToLmdEstimatorType.at(
					pt.get<std::string>("fit.estimator_type")));

	est_opt.setWithIntegralScaling(
			pt.get<bool>("fit.estimator_options.with_integral_scaling"));

	DataStructs::DimensionRange dim_range;
	dim_range.is_active = pt.get<bool>(
			"fit.estimator_options.fit_range_x_active");
	dim_range.range_low = pt.get<double>("fit.estimator_options.fit_range_x_low");
	dim_range.range_high = pt.get<double>(
			"fit.estimator_options.fit_range_x_high");
	est_opt.setFitRangeX(dim_range);

	dim_range.is_active = pt.get<bool>(
			"fit.estimator_options.fit_range_y_active");
	dim_range.range_low = pt.get<double>("fit.estimator_options.fit_range_y_low");
	dim_range.range_high = pt.get<double>(
			"fit.estimator_options.fit_range_y_high");
	est_opt.setFitRangeY(dim_range);

	lmd_fit_facade.setEstimatorOptions(est_opt);

	// fit options
	fit_model_options.fit_dimension = pt.get<int>(
			"fit.fit_model_options.fit_dimension");

	fit_model_options.dpm_elastic_parts = LumiFit::StringToDPMElasticParts.at(
			pt.get<std::string>("fit.fit_model_options.dpm_elastic_parts"));
	fit_model_options.momentum_transfer_active = pt.get<bool>(
			"fit.fit_model_options.momentum_transfer_active");

	fit_model_options.use_theta_xy_coordinate_system = pt.get<bool>(
			"fit.fit_model_options.use_theta_xy_coordinate_system");

	fit_model_options.divergence_smearing_active = pt.get<bool>(
			"fit.fit_model_options.divergence_smearing_active");
	fit_model_options.elastic_reco_data_file_url = pt.get<std::string>(
			"fit.fit_model_options.elastic_data_input_directory");
	if (fit_model_options.elastic_reco_data_file_url == "") {
		fit_model_options.elastic_reco_data_file_url = elastic_data_input_directory;
	}
	fit_model_options.elastic_reco_data_file_url += "/lmd_data.root";

	fit_model_options.fix_beam_tilts = pt.get<bool>(
			"fit.fit_model_options.fix_beam_tilts");
	fit_model_options.beam_tilt_x = pt.get<double>(
			"fit.fit_model_options.beam_tilt_x");
	fit_model_options.beam_tilt_y = pt.get<double>(
			"fit.fit_model_options.beam_tilt_y");

	fit_model_options.acceptance_correction_active = pt.get<bool>(
			"fit.fit_model_options.acceptance_correction_active");
	fit_model_options.acceptance_interpolation =
			LumiFit::StringToInterpolationType.at(
					pt.get<std::string>(
							"fit.fit_model_options.acceptance_interpolation"));
	fit_model_options.automatic_acceptance_boundary_finding_active = pt.get<bool>(
			"fit.fit_model_options.automatic_acceptance_boundary_finding_active");
	fit_model_options.acceptance_bound_low = pt.get<double>(
			"fit.fit_model_options.acceptance_bound_low");
	fit_model_options.acceptance_bound_high = pt.get<double>(
			"fit.fit_model_options.acceptance_bound_high");

	fit_model_options.resolution_smearing_active = pt.get<bool>(
			"fit.fit_model_options.resolution_smearing_active");

	fit_model_options.acceptance_resolution_input_directory =
			acceptance_resolution_input_directory;

	if (pt.get<std::string>(
			"fit.fit_model_options.resolution_parametrization_file_name") == "") {
		if (fit_model_options.fit_dimension == 1)
			fit_model_options.resolution_parametrization_file_url =
					res_param_data_name;
		else
			fit_model_options.resolution_parametrization_file_url = res_data_name;
	} else {
		fit_model_options.resolution_parametrization_file_url = pt.get<std::string>(
				"fit.fit_model_options.resolution_parametrization_file_name");
	}
	fit_model_options.resolution_parametrization_file_url =
			acceptance_resolution_input_directory + "/"
					+ fit_model_options.resolution_parametrization_file_url;

	fit_model_options.smearing_model = LumiFit::StringToModelType.at(
			pt.get<std::string>("fit.fit_model_options.smearing_model"));
	fit_model_options.use_resolution_parameter_interpolation = pt.get<bool>(
			"fit.fit_model_options.use_resolution_parameter_interpolation");

	fit_model_options.vertex_model = LumiFit::StringToModelType.at(
			pt.get<std::string>("fit.fit_model_options.vertex_model"));

	lmd_fit_facade.setModelFitOptions(fit_model_options);

	BOOST_FOREACH(boost::property_tree::ptree::value_type& free_param_name, pt.get_child(
					"fit.free_parameter_names")){
	lmd_fit_facade.addFreeFitParameter(free_param_name.first);
}
}

void PndLmdRuntimeConfiguration::initDataFacadeFromConfigFile(
		PndLmdDataFacade &lmd_data_facade, const std::string &file_url) {
	// Create an empty property tree object
	boost::property_tree::ptree pt;
	// read the config file
	read_json(file_url, pt);

	// set some dimension parameters
	primary_dimension.is_active = pt.get<bool>(
			"general_data.primary_dimension.is_active");

	primary_dimension.dimension_options.dimension_type =
			LumiFit::StringToDimensionType.at(
					pt.get<std::string>("general_data.primary_dimension.dimension_type"));
	primary_dimension.dimension_options.track_param_type =
			LumiFit::StringToTrackParamType.at(
					pt.get<std::string>(
							"general_data.primary_dimension.track_param_type"));

	primary_dimension.bins = pt.get<int>("general_data.primary_dimension.bins");
	primary_dimension.dimension_range.setRangeLow(
			pt.get<double>("general_data.primary_dimension.range_low"));
	primary_dimension.dimension_range.setRangeHigh(
			pt.get<double>("general_data.primary_dimension.range_high"));

	secondary_dimension.is_active = pt.get<bool>(
			"general_data.secondary_dimension.is_active");

	secondary_dimension.dimension_options.dimension_type =
			LumiFit::StringToDimensionType.at(
					pt.get<std::string>(
							"general_data.secondary_dimension.dimension_type"));
	secondary_dimension.dimension_options.track_param_type =
			LumiFit::StringToTrackParamType.at(
					pt.get<std::string>(
							"general_data.secondary_dimension.track_param_type"));

	secondary_dimension.bins = pt.get<int>(
			"general_data.secondary_dimension.bins");
	secondary_dimension.dimension_range.setRangeLow(
			pt.get<double>("general_data.secondary_dimension.range_low"));
	secondary_dimension.dimension_range.setRangeHigh(
			pt.get<double>("general_data.secondary_dimension.range_high"));

	BOOST_FOREACH(boost::property_tree::ptree::value_type& sel_dim, pt.get_child(
					"selections")){
	LumiFit::LmdDimension selection_dimension;

	selection_dimension.is_active = pt.get<bool>(
			"general_data.primary_dimension.is_active");

	if(selection_dimension.is_active) {
		selection_dimension.dimension_options.dimension_type =
		LumiFit::StringToDimensionType.at(
				sel_dim.second.get<std::string>("dimension_type"));
		selection_dimension.dimension_options.track_param_type =
		LumiFit::StringToTrackParamType.at(
				sel_dim.second.get<std::string>(
						"track_param_type"));

		selection_dimension.bins = sel_dim.second.get<int>(
				"bins");
		selection_dimension.dimension_range.setRangeLow(
				sel_dim.second.get<double>("range_low"));
		selection_dimension.dimension_range.setRangeHigh(
				sel_dim.second.get<double>("range_high"));

		selection_dimensions.first = selection_dimensions.first + "_" + sel_dim.second.get<std::string>("name_suffix");
		selection_dimensions.second.push_back(selection_dimension);
	}
}

	if (primary_dimension.is_active) {
		efficiency_primary_dimension.is_active = primary_dimension.is_active;
		efficiency_primary_dimension.dimension_options.dimension_type =
				primary_dimension.dimension_options.dimension_type;
		efficiency_primary_dimension.dimension_options.track_param_type =
				primary_dimension.dimension_options.track_param_type;

		efficiency_primary_dimension.bins = pt.get<int>(
				"efficiency.primary_dimension.bins");
		efficiency_primary_dimension.dimension_range.setRangeLow(
				pt.get<double>("efficiency.primary_dimension.range_low"));
		efficiency_primary_dimension.dimension_range.setRangeHigh(
				pt.get<double>("efficiency.primary_dimension.range_high"));
	}
	if (secondary_dimension.is_active) {
		efficiency_secondary_dimension.is_active = secondary_dimension.is_active;
		efficiency_secondary_dimension.dimension_options.dimension_type =
				secondary_dimension.dimension_options.dimension_type;
		efficiency_secondary_dimension.dimension_options.track_param_type =
				secondary_dimension.dimension_options.track_param_type;

		efficiency_secondary_dimension.bins = pt.get<int>(
				"efficiency.secondary_dimension.bins");
		efficiency_secondary_dimension.dimension_range.setRangeLow(
				pt.get<double>("efficiency.secondary_dimension.range_low"));
		efficiency_secondary_dimension.dimension_range.setRangeHigh(
				pt.get<double>("efficiency.secondary_dimension.range_high"));
	}

	if (primary_dimension.is_active) {
		resolution_primary_dimension.is_active = pt.get<bool>(
				"resolution.primary_dimension.is_active");
		resolution_primary_dimension.dimension_options.dimension_type =
				primary_dimension.dimension_options.dimension_type;
		resolution_primary_dimension.dimension_options.track_param_type =
				primary_dimension.dimension_options.track_param_type;

		resolution_primary_dimension.bins = pt.get<int>(
				"resolution.primary_dimension.bins");
		resolution_primary_dimension.dimension_range.setRangeLow(
				pt.get<double>("resolution.primary_dimension.range_low"));
		resolution_primary_dimension.dimension_range.setRangeHigh(
				pt.get<double>("resolution.primary_dimension.range_high"));
	}
	if (secondary_dimension.is_active) {
		resolution_secondary_dimension.is_active = pt.get<bool>(
				"resolution.secondary_dimension.is_active");
		resolution_secondary_dimension.dimension_options.dimension_type =
				secondary_dimension.dimension_options.dimension_type;
		resolution_secondary_dimension.dimension_options.track_param_type =
				secondary_dimension.dimension_options.track_param_type;

		resolution_secondary_dimension.bins = pt.get<int>(
				"resolution.secondary_dimension.bins");
		resolution_secondary_dimension.dimension_range.setRangeLow(
				pt.get<double>("resolution.secondary_dimension.range_low"));
		resolution_secondary_dimension.dimension_range.setRangeHigh(
				pt.get<double>("resolution.secondary_dimension.range_high"));
	}

	if (primary_dimension.is_active) {
		resolution_primary_selection_dimension.is_active =
				primary_dimension.is_active;
		resolution_primary_selection_dimension.dimension_options.dimension_type =
				primary_dimension.dimension_options.dimension_type;
		resolution_primary_selection_dimension.dimension_options.track_param_type =
				primary_dimension.dimension_options.track_param_type;
		resolution_primary_selection_dimension.dimension_options.track_type =
				LumiFit::MC;

		resolution_primary_selection_dimension.bins = pt.get<int>(
				"resolution.primary_selection_dimension.bins");
		resolution_primary_selection_dimension.dimension_range.setRangeLow(
				pt.get<double>("resolution.primary_selection_dimension.range_low"));
		resolution_primary_selection_dimension.dimension_range.setRangeHigh(
				pt.get<double>("resolution.primary_selection_dimension.range_high"));
	}
	if (secondary_dimension.is_active) {
		resolution_secondary_selection_dimension.is_active =
				secondary_dimension.is_active;
		resolution_secondary_selection_dimension.dimension_options.dimension_type =
				secondary_dimension.dimension_options.dimension_type;
		resolution_secondary_selection_dimension.dimension_options.track_param_type =
				secondary_dimension.dimension_options.track_param_type;
		resolution_secondary_selection_dimension.dimension_options.track_type =
				LumiFit::MC;

		resolution_secondary_selection_dimension.bins = pt.get<int>(
				"resolution.secondary_selection_dimension.bins");
		resolution_secondary_selection_dimension.dimension_range.setRangeLow(
				pt.get<double>("resolution.secondary_selection_dimension.range_low"));
		resolution_secondary_selection_dimension.dimension_range.setRangeHigh(
				pt.get<double>("resolution.secondary_selection_dimension.range_high"));
	}
}
