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

#ifdef HAS_SHAREDPOINTER
#include <memory>
using std::shared_ptr;
#else
#include <tr1/memory>
using std::tr1::shared_ptr;
#endif

class TGraphErrors;

class ROOTPlotter {
private:
public:
	ROOTPlotter();
	virtual ~ROOTPlotter();

	TGraphErrors* createGraphFromModel1D(shared_ptr<Model> model,
			ModelVisualizationProperties1D &visualization_properties);
};

#endif /* ROOTPLOTTER_H_ */
