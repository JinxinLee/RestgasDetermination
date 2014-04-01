/*
 * PndLmdVertexData.cxx
 *
 *  Created on: Mar 26, 2014
 *      Author: steve
 */

#include "PndLmdVertexData.h"

ClassImp(PndLmdVertexData)

PndLmdVertexData::PndLmdVertexData() :
		simulation_ip_parameters_exist(false) {
	// TODO Auto-generated constructor stub

}

PndLmdVertexData::~PndLmdVertexData() {
	// TODO Auto-generated destructor stub
}

const LumiFit::LmdSimIPParameters& PndLmdVertexData::getSimulationIPParameters() const {
	return simulation_ip_parameters;
}
void PndLmdVertexData::setSimulationIPParameters(
		LumiFit::LmdSimIPParameters& simulation_ip_parameters_) {
	if (!simulation_ip_parameters_exist)
		simulation_ip_parameters = simulation_ip_parameters_;
}
