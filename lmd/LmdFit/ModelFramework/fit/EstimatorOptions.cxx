/*
 * EstimatorOptions.cxx
 *
 *  Created on: Jun 16, 2013
 *      Author: steve
 */

#include "EstimatorOptions.h"

EstimatorOptions::EstimatorOptions() {
	// TODO Auto-generated constructor stub

}

EstimatorOptions::~EstimatorOptions() {
	// TODO Auto-generated destructor stub
}

bool EstimatorOptions::isWithIntegralScaling() const {
	return with_integral_scaling;
}

void EstimatorOptions::setWithIntegralScaling(bool with_integral_scaling_) {
	with_integral_scaling = with_integral_scaling_;
}

std::pair<double, double> EstimatorOptions::getFitRangeX() const {
	return fit_range_x;
}

std::pair<double, double> EstimatorOptions::getFitRangeY() const {
	return fit_range_y;
}

void EstimatorOptions::setFitRangeX(std::pair<double, double> fit_range_x_) {
	fit_range_x_used = true;
	fit_range_x = fit_range_x_;
}

void EstimatorOptions::setFitRangeY(std::pair<double, double> fit_range_y_) {
	fit_range_y_used = true;
	fit_range_y = fit_range_y_;
}

bool EstimatorOptions::isFitRangeXUsed() const {
	return fit_range_x_used;
}

bool EstimatorOptions::isFitRangeYUsed() const {
	return fit_range_y_used;
}

void EstimatorOptions::setFitRangeXUsed(bool fit_range_x_used_) {
	fit_range_x_used = fit_range_x_used_;
}

void EstimatorOptions::setFitRangeYUsed(bool fit_range_y_used_) {
	fit_range_y_used = fit_range_y_used_;
}
