#include <PndLmdRuntimeConfiguration.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <iostream>

using boost::property_tree::ptree;

PndLmdRuntimeConfiguration::PndLmdRuntimeConfiguration() :
		elastic_data_name("lmd_data.root"), acc_data_name("lmd_acc_data.root"), res_data_name(
				"lmd_res_data.root"), res_param_data_name("resolution_params_1.root"), raw_elastic_data_directory(
				""), fitted_elastic_data_name("lmd_fitted_data.root"), raw_uniform_data_directory(
				""), elastic_data_output_directory(""), uniform_data_output_directory(
				""), data_types(""), total_elastic_cross_section(-1.0) {
}

PndLmdRuntimeConfiguration::~PndLmdRuntimeConfiguration() {
}

void PndLmdRuntimeConfiguration::initFitFacadeFromConfigFile(
		const std::string &file_url) {
	// Create an empty property tree object
	boost::property_tree::ptree pt;
	// read the config file
	read_json(file_url, pt);

	// estimator options
	estimator_type = LumiFit::StringToLmdEstimatorType.at(
			pt.get<std::string>("fit.estimator_type"));

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

	// fit options
	fit_model_options.fit_dimension = pt.get<int>(
			"fit.fit_model_options.fit_dimension");

	fit_model_options.dpm_elastic_parts = LumiFit::StringToDPMElasticParts.at(
			pt.get<std::string>("fit.fit_model_options.dpm_elastic_parts"));
	fit_model_options.momentum_transfer_active = pt.get<bool>(
			"fit.fit_model_options.momentum_transfer_active");

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

	BOOST_FOREACH(boost::property_tree::ptree::value_type& free_param_name, pt.get_child(
					"fit.free_parameter_names")){
	free_parameter_names.insert(free_param_name.first);
}
}
