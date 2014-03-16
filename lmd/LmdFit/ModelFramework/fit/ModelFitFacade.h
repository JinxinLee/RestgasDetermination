/*
 * ModelFitFacade.h
 *
 *  Created on: Oct 7, 2013
 *      Author: steve
 */

#ifndef MODELFITFACADE_H_
#define MODELFITFACADE_H_

#include "ModelMinimizer.h"
#include "ModelEstimator.h"
#include "Model1D.h"

class ModelFitFacade {
private:
	shared_ptr<Data> data;
	shared_ptr<Model1D> model1d;

	shared_ptr<ModelEstimator> estimator;
	shared_ptr<ModelMinimizer> minimizer;

	EstimatorOptions estimator_options;

public:
	ModelFitFacade();
	virtual ~ModelFitFacade();

	shared_ptr<Data> getData() const;
	shared_ptr<ModelEstimator> getEstimator() const;
	const EstimatorOptions& getEstimatorOptions() const;
	shared_ptr<ModelMinimizer> getMinimizer() const;
	shared_ptr<Model1D> getModel1d() const;
	void setData(shared_ptr<Data> data_);
	void setEstimator(shared_ptr<ModelEstimator> estimator_);
	void setEstimatorOptions(const EstimatorOptions& est_opt_);
	void setMinimizer(shared_ptr<ModelMinimizer> minimizer_);
	void setModel1d(shared_ptr<Model1D> model1d_);

	ModelFitResult Fit();

};

#endif /* MODELFITFACADE_H_ */
