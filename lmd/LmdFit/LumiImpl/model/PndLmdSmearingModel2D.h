/*
 * PndLmdSmearingModel2D.h
 *
 *  Created on: Nov 21, 2014
 *      Author: steve
 */

#ifndef PNDLMDSMEARINGMODEL2D_H_
#define PNDLMDSMEARINGMODEL2D_H_

#include "PndLmdLumiHelper.h"

#include <map>

class PndLmdSmearingModel2D {
	std::map<PndLmdLumiHelper::bin_dimension, double> empty_contribution_map;
	double search_distance_x;
	double search_distance_y;

	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> > smearing_parameterization;

	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> >::const_iterator findNearestNeighbour(
			const double *x) const;

	void determineSearchDistance();

public:
	PndLmdSmearingModel2D();
	virtual ~PndLmdSmearingModel2D();

	void setSmearingParameterization(std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> > smearing_parameterization_);

	const std::map<PndLmdLumiHelper::bin_dimension, double>& getListOfContributors(
			const double *x) const;

	virtual void updateSmearingModel();
};

#endif /* PNDLMDSMEARINGMODEL2D_H_ */
