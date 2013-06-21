/*
 * ROOTPlotter.cxx
 *
 *  Created on: Jun 14, 2013
 *      Author: steve
 */

#include "ROOTPlotter.h"
#include "ModelVisualizationProperties1D.h"

#include "TGraphErrors.h"

#include <iostream>

ROOTPlotter::ROOTPlotter() {
	// TODO Auto-generated constructor stub

}

ROOTPlotter::~ROOTPlotter() {
	// TODO Auto-generated destructor stub
}

TGraphErrors* ROOTPlotter::createGraphFromModel1D(shared_ptr<Model1D> model,
		ModelVisualizationProperties1D &visualization_properties) {

	TGraphErrors* graph = new TGraphErrors(
			visualization_properties.getEvaluations());

	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	double stepsize = (visualization_properties.getPlotRange().second
			- visualization_properties.getPlotRange().first)
			/ visualization_properties.getEvaluations();
	double x;
	for (unsigned int i = 0; i < visualization_properties.getEvaluations(); i++) {
		x = visualization_properties.getPlotRange().first + stepsize * i;
		graph->SetPoint(i, x,
				model->evaluate(&x) * visualization_properties.getBinningFactor());
		graph->SetPointError(i, 0, 0);
	}
	return graph;
}
