/*
 * PndLmdROOTDataModel1D.h
 *
 *  Created on: Jan 20, 2013
 *      Author: steve
 */

#ifndef PNDLMDROOTDATAMODEL1D_H_
#define PNDLMDROOTDATAMODEL1D_H_

#include "Model1D.h"

class TEfficiency;
class TGraphAsymmErrors;

class PndLmdROOTDataModel1D: public Model1D {
  private:
    TEfficiency *acceptance;
    TGraphAsymmErrors *graph;
	double acc_range_low, acc_range_high;

    void updateDomainFromPars(double *par);

	void determineAcceptanceBounds();

	// function pointer used to switch between different algorithms for interpolation
	typedef double (PndLmdROOTDataModel1D::*function)(double *x) const;

	function model_func;

  public:
    enum interpolation_type {
      CONSTANT, LINEAR, SPLINE
    } intpol_type;

    PndLmdROOTDataModel1D(TEfficiency *acceptance_, interpolation_type type_);
    virtual ~PndLmdROOTDataModel1D();

    void initModelParameters();

    double eval(double *x) const;

    double evaluateConstant(double *x) const;
    double evaluateLinear(double *x) const;
    double evaluateSpline(double *x) const;

	void updateDomain();
};

#endif /* PNDLMDROOTDATAMODEL1D_H_ */
