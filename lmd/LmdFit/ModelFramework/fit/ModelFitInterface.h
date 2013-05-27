/*
 * ModelFitInterface.h
 *
 *  Created on: May 2, 2013
 *      Author: steve
 */

#ifndef MODELFITINTERFACE_H_
#define MODELFITINTERFACE_H_

#include <vector>
#include <tr1/memory>

class Model;
class ModelPar;

using std::tr1::shared_ptr;

/**
 * Fit Interface class. Concrete implementations (for example PndLmdModelFitter
 * using ROOT's Minuit Minimizer) need to derive from this class and implement
 * the #minimize() function that interfaces to the minimizer.
 */
class ModelFitInterface {
public:
	// data structure
	struct data_point {
		double x[2]; // max 2 dimensional fits
		double z;
		double z_error;
		double scale;
	};

	virtual int minimize() =0;

private:
	// this is the vector that stores the information for the chi2 calculation
	std::vector<data_point> data_points;

	// binning factor
	double binning_factor;

	// list of free parameters
	std::vector<shared_ptr<ModelPar> > free_parameters;

	void updateFreeModelParameters(const double *new_values);

protected:
	// model used for fitting
	shared_ptr<Model> model;

	void clearData();
	void insertData(std::vector<data_point> &data_points_);
	void insertData(data_point &data_point_);

public:
	ModelFitInterface();
	virtual ~ModelFitInterface();

	void setModel(shared_ptr<Model> new_model);

	void setBinningFactor(double binning_factor_);

	unsigned int getNumberOfDataPoints() const;

	std::vector<shared_ptr<ModelPar> >& getFreeParameterList();

	// the chisquare function
	double chi2(const double *par);

	int doFit();
};

#endif /* MODELFITINTERFACE_H_ */
