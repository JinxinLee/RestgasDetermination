/*
 * Model1D.cxx
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#include "Model1D.h"
#include "IntegralStrategyGSL1D.h"

Model1D::Model1D(std::string name_) :
		Model(name_, 1) {
	integral_strategy = new IntegralStrategyGSL1D();
}

Model1D::~Model1D() {
	// TODO Auto-generated destructor stub
}

double Model1D::Integral(std::vector<std::pair<double, double> > &ranges
		, double precision) {
	return integral_strategy->Integral(this, ranges[0].first, ranges[0].second,
			precision);
}

double Model1D::getDomainRange() {
	return domain_bounds.second - domain_bounds.first;
}

double Model1D::getDomainLowerBound() {
	return domain_bounds.first;
}

const std::pair<double, double>& Model1D::getDomain() const {
	return domain_bounds;
}

void Model1D::setDomain(double lower_bound, double upper_bound) {
	domain_bounds.first = lower_bound;
	domain_bounds.second = upper_bound;
}
