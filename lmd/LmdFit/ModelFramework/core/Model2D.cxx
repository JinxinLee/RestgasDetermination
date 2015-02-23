/*
 * Model2D.cxx
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#include "Model2D.h"

#include "operators2d/integration/IntegralStrategyGSL2D.h"

Model2D::Model2D(std::string name_) :
		Model(name_, 2), var1_domain_bounds(), var2_domain_bounds(), integral_strategy(
				new IntegralStrategyGSL2D()) {
}

Model2D::~Model2D() {
	// TODO Auto-generated destructor stub
}

double Model2D::getVar1DomainRange() {
	return var1_domain_bounds.second - var1_domain_bounds.first;
}
double Model2D::getVar2DomainRange() {
	return var2_domain_bounds.second - var2_domain_bounds.first;
}

double Model2D::getVar1DomainLowerBound() {
	return var1_domain_bounds.first;
}
double Model2D::getVar2DomainLowerBound() {
	return var2_domain_bounds.first;
}
void Model2D::setVar1Domain(double lower_bound, double upper_bound) {
	var1_domain_bounds.first = lower_bound;
	var1_domain_bounds.second = upper_bound;
}
void Model2D::setVar2Domain(double lower_bound, double upper_bound) {
	var2_domain_bounds.first = lower_bound;
	var2_domain_bounds.second = upper_bound;
}

void Model2D::setIntegralStrategy(
		shared_ptr<IntegralStrategy2D> integral_strategy_) {
	integral_strategy = integral_strategy_;
}

double Model2D::Integral(std::vector<DataStructs::DimensionRange> &ranges,
		double precision) {
	return integral_strategy->Integral(this, ranges[0].range_low,
			ranges[0].range_high, ranges[1].range_low, ranges[1].range_high,
			precision);
}
