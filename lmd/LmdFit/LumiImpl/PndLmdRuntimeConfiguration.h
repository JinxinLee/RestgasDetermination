/*
 * PndLmdRuntimeConfiguration.h
 *
 *  Created on: Dec 17, 2014
 *      Author: steve
 */

#ifndef PNDLMDRUNTIMECONFIGURATION_H_
#define PNDLMDRUNTIMECONFIGURATION_H_

#include "fit/PndLmdLumiFitOptions.h"

class PndLmdRuntimeConfiguration {
	friend class PndLmdRuntimeControl;

	// fit options
	LumiFit::PndLmdFitModelOptions fit_model_options;
  EstimatorOptions est_opt;
	LumiFit::LmdEstimatorType estimator_type;
	std::set<std::string, ModelStructs::string_comp> free_parameter_names;

	std::string elastic_data_input_directory;
	std::string acceptance_resolution_input_directory;
	std::string reference_acceptance_resolution_input_directory;

	// data options
	// file names
	std::string elastic_data_name;
	std::string acc_data_name;
	std::string res_data_name;
	std::string res_param_data_name;
	std::string fitted_elastic_data_name;

	std::string raw_elastic_data_directory;
	std::string raw_uniform_data_directory;

	std::string elastic_data_output_directory;
	std::string uniform_data_output_directory;

	std::string data_types;

	double total_elastic_cross_section;

public:
	PndLmdRuntimeConfiguration();
	virtual ~PndLmdRuntimeConfiguration();

	void initFitFacadeFromConfigFile(const std::string &file_url);
};

#endif /* PNDLMDRUNTIMECONFIGURATION_H_ */
