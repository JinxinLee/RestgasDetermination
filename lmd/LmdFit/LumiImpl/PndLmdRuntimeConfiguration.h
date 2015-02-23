/*
 * PndLmdRuntimeConfiguration.h
 *
 *  Created on: Dec 17, 2014
 *      Author: steve
 */

#ifndef PNDLMDRUNTIMECONFIGURATION_H_
#define PNDLMDRUNTIMECONFIGURATION_H_

#include "fit/PndLmdLumiFitOptions.h"

class PndLmdFitFacade;
class PndLmdDataFacade;

class PndLmdRuntimeConfiguration {
	friend class PndLmdRuntimeControl;

	// directory paths
	std::string elastic_data_input_directory;
	std::string acceptance_resolution_input_directory;
	std::string reference_acceptance_resolution_input_directory;

	std::string raw_data_directory;
	std::string raw_data_filelist_path;
	std::string data_output_directory;

	// file names
	std::string elastic_data_name;
	std::string acc_data_name;
	std::string res_data_name;
	std::string res_param_data_name;
	std::string fitted_elastic_data_name;
	std::string vertex_data_name;

	// data stuff
	LumiFit::LmdDimension primary_dimension;
	LumiFit::LmdDimension secondary_dimension;

	std::pair<std::string, std::vector<LumiFit::LmdDimension> > selection_dimensions;

	LumiFit::LmdDimension efficiency_primary_dimension;
	LumiFit::LmdDimension efficiency_secondary_dimension;

	LumiFit::LmdDimension resolution_primary_dimension;
	LumiFit::LmdDimension resolution_secondary_dimension;

	LumiFit::LmdDimension resolution_primary_selection_dimension;
	LumiFit::LmdDimension resolution_secondary_selection_dimension;

public:
	PndLmdRuntimeConfiguration();
	virtual ~PndLmdRuntimeConfiguration();

	void initFitFacadeFromConfigFile(PndLmdFitFacade &lmd_fit_facade,
			const std::string &file_url);

	void initDataFacadeFromConfigFile(PndLmdDataFacade &lmd_data_facade,
			const std::string &file_url);
};

#endif /* PNDLMDRUNTIMECONFIGURATION_H_ */
