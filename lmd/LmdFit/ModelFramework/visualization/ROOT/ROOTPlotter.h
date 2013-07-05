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

#include <tr1/memory>

class TGraphErrors;

using std::tr1::shared_ptr;

class ROOTPlotter {
private:
public:
	ROOTPlotter();
	virtual ~ROOTPlotter();

	TGraphErrors* createGraphFromModel1D(shared_ptr<Model> model,
			ModelVisualizationProperties1D &visualization_properties);
};

#endif /* ROOTPLOTTER_H_ */
