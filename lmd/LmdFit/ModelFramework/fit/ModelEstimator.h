/*
 * ModelEstimator.h
 *
 *  Created on: Jun 5, 2013
 *      Author: steve
 */

#ifndef MODELESTIMATOR_H_
#define MODELESTIMATOR_H_

#include "ModelStructs.h"
#include "ModelControlParameter.h"
#include "EstimatorOptions.h"

#include <map>
#include <vector>
#include <tr1/memory>

class Data;
class Model;
class ModelPar;

using std::tr1::shared_ptr;

class ModelEstimator: public ModelControlParameter {
private:
	// list of free parameters
	std::map<std::pair<std::string, std::string>, shared_ptr<ModelPar>
			, ModelStructs::stringpair_comp> free_parameters;

	void insertParameters();

	void updateFreeModelParameters(const double *new_values);

protected:
	// data
	shared_ptr<Data> data;
	// model used for fitting
	shared_ptr<Model> fit_model;

public:
	ModelEstimator();
	virtual ~ModelEstimator();

	const shared_ptr<Model> getModel() const;
	void setModel(shared_ptr<Model> new_model);

	const shared_ptr<Data> getData() const;
	void setData(shared_ptr<Data> new_data);

	std::vector<shared_ptr<ModelPar> >& getFreeParameterList();

	double evaluate(const double *par);

	void applyEstimatorOptions(const EstimatorOptions &estimator_options);

	/**
	 * The estimator function (chi2, likelihood, etc)
	 */
	virtual double eval() const =0;
};

#endif /* MODELESTIMATOR_H_ */
