/*
 * PndLmdSmearingGaussianModelParametrization1D.h
 *
 *  Created on: Jan 22, 2013
 *      Author: steve
 */

#ifndef PNDLMDSMEARINGGAUSSIANMODELPARAMETRIZATION1D_H_
#define PNDLMDSMEARINGGAUSSIANMODELPARAMETRIZATION1D_H_

#include "Parametrization.h"

class PndLmdSmearingGaussianModelParametrization1D: public Parametrization {
private:
  shared_ptr<ModelPar> p_lab;
  shared_ptr<ModelPar> gauss_sigma;
  shared_ptr<ModelPar> gauss_mean;

public:
  PndLmdSmearingGaussianModelParametrization1D(ModelParSet &model_par_set_);
  virtual ~PndLmdSmearingGaussianModelParametrization1D();

  void initParameters();

  void parametrize();
};

#endif /* PNDLMDSMEARINGGAUSSIANMODELPARAMETRIZATION1D_H_ */
