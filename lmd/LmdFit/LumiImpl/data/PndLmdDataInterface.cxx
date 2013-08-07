/*
 * PndLmdDataInterface.cxx
 *
 *  Created on: Apr 17, 2013
 *      Author: steve
 */

#include "PndLmdDataInterface.h"
#include "PndLmdLumiHelper.h"

#include <iostream>
#include <tr1/memory>

using std::tr1::shared_ptr;

ClassImp(PndLmdDataInterface)

PndLmdDataInterface::PndLmdDataInterface() {
	p_lab = 0.0;
}

PndLmdDataInterface::PndLmdDataInterface(double p_lab_) {
	p_lab = p_lab_;
}

PndLmdDataInterface::PndLmdDataInterface(double p_lab_,
		PndLmdFit::lmd_dimension th_dimension_,
		PndLmdFit::lmd_dimension phi_dimension_) {
	p_lab = p_lab_;
	th_dimension = th_dimension_;
	phi_dimension = phi_dimension_;
	// calculate momentum transfer histogram bounds
	t_dimension = PndLmdFit::lmd_dimension(
			th_dimension.bins,
			PndLmdLumiHelper::getMomentumTransferFromTheta(p_lab,
					th_dimension.range_low),
			PndLmdLumiHelper::getMomentumTransferFromTheta(p_lab,
					th_dimension.range_high));
	std::cout << "calulated t range: " << t_dimension.range_low << " - "
			<< t_dimension.range_high << std::endl;
}

PndLmdDataInterface::~PndLmdDataInterface() {
	// TODO Auto-generated destructor stub
}

int PndLmdDataInterface::getNumEvents() const {
	return num_events;
}

double PndLmdDataInterface::getLabMomentum() const {
	return p_lab;
}

TString& PndLmdDataInterface::getName() {
	return name;
}

PndLmdFit::lmd_dimension PndLmdDataInterface::getTDimension() const {
	return t_dimension;
}


PndLmdFit::lmd_dimension PndLmdDataInterface::getThetaDimension() const {
	return th_dimension;
}

PndLmdFit::lmd_dimension PndLmdDataInterface::getPhiDimension() const {
	return phi_dimension;
}


void PndLmdDataInterface::setNumEvents(int num_events_) {
	num_events = num_events_;
}

void PndLmdDataInterface::setLabMomentum(double p_lab_) {
	p_lab = p_lab_;
}

int PndLmdDataInterface::addFileToList(TString filepath) {
	return filepath_list.insert(filepath).second;
}
