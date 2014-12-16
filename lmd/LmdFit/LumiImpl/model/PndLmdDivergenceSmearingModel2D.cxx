/*
 * PndLmdDivergenceSmearingModel2D.cxx
 *
 *  Created on: Nov 21, 2014
 *      Author: steve
 */

#include <model/PndLmdDivergenceSmearingModel2D.h>
#include <data/PndLmdAbstractData.h>
#include <cmath>

#include "TVector3.h"

PndLmdDivergenceSmearingModel2D::PndLmdDivergenceSmearingModel2D(
		std::string name_, shared_ptr<Model2D> model_,
		shared_ptr<Model2D> smearing_model_,
		const LumiFit::LmdDimension& data_dim_x_,
		const LumiFit::LmdDimension& data_dim_y_) :
		Model2D(name_), smearing_model(smearing_model_), model(model_), dim_x(
				data_dim_x_), dim_y(data_dim_y_) {

	addModelToList(model);
	addModelToList(smearing_model);

	smear_grid_bins_x = 50;
	smear_grid_bins_y = 50;

	value_grid = new double*[dim_x.bins];
	for (unsigned int ix = 0; ix < dim_x.bins; ix++) {
		value_grid[ix] = new double[dim_y.bins];
	}

}

PndLmdDivergenceSmearingModel2D::~PndLmdDivergenceSmearingModel2D() {
// TODO Auto-generated destructor stub
}

std::vector<double> PndLmdDivergenceSmearingModel2D::calculateThetaFromTiltedSystem(
		const double theta, const double phi, const TVector3 &tilt,
		const TVector3 &rotate_axis) const {
	std::vector<double> return_vec(2);
	TVector3 measured_direction(sin(theta) * cos(phi), sin(theta) * sin(phi),
			cos(theta));
	measured_direction.Rotate(tilt.Theta(), rotate_axis);
	return_vec[0] = measured_direction.Theta();
	return_vec[1] = measured_direction.Phi();
	return return_vec;
}

double PndLmdDivergenceSmearingModel2D::calculateJacobianDeterminant(
		const double theta, const double phi, const TVector3 &tilt,
		const TVector3 &rotate_axis) const {
	double e_m = 1.0 * 1e-16; // machine precision
	double ht = pow(e_m, 0.33) * theta;
	double hp = pow(e_m, 0.33) * phi;

	std::vector<double> shift_plus_ht = calculateThetaFromTiltedSystem(theta + ht,
			phi, tilt, rotate_axis);
	std::vector<double> shift_min_ht = calculateThetaFromTiltedSystem(theta - ht,
			phi, tilt, rotate_axis);
	std::vector<double> shift_plus_hp = calculateThetaFromTiltedSystem(theta,
			phi + hp, tilt, rotate_axis);
	std::vector<double> shift_min_hp = calculateThetaFromTiltedSystem(theta,
			phi - hp, tilt, rotate_axis);

	double j11 = (shift_plus_ht[0] - shift_min_ht[0]) / (2 * ht);
	double j12 = (shift_plus_hp[0] - shift_min_hp[0]) / (2 * hp);
	double j21 = (shift_plus_ht[1] - shift_min_ht[1]) / (2 * ht);
	double j22 = (shift_plus_hp[1] - shift_min_hp[1]) / (2 * hp);

	return j11 * j22 - j21 * j12;
}

void PndLmdDivergenceSmearingModel2D::generateSmearedGrid2D() {
	// loop over divergence grid
	double tilt_value_x;
	double tilt_value_y;
	double div_binsize_x = smearing_model->getVar1DomainRange()
			/ smear_grid_bins_x;
	double div_binsize_y = smearing_model->getVar2DomainRange()
			/ smear_grid_bins_y;
	std::vector<double> x;
	x.push_back(0.005);
	x.push_back(0.0);

	std::vector<DataStructs::DimensionRange> smear_model_integral_range;
	DataStructs::DimensionRange temp;
	smear_model_integral_range.push_back(temp);
	smear_model_integral_range.push_back(temp);

	for (unsigned int smear_ix = 0; smear_ix < smear_grid_bins_x; smear_ix++) {
		tilt_value_x = smearing_model->getVar1DomainLowerBound()
				+ div_binsize_x * (0.5 + smear_ix);
		std::cout << "asdfasdfsadfas" << std::endl;
		for (unsigned int smear_iy = 0; smear_iy < smear_grid_bins_y; smear_iy++) {
			tilt_value_y = smearing_model->getVar2DomainLowerBound()
					+ div_binsize_y * (0.5 + smear_iy);

			smear_model_integral_range[0].range_low = tilt_value_x
					- 0.5 * div_binsize_x;
			smear_model_integral_range[0].range_high = tilt_value_x
					+ 0.5 * div_binsize_x;
			smear_model_integral_range[1].range_low = tilt_value_y
					- 0.5 * div_binsize_y;
			smear_model_integral_range[1].range_high = tilt_value_y
					+ 0.5 * div_binsize_y;

			// integrate the smearing model (has to  be normalized overall)
			// over this bin in the divergence coordinates
			double smear_weight = smearing_model->Integral(smear_model_integral_range,
					1e-2);
			//double smear_weight = 0.1;

			// then calculate the angular distribution at each grid point for the model for this tilt
			TVector3 tilt(tilt_value_x, tilt_value_y, 1.0);
			TVector3 rotate_axis(tilt_value_y, -tilt_value_x, 0.0);

			double theta;
			double phi;
			for (unsigned int ix = 0; ix < dim_x.bins; ix++) {
				theta = dim_x.bin_size * (0.5 + ix);
				for (unsigned int iy = 0; iy < dim_y.bins; iy++) {
					phi = dim_y.bin_size * (0.5 + iy);

					x = calculateThetaFromTiltedSystem(theta, phi, tilt, rotate_axis);
					double jacobi_from_div_tilt = calculateJacobianDeterminant(theta, phi,
							tilt, rotate_axis);

					value_grid[ix][iy] = smear_weight * model->eval(&x[0])
							* jacobi_from_div_tilt;
				}
			}
		}
	}
}

void PndLmdDivergenceSmearingModel2D::initModelParameters() {
	generateSmearedGrid2D();
}

double PndLmdDivergenceSmearingModel2D::eval(const double *x) const {
	unsigned int binx = (x[0] - dim_x.dimension_range.getRangeLow())
			/ dim_x.dimension_range.getDimensionLength();
	unsigned int biny = (x[1] - dim_y.dimension_range.getRangeLow())
			/ dim_y.dimension_range.getDimensionLength();
	return value_grid[binx][biny];
}

void PndLmdDivergenceSmearingModel2D::updateDomain() {
	generateSmearedGrid2D();
}
