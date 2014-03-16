/*
 * ROOTPlotter.cxx
 *
 *  Created on: Jun 14, 2013
 *      Author: steve
 */

#include "ROOTPlotter.h"
#include "ModelVisualizationProperties1D.h"

#include "TGraphAsymmErrors.h"

#include <iostream>

ROOTPlotter::ROOTPlotter() {
	// TODO Auto-generated constructor stub

}

ROOTPlotter::~ROOTPlotter() {
	// TODO Auto-generated destructor stub
}

TGraphAsymmErrors* ROOTPlotter::createGraphFromModel1D(shared_ptr<Model> model,
		ModelVisualizationProperties1D &visualization_properties) {

	TGraphAsymmErrors* graph = new TGraphAsymmErrors(
			visualization_properties.getEvaluations());

	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	double stepsize = visualization_properties.getPlotRange().getDimensionLength()
			/ visualization_properties.getEvaluations();
	double x;

	for (unsigned int i = 0; i < visualization_properties.getEvaluations(); i++) {
		x = visualization_properties.getPlotRange().range_low + stepsize * i;
		graph->SetPoint(i, x,
				model->evaluate(&x) * visualization_properties.getBinningFactor());
		graph->SetPointError(
				i,
				0,
				0,
				model->getUncertaincy(&x).first
						* visualization_properties.getBinningFactor(),
				model->getUncertaincy(&x).second
						* visualization_properties.getBinningFactor());
	}
	return graph;
}
