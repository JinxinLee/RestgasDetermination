/*
 * ProductModel2D.cxx
 *
 *  Created on: Jan 10, 2013
 *      Author: steve
 */

#include "ProductModel2D.h"

#include <iostream>

ProductModel2D::ProductModel2D(std::string name_, shared_ptr<Model2D> first_,
		shared_ptr<Model2D> second_) :
		Model2D(name_) {
	first = first_;
	second = second_;

	addModelToList(first);
	addModelToList(second);
}

void ProductModel2D::initModelParameters() {

}

double ProductModel2D::eval(const double *x) const {
	return multiply(first, second, x);
}

std::pair<double, double> ProductModel2D::getUncertaincy(
		const double *x) const {
	return std::make_pair(
			first->getUncertaincy(x).first * second->eval(x)
					+ second->getUncertaincy(x).first * first->eval(x),
			first->getUncertaincy(x).second * second->eval(x)
					+ second->getUncertaincy(x).second * first->eval(x));
}

void ProductModel2D::updateDomain() {
	// first we need to check if user defined a domain for his models
	if (first->getVar1DomainRange() == 0 || second->getVar1DomainRange() == 0
			|| first->getVar2DomainRange() == 0
			|| second->getVar2DomainRange() == 0) {
		std::cout
				<< "Warning: Some of the models used for the multiplication have not"
						" defined any domains!" << std::endl;
	} else {
		setVar1Domain(
				std::max(first->getVar1DomainLowerBound(),
						second->getVar1DomainLowerBound()),
				std::min(first->getVar1DomainLowerBound() + first->getVar1DomainRange(),
						second->getVar1DomainLowerBound() + second->getVar1DomainRange()));
		setVar2Domain(
				std::max(first->getVar2DomainLowerBound(),
						second->getVar2DomainLowerBound()),
				std::min(first->getVar2DomainLowerBound() + first->getVar2DomainRange(),
						second->getVar2DomainLowerBound() + second->getVar2DomainRange()));
	}
}
