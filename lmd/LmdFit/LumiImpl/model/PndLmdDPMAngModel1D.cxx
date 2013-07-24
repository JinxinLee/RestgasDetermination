/*
 * PndLmdDPMAngModel1D.cxx
 *
 *  Created on: Mar 7, 2013
 *      Author: steve
 */

#include "PndLmdDPMAngModel1D.h"

#include <iostream>

#include "TMath.h"

PndLmdDPMAngModel1D::PndLmdDPMAngModel1D(std::string name_, dpm_elastic_parts elastic_type_) :
		PndLmdDPMMTModel1D(name_, elastic_type_) {
	// TODO Auto-generated constructor stub

}

PndLmdDPMAngModel1D::~PndLmdDPMAngModel1D() {
	// TODO Auto-generated destructor stub
}

double PndLmdDPMAngModel1D::getMomentumTransferFromTheta(
		const double theta) const {
	//double gtan = gamma->getValue() * TMath::Tan(theta / 1000.);

	// ultrarelativistic approximation (actually for protons this is would start at about 15GeV
	// or even higher so really bad approximation in our case)
	/* double thetacm = 2.0 * TMath::ATan(gtan);
	 double tapprox = -2.0 * pcm2->getValue() * (1.0 - cos(thetacm)); //(this formula is exact just thetacm above is approx)

	 // a * sin(x) / (a * cos(x) + b) == c   where x=theta_cm, a = plab, b = v*Elab, c = gamma*tan(theta_lab)
	 // solution is: x = acos([+-sqrt(a^4*c^2+a^4-(a*b*c)^2) - a*b*c^2]/[(a*c)^2 + a^2])
	 double sqrt_abc = sqrt(
	 pow(p_lab->getValue(), 4.0) * (pow(gtan, 2.0) + 1.0)
	 - pow(
	 p_lab->getValue() * beta_lab_cms->getValue() * E_lab->getValue()
	 * gtan, 2.0));
	 // ok at pi/2 there is sign switching so we need a case statement to extend our function to the full pi range
	 double texact = 0.0;
	 if (theta / 1000.0 < C_PI / 2) {
	 texact = -2.0 * pcm2->getValue()
	 * (1.0
	 - (sqrt_abc
	 - p_lab->getValue() * beta_lab_cms->getValue()
	 * E_lab->getValue() * pow(gtan, 2.0))
	 / (pow(p_lab->getValue() * gtan, 2.0)
	 + pow(p_lab->getValue(), 2.0)));
	 } else {
	 texact = -2.0 * pcm2->getValue()
	 * (1.0
	 - (-sqrt_abc
	 - p_lab->getValue() * beta_lab_cms->getValue()
	 * E_lab->getValue() * pow(gtan, 2.0))
	 / (pow(p_lab->getValue() * gtan, 2.0)
	 + pow(p_lab->getValue(), 2.0)));
	 }*/
	double texact_old =
			-2.0 * pcm2->getValue()
					* (1.0
							- cos(
									atan(
											p_lab->getValue() * sin(theta / 1000.0)
													/ (gamma->getValue()
															* (p_lab->getValue() * cos(theta / 1000.0)
																	- beta_lab_cms->getValue() * E_lab->getValue())))));
	// std::cout<<texact<<" =? "<<texact_old<<" =? "<<tapprox<<std::endl;
	return texact_old;
}

double PndLmdDPMAngModel1D::getThetaMomentumTransferJacobian(
		const double theta) const {
	//analytical solution for ultra relativistic case
	/*return 8. * gamma * gamma * pcm2 * TMath::Tan(theta / 1000.) * 4.
	 / TMath::Power(TMath::Cos(2. * theta / 1000.) + 1., 2)
	 / TMath::Power(
	 1.
	 + gamma * gamma * TMath::Tan(theta / 1000.)
	 * TMath::Tan(theta / 1000.), 2);*/

	//numerical derivate calculation (should be accurate enough with relatively easy implementation
	//instead of analytical solution via mathematica etc)
	//via f (x) ≈ [f(x + h) − f(x − h)] / 2h
	//important!: pick h appropriately
	double e_m = 1.0 * 1e-16; // machine precision
	double h = pow(e_m, 0.33) * theta;

	return TMath::Abs(
			(getMomentumTransferFromTheta(theta + h)
					- getMomentumTransferFromTheta(theta - h)) / (2 * h));
}

double PndLmdDPMAngModel1D::eval(const double *x) const {
	double t = getMomentumTransferFromTheta(x[0]);
	double jaco = getThetaMomentumTransferJacobian(x[0]);
	return PndLmdDPMMTModel1D::eval(&t) * jaco;
}

void PndLmdDPMAngModel1D::updateDomain() {
	setDomain(0, TMath::Pi());
}
