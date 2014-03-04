/*
 * PndLmdSmearingAsymmetricGaussianModelParameterization1D.h
 *
 *  Created on: Jan 22, 2013
 *      Author: steve
 */

#ifndef PNDLMDSMEARINGASYMMETRICGAUSSIANMODELPARAMETERIZATION1D_H_
#define PNDLMDSMEARINGASYMMETRICGAUSSIANMODELPARAMETERIZATION1D_H_

#include "SharedPtr.h"

class Model1D;
class ModelPar;
class PndLmdLumiFitOptions;

class PndLmdSmearingAsymmetricGaussianModelParameterization1D {
	private:
		shared_ptr<ModelPar> p_lab;
	public:
		PndLmdSmearingAsymmetricGaussianModelParameterization1D(
				shared_ptr<Model1D> model, const PndLmdLumiFitOptions *fit_options);
		virtual ~PndLmdSmearingAsymmetricGaussianModelParameterization1D();
};

#endif /* PNDLMDSMEARINGASYMMETRICGAUSSIANMODELPARAMETERIZATION1D_H_ */
