/*
 * ModelVisualizationProperties1D.cxx
 *
 *  Created on: Jun 15, 2013
 *      Author: steve
 */

#include "ModelVisualizationProperties1D.h"

ModelVisualizationProperties1D::ModelVisualizationProperties1D() : binning_factor(1.0) {
	// TODO Auto-generated constructor stub
	evaluations = 500;
}

ModelVisualizationProperties1D::ModelVisualizationProperties1D(shared_ptr<Data> data) {
	// TODO Auto-generated constructor stub
	evaluations = 500;
	binning_factor = data->getBinningFactor();
}

double ModelVisualizationProperties1D::getBinningFactor() const {
	return binning_factor;
}

unsigned int ModelVisualizationProperties1D::getEvaluations() const {
	return evaluations;
}

DataStructs::dimension_range ModelVisualizationProperties1D::getPlotRange() const {
	return plot_range;
}

void ModelVisualizationProperties1D::setBinningFactor(double binning_factor_) {
	binning_factor = binning_factor_;
}

void ModelVisualizationProperties1D::setEvaluations(unsigned int evaluations_) {
	this->evaluations = evaluations_;
}

void ModelVisualizationProperties1D::setPlotRange(
		DataStructs::dimension_range plot_range_) {
	plot_range = plot_range_;
}

ModelVisualizationProperties1D::~ModelVisualizationProperties1D() {
	// TODO Auto-generated destructor stub
}

