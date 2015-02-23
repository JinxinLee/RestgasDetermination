/*
 * PndLmdDPMModel2D.cxx
 *
 *  Created on: Jan 17, 2013
 *      Author: steve
 */

#include "PndLmdFastDPMAngModel2D.h"

#include "TMath.h"
#include "TVector3.h"

PndLmdFastDPMAngModel2D::PndLmdFastDPMAngModel2D(std::string name_,
		shared_ptr<PndLmdDPMAngModel1D> dpm_model_1d_) :
		Model2D(name_), dpm_model_1d(dpm_model_1d_) {
	initModelParameters();
	this->addModelToList(dpm_model_1d);
	//getModelParameterSet().addModelParameters(
	//		dpm_model_1d.getModelParameterSet());
}

PndLmdFastDPMAngModel2D::~PndLmdFastDPMAngModel2D() {
	// TODO Auto-generated destructor stub
}

void PndLmdFastDPMAngModel2D::initModelParameters() {
	tilt_x = getModelParameterSet().addModelParameter("tilt_x");
	tilt_x->setValue(0.0);
	tilt_y = getModelParameterSet().addModelParameter("tilt_y");
	tilt_y->setValue(0.0);
}

std::pair<double, double> PndLmdFastDPMAngModel2D::calculateThetaFromTiltedSystem(
		const double thetax, const double thetay) const {

	double x = tan(tilt_x->getValue());
	double y = tan(tilt_y->getValue());
	TVector3 tilt(x, y, 1.0);
	TVector3 rotate_axis(y, -x, 0.0);
	double theta = atan(sqrt(pow(thetax, 2.0) + pow(thetay, 2.0)));
	double phi = atan2(thetay, thetax);
	TVector3 measured_direction(sin(theta) * cos(phi), sin(theta) * sin(phi),
			cos(theta));
	measured_direction.Rotate(tilt.Theta(), rotate_axis);
	return std::make_pair(measured_direction.Theta(), measured_direction.Phi());

	//double tilted_thetax = thetax - tilt_x->getValue();
	//double tilted_thetay = thetay - tilt_y->getValue();
	//return std::make_pair(
	//		atan(sqrt(pow(tilted_thetax, 2.0) + pow(tilted_thetay, 2.0))),
	//		atan2(tilted_thetay, tilted_thetax));
}

double PndLmdFastDPMAngModel2D::calculateJacobianDeterminant(
		const double thetax, const double thetay) const {
	double e_m = 1.0 * 1e-16; // machine precision
	 double htx = pow(e_m, 0.33) * thetax;
	 double hty = pow(e_m, 0.33) * thetay;

	 std::pair<double, double> shift_plus_ht = calculateThetaFromTiltedSystem(
	 thetax + htx, thetay);
	 std::pair<double, double> shift_min_ht = calculateThetaFromTiltedSystem(
	 thetax - htx, thetay);
	 std::pair<double, double> shift_plus_hp = calculateThetaFromTiltedSystem(
	 thetax, thetay + hty);
	 std::pair<double, double> shift_min_hp = calculateThetaFromTiltedSystem(
	 thetax, thetay - hty);

	 double j11 = (shift_plus_ht.first - shift_min_ht.first) / (2 * htx);
	 double j12 = (shift_plus_hp.first - shift_min_hp.first) / (2 * hty);
	 double j21 = (shift_plus_ht.second - shift_min_ht.second) / (2 * htx);
	 double j22 = (shift_plus_hp.second - shift_min_hp.second) / (2 * hty);

	 return j11 * j22 - j21 * j12;

	// analytic formula
	/*double xypowsum = pow(thetax, 2.0) + pow(thetay, 2.0);

	return 1.0 / (sqrt(xypowsum) * (1 + xypowsum));*/
}

double PndLmdFastDPMAngModel2D::eval(const double *x) const {
	double jaco = calculateJacobianDeterminant(x[0], x[1]);
	double theta_tilted = calculateThetaFromTiltedSystem(x[0], x[1]).first;

	/*std::cout << "measured thetax, thetay: " << x[0] << "," << x[1]
	 << " -> transforms to evaluated theta of: " << theta_tilted << std::endl;
	 std::cout << "jacobian: " << jaco << std::endl;*/
	/*if(theta_tilted > 0.001 && theta_tilted < 0.002) {
	 return jaco*1.0;
	 }
	 return 0.0;*/
	return jaco * dpm_model_1d->eval(&theta_tilted) / (2.0 * TMath::Pi());
}

void PndLmdFastDPMAngModel2D::updateDomain() {
	setVar1Domain(-TMath::Pi(), TMath::Pi());
	setVar2Domain(-TMath::Pi(), TMath::Pi());
}
