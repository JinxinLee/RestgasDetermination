/*
 * ModelVisualizationProperties1D.h
 *
 *  Created on: Jun 15, 2013
 *      Author: steve
 */

#ifndef MODELVISUALIZATIONPROPERTIES1D_H_
#define MODELVISUALIZATIONPROPERTIES1D_H_

#include "Data.h"

#include <utility>

class ModelVisualizationProperties1D {
private:
	DataStructs::dimension_range plot_range;
	unsigned int evaluations;
	double binning_factor;

public:
	ModelVisualizationProperties1D();
	ModelVisualizationProperties1D(shared_ptr<Data> data);
	virtual ~ModelVisualizationProperties1D();

	double getBinningFactor() const;
	unsigned int getEvaluations() const;
	DataStructs::dimension_range getPlotRange() const;
	void setBinningFactor(double binning_factor_);
	void setEvaluations(unsigned int evaluations_);
	void setPlotRange(DataStructs::dimension_range dim_range_);
};

#endif /* MODELVISUALIZATIONPROPERTIES1D_H_ */
