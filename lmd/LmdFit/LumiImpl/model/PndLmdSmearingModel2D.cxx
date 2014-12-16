/*
 * PndLmdSmearingModel2D.cxx
 *
 *  Created on: Nov 21, 2014
 *      Author: steve
 */

#include <model/PndLmdSmearingModel2D.h>

#include <cmath>

PndLmdSmearingModel2D::PndLmdSmearingModel2D() {
}

PndLmdSmearingModel2D::~PndLmdSmearingModel2D() {
}

std::map<std::pair<double, double>,
		std::map<PndLmdLumiHelper::bin_dimension, double> >::const_iterator PndLmdSmearingModel2D::findNearestNeighbour(
		const double *x) const {
	double current_distance_x(0.0);
	double current_distance_y(0.0);
	bool first(true);

	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> >::const_iterator return_it =
			smearing_parameterization.end();

	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> >::const_iterator reco_element_it;
	for (reco_element_it = smearing_parameterization.begin();
			reco_element_it != smearing_parameterization.end(); reco_element_it++) {
		current_distance_x = fabs(reco_element_it->first.first - x[0]);
		current_distance_y = fabs(reco_element_it->first.second - x[1]);
		if (current_distance_x < search_distance_x
				&& current_distance_y < search_distance_y) {
			return_it = reco_element_it;
			break;
		}
	}

	return return_it;
}

void PndLmdSmearingModel2D::determineSearchDistance() {
	double min_dist_x(-1.0);
	double min_dist_y(-1.0);
	double binsize_x(-1.0);
	double binsize_y(-1.0);
	double current_distance_x;
	double current_distance_y;
	bool first(true);

	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> >::const_iterator smear_element_it;

	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> >::const_iterator first_smear_element_it =
			smearing_parameterization.begin();

	if (smearing_parameterization.size() > 0) {
		for (smear_element_it = (++smearing_parameterization.begin());
				smear_element_it != smearing_parameterization.end();
				smear_element_it++) {
			current_distance_x = fabs(
					smear_element_it->first.first - first_smear_element_it->first.first);
			current_distance_y = fabs(
					smear_element_it->first.second
							- first_smear_element_it->first.second);

			if (first) {
				min_dist_x = current_distance_x;
				min_dist_y = current_distance_y;
				if (current_distance_x > 0)
					binsize_x = current_distance_x;
				if (current_distance_y > 0)
					binsize_y = current_distance_y;
				first = false;
			} else {
				if (current_distance_x > 0
						&& (current_distance_x < min_dist_x || binsize_x < 0))
					binsize_x = current_distance_x;
				if (current_distance_x < min_dist_x) {
					min_dist_x = current_distance_x;
				}
				if (current_distance_y > 0
						&& (current_distance_y < min_dist_y || binsize_y < 0))
					binsize_y = current_distance_y;
				if (current_distance_y < min_dist_y) {
					min_dist_y = current_distance_y;
				}
			}
		}
	}

	search_distance_x = min_dist_x + binsize_x / 2;
	search_distance_y = min_dist_y + binsize_y / 2;

	std::cout << "determined search distance as: " << search_distance_x << " and "
			<< search_distance_y << std::endl;
}

void PndLmdSmearingModel2D::setSmearingParameterization(
		std::map<std::pair<double, double>,
				std::map<PndLmdLumiHelper::bin_dimension, double> > smearing_parameterization_) {
	smearing_parameterization = smearing_parameterization_;
	determineSearchDistance();
}

const std::map<PndLmdLumiHelper::bin_dimension, double>& PndLmdSmearingModel2D::getListOfContributors(
		const double *x) const {

	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> >::const_iterator smearing_iter =
			findNearestNeighbour(x);

	if (smearing_iter != smearing_parameterization.end())
		return smearing_iter->second;
	else
		return empty_contribution_map;
}

void PndLmdSmearingModel2D::updateSmearingModel() {

}
