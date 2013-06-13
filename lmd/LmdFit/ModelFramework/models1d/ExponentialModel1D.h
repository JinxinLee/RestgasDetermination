/*
 * ExponentialModel1D.h
 *
 *  Created on: Apr 5, 2013
 *      Author: steve
 */

#ifndef EXPONENTIALMODEL1D_H_
#define EXPONENTIALMODEL1D_H_

#include "Model1D.h"

class ExponentialModel1D: public Model1D {
  private:
    shared_ptr<ModelPar> amplitude;
    shared_ptr<ModelPar> exp_factor;

  public:
    ExponentialModel1D(std::string name_);
    virtual ~ExponentialModel1D();

    double eval(const double *x) const;

    void initModelParameters();

    void updateDomain();
};

#endif /* EXPONENTIALMODEL1D_H_ */
