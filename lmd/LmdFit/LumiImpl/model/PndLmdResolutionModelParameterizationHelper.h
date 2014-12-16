/*
 * PndLmdResolutionModelParameterizationHelper.h
 *
 *  Created on: Jul 30, 2014
 *      Author: steve
 */

#ifndef PNDLMDRESOLUTIONMODELPARAMETERIZATIONHELPER_H_
#define PNDLMDRESOLUTIONMODELPARAMETERIZATIONHELPER_H_

#include "core/Model.h"
#include "LumiModelOptions.h"

class PndLmdResolutionModelParameterizationHelper {
	shared_ptr<Model> resolution_model;
	LumiFit::PndLmdFitModelOptions model_options;

	void addPolynomialParametrizationModelToResolutionModelParameter(
			const std::string &parameter_name);

	void addAndInitDataParametrizationModelForResolutionModelParameter(
			const std::string &parameter_name);

	void initParametrizationModelParametersFromFile();

public:
	PndLmdResolutionModelParameterizationHelper(
			shared_ptr<Model> resolution_model_,
			const LumiFit::PndLmdFitModelOptions& model_options_);
	virtual ~PndLmdResolutionModelParameterizationHelper();

	void createAndInitializeParametrizationModelsForParameters(
			const std::vector<std::string> &parameter_names);
};

#endif /* PNDLMDRESOLUTIONMODELPARAMETERIZATIONHELPER_H_ */
