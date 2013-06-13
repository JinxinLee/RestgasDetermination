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

#include <tr1/memory>

class Model;
class ModelPar;

using std::tr1::shared_ptr;

class ModelEstimator : public ModelControlParameter {
private:
	// list of free parameters
	std::vector<shared_ptr<ModelPar> > free_parameters;

	void insertParameters();

	void updateFreeModelParameters(const double *new_values);

protected:
	// this is the vector that stores the information for the chi2 calculation
	std::vector<ModelStructs::data_point> data_points;

	// binning factor
	double binning_factor;

	// model used for fitting
	shared_ptr<Model> fit_model;

public:
	ModelEstimator();
	virtual ~ModelEstimator();

	unsigned int getNumberOfDataPoints() const;
	const shared_ptr<Model> getModel() const;

	void setBinningFactor(double binning_factor_);
	void setModel(shared_ptr<Model> new_model);

	std::vector<shared_ptr<ModelPar> >& getFreeParameterList();

	void clearData();
	void insertData(std::vector<ModelStructs::data_point> &data_points_);
	void insertData(ModelStructs::data_point &data_point_);

	double evaluate(const double *par);

	// the chisquare function
	virtual double eval() const =0;
};

#endif /* MODELESTIMATOR_H_ */
