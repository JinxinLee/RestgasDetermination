/*
 * DoubleGaussianModel1D.h
 *
 *  Created on: Feb 19, 2013
 *      Author: steve
 */

#ifndef DOUBLEGAUSSIANMODEL1D_H_
#define DOUBLEGAUSSIANMODEL1D_H_

#include "core/Model1D.h"

class DoubleGaussianModel1D: public Model1D {
private:
	double num_sigmas;
	/*shared_ptr<ModelPar> gauss_mean_narrow;
	 shared_ptr<ModelPar> gauss_mean_wide;
	 shared_ptr<ModelPar> gauss_sigma_narrow;
	 shared_ptr<ModelPar> gauss_sigma_wide;
	 shared_ptr<ModelPar> gauss_ratio_narrow_wide;
	 shared_ptr<ModelPar> gauss_amplitude;*/

	shared_ptr<ModelPar> gauss_mean_narrow;
	shared_ptr<ModelPar> gauss_mean_wide;
	shared_ptr<ModelPar> gauss_sigma_narrow;
	shared_ptr<ModelPar> gauss_sigma_ratio_narrow_wide;
	shared_ptr<ModelPar> gauss_ratio_narrow_wide;
	shared_ptr<ModelPar> gauss_amplitude;
public:
	DoubleGaussianModel1D(std::string name_);
	virtual ~DoubleGaussianModel1D();

	void initModelParameters();

	double eval(const double *x) const;

	void updateDomain();
};

#endif /* DOUBLEGAUSSIANMODEL1D_H_ */
