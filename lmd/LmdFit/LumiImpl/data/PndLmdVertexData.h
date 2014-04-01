/*
 * PndLmdVertexData.h
 *
 *  Created on: Mar 26, 2014
 *      Author: steve
 */

#ifndef PNDLMDVERTEXDATA_H_
#define PNDLMDVERTEXDATA_H_

#include "PndLmdHistogramData.h"

class PndLmdVertexData: public PndLmdHistogramData {
	LumiFit::LmdSimIPParameters simulation_ip_parameters;
	bool simulation_ip_parameters_exist;

public:
	PndLmdVertexData();
	virtual ~PndLmdVertexData();

	const LumiFit::LmdSimIPParameters& getSimulationIPParameters() const;
	void setSimulationIPParameters(
			LumiFit::LmdSimIPParameters& simulation_ip_parameters_);

	ClassDef(PndLmdVertexData, 1)
};

#endif /* PNDLMDVERTEXDATA_H_ */
