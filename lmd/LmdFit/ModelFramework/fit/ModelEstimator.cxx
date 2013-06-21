/*
 * ModelEstimator.cxx
 *
 *  Created on: Jun 5, 2013
 *      Author: steve
 */

#include "ModelEstimator.h"
#include "Model.h"
#include "ModelPar.h"
#include "Data.h"

#include <cmath>

ModelEstimator::ModelEstimator() {
	// TODO Auto-generated constructor stub

}

ModelEstimator::~ModelEstimator() {
	// TODO Auto-generated destructor stub
}

const shared_ptr<Model> ModelEstimator::getModel() const {
	return fit_model;
}

void ModelEstimator::setModel(shared_ptr<Model> new_model) {
	fit_model = new_model;

	// get list of all free parameters
	getParameterList().clear();
	free_parameters.clear();
	free_parameters = fit_model->getModelParameterSet().getFreeModelParameters();
	insertParameters();
}

const shared_ptr<Data> ModelEstimator::getData() const {
	return data;
}

void ModelEstimator::setData(shared_ptr<Data> new_data) {
	data = new_data;
}

void ModelEstimator::insertParameters() {
	for (std::map<std::pair<std::string, std::string>, shared_ptr<ModelPar>
			, ModelStructs::stringpair_comp>::iterator it = free_parameters.begin();
			it != free_parameters.end(); it++) {
		getParameterList().push_back(
				ModelStructs::minimization_parameter(it->first, it->second->getValue(),
						0.0));
	}
}

void ModelEstimator::updateFreeModelParameters(const double *new_values) {
	int counter = 0;
	// first overwrite the corresponding parameter values
	// loop over the parameter set and update all the free parameters with these values
	for (std::map<std::pair<std::string, std::string>, shared_ptr<ModelPar>
			, ModelStructs::stringpair_comp>::iterator it = free_parameters.begin();
			it != free_parameters.end(); it++) {
		it->second->setValue(new_values[counter]);
		counter++;
	}
}

void ModelEstimator::applyEstimatorOptions(
		const EstimatorOptions &estimator_options) {
	std::vector<DataPointProxy> &datapoints = data->getData();
	for (unsigned int i = 0; i < datapoints.size(); i++) {
		if (datapoints[i].isBinnedDataPoint()) {
			shared_ptr<DataStructs::binned_data_point> data_point =
					datapoints[i].getBinnedDataPoint();
			// check fit ranges
			if (data->getDimension() > 0 && estimator_options.isFitRangeXUsed()) {
				if (data_point->bin_center_value[0]
						< estimator_options.getFitRangeX().first
						|| data_point->bin_center_value[0]
								> estimator_options.getFitRangeX().second) {
					datapoints[i].setIsPointUsed(false);
					continue;
				}
				if (data->getDimension() > 1 && estimator_options.isFitRangeYUsed()) {
					if (data_point->bin_center_value[0]
							< estimator_options.getFitRangeY().first
							|| data_point->bin_center_value[0]
									> estimator_options.getFitRangeY().second) {
						datapoints[i].setIsPointUsed(false);
						continue;
					}
				}
			}
			if (estimator_options.isWithIntegralScaling()) {
				if (fit_model.get()) {
					std::vector<std::pair<double, double> > bin_ranges;
					for (unsigned int dim = 0; dim < data->getDimension(); dim++) {
						std::pair<double, double> bin_range;
						bin_range.first = data_point->bin_center_value[dim]
								- data_point->bin_widths[dim] / 2.0;
						bin_range.second = bin_range.first + data_point->bin_widths[dim];
						bin_ranges.push_back(bin_range);
					}

					double scale = 1.0;
					double int_func_real = fit_model->Integral(bin_ranges, 0.001);
					double int_func_approx = fit_model->evaluate(
							data_point->bin_center_value);
					for (unsigned int dim = 0; dim < data->getDimension(); dim++) {
						int_func_approx *= (bin_ranges[dim].second - bin_ranges[dim].first);
					}

					if (int_func_approx > 0.0 && int_func_real > 0.0) {
						scale = int_func_approx / int_func_real;
					}
					data_point->scale = scale;
					data_point->z = data_point->z * scale;
					data_point->z_error = data_point->z_error * sqrt(scale);
				}
			}
		} else if (datapoints[i].isUnbinnedDataPoint()) {
			// TODO implement for unbinned data
		} else {
			// this point is neither...

		}
	}
}

double ModelEstimator::evaluate(const double *par) {
	/* This point is crucial: because this method is called for every iteration
	 * of the fitter, the "newly changed" parameters have to be updated in the
	 * model so that the changes are actually registered.
	 */
	updateFreeModelParameters(par);

	return eval();
}
