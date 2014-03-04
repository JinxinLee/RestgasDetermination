/*
 * PndLmdROOTDataModel1D.h
 *
 *  Created on: Jan 20, 2013
 *      Author: steve
 */

#ifndef PNDLMDROOTDATAMODEL1D_H_
#define PNDLMDROOTDATAMODEL1D_H_

#include "Model1D.h"
#include "LumiFitStructs.h"

class TEfficiency;
class TGraphAsymmErrors;
class TSpline3;

class PndLmdROOTDataModel1D: public Model1D {
private:
	LumiFit::LmdDimensionRange data_dimension;
	TGraphAsymmErrors *graph;
	TSpline3 *spline;
	double acc_range_low, acc_range_high;
	bool using_acceptance_bounds;

	void updateDomainFromPars(double *par);

	void determineAcceptanceBounds();

	// function pointer used to switch between different algorithms for interpolation
	typedef double (PndLmdROOTDataModel1D::*function)(const double *x) const;

	function model_func;

public:
	enum interpolation_type {
		CONSTANT, LINEAR, SPLINE
	} intpol_type;

	PndLmdROOTDataModel1D(std::string name_);
	virtual ~PndLmdROOTDataModel1D();

	LumiFit::LmdDimensionRange getDataDimension() const;
	void setDataDimension(LumiFit::LmdDimensionRange data_dimension_);

	TGraphAsymmErrors *getGraph() const;
	interpolation_type getIntpolType() const;
	void setGraph(TGraphAsymmErrors *graph_);
	void setIntpolType(interpolation_type intpol_type_);

	void initModelParameters();

	virtual std::pair<double, double> getUncertaincy(const double *x) const;

	double eval(const double *x) const;

	std::pair<double, double> getAcceptanceBounds() const;

	double evaluateConstant(const double *x) const;
	double evaluateLinear(const double *x) const;
	double evaluateSpline(const double *x) const;

	void updateDomain();
};

#endif /* PNDLMDROOTDATAMODEL1D_H_ */
