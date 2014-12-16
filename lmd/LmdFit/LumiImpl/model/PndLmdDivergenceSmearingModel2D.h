/*
 * PndLmdDivergenceSmearingModel2D.h
 *
 *  Created on: Nov 21, 2014
 *      Author: steve
 */

#ifndef PNDLMDDIVERGENCESMEARINGMODEL2D_H_
#define PNDLMDDIVERGENCESMEARINGMODEL2D_H_

#include <core/Model2D.h>

#include "LumiFitStructs.h"

class TVector3;

class PndLmdDivergenceSmearingModel2D: public Model2D {
	shared_ptr<Model2D> smearing_model;
	shared_ptr<Model2D> model;

	LumiFit::LmdDimension dim_x;
	LumiFit::LmdDimension dim_y;
	double** value_grid;

	unsigned int smear_grid_bins_x;
	unsigned int smear_grid_bins_y;

	std::vector<double> calculateThetaFromTiltedSystem(const double theta,
			const double phi, const TVector3 &tilt,
			const TVector3 &rotate_axis) const;

	double calculateJacobianDeterminant(const double theta, const double phi,
			const TVector3 &tilt, const TVector3 &rotate_axis) const;

	void generateSmearedGrid2D();

public:
	PndLmdDivergenceSmearingModel2D(std::string name_, shared_ptr<Model2D> model_,
			shared_ptr<Model2D> smearing_model_,
			const LumiFit::LmdDimension& data_dim_x_,
			const LumiFit::LmdDimension& data_dim_y_);
	virtual ~PndLmdDivergenceSmearingModel2D();

	void initModelParameters();

	double eval(const double *x) const;

	void updateDomain();
};

#endif /* PNDLMDDIVERGENCESMEARINGMODEL2D_H_ */
