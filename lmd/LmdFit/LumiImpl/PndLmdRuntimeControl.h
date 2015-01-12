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
	double momentum;

	PndLmdRuntimeConfiguration lmd_runtime_config;

	PndLmdDataFacade lmd_data_facade;
	PndLmdFitFacade lmd_fit_facade;

public:
	PndLmdRuntimeControl();
	virtual ~PndLmdRuntimeControl();

	void initRuntimeConfiguration(const std::string& config_url);

	void setFitPaths(const std::string& input_file_dir,
			const std::string& acceptance_file_dir,
			const std::string& reference_acceptance_file_dir);

	void createData();

	void doLuminosityFits();
};

#endif /* PNDLMDRUNTIMECONTROL_H_ */
