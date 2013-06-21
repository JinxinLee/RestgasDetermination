/*
 * EstimatorOptions.h
 *
 *  Created on: Jun 16, 2013
 *      Author: steve
 */

#ifndef ESTIMATOROPTIONS_H_
#define ESTIMATOROPTIONS_H_

#include <utility>

class EstimatorOptions {
private:
	bool with_integral_scaling;
	bool fit_range_x_used;
	bool fit_range_y_used;
	std::pair<double, double> fit_range_x;
	std::pair<double, double> fit_range_y;

public:
	EstimatorOptions();
	virtual ~EstimatorOptions();
	bool isWithIntegralScaling() const;
	void setWithIntegralScaling(bool with_integral_scaling_);
	std::pair<double, double> getFitRangeX() const;
	std::pair<double, double> getFitRangeY() const;
	void setFitRangeX(std::pair<double, double> fit_range_x_);
	void setFitRangeY(std::pair<double, double> fit_range_y_);
	bool isFitRangeXUsed() const;
	bool isFitRangeYUsed() const;
	void setFitRangeXUsed(bool fit_range_x_used_);
	void setFitRangeYUsed(bool fit_range_y_used_);
};

#endif /* ESTIMATOROPTIONS_H_ */
