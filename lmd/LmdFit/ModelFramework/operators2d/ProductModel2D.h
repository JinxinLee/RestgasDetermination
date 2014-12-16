/*
 * ProductModel2D.h
 *
 *  Created on: Jan 10, 2013
 *      Author: steve
 */

#ifndef PRODUCTMODEL2D_H_
#define PRODUCTMODEL2D_H_

#include "core/Model2D.h"

class ProductModel2D: public Model2D {
private:
	shared_ptr<Model2D> first, second;
public:
	ProductModel2D(std::string name_, shared_ptr<Model2D> first_,
			shared_ptr<Model2D> second_);

	void initModelParameters();

	double eval(const double *x) const;

	void updateDomain();

	virtual std::pair<double, double> getUncertaincy(const double *x) const;
};

#endif /* PRODUCTMODEL2D_H_ */
