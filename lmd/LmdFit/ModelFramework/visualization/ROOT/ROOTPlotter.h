/*
 * ROOTPlotter.h
 *
 *  Created on: Jun 14, 2013
 *      Author: steve
 */

#ifndef ROOTPLOTTER_H_
#define ROOTPLOTTER_H_

#include "Model.h"
#include "ModelVisualizationProperties1D.h"

#include "SharedPtr.h"

class TGraphAsymmErrors;

class ROOTPlotter {
private:
public:
	ROOTPlotter();
	virtual ~ROOTPlotter();

	TGraphAsymmErrors* createGraphFromModel1D(shared_ptr<Model> model,
			ModelVisualizationProperties1D &visualization_properties);
};

#endif /* ROOTPLOTTER_H_ */
