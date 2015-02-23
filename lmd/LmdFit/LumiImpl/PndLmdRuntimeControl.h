/*
 * PndLmdRuntimeControl.h
 *
 *  Created on: Dec 17, 2014
 *      Author: steve
 */

#ifndef PNDLMDRUNTIMECONTROL_H_
#define PNDLMDRUNTIMECONTROL_H_

#include "PndLmdRuntimeConfiguration.h"
#include "data/PndLmdDataFacade.h"
#include "fit/PndLmdFitFacade.h"

class PndLmdRuntimeControl {
	PndLmdRuntimeConfiguration lmd_runtime_config;

	PndLmdDataFacade lmd_data_facade;
	PndLmdFitFacade lmd_fit_facade;

	void applySelections();
	void applyAutomaticResolutionDimensionRanges();

	void createAngularData();
	void createEfficiencies();
	void createResolutions();
	void createVertexData();

public:
	double momentum;
	unsigned int num_events;
	double total_elastic_cross_section;

	PndLmdRuntimeControl();
	virtual ~PndLmdRuntimeControl();

	void initFitRuntimeConfiguration(const std::string& config_url);
	void initDataRuntimeConfiguration(const std::string& config_url);

	void setFitPaths(const std::string& input_file_dir,
			const std::string& acceptance_file_dir,
			const std::string& reference_acceptance_file_dir);

	void setDataPaths(const std::string &input_dir_path,
			const std::string &filelist_path, const std::string &output_dir_path);

	void createData(const std::string &data_types);

	void doLuminosityFits();
};

#endif /* PNDLMDRUNTIMECONTROL_H_ */
