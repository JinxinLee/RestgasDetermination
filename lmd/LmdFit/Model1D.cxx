/*
 * Model1D.cxx
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#include "Model1D.h"

#include <cmath>

#include <gsl/gsl_integration.h>

#include <iostream>

Model1D::Model1D() :
    Model(1) {
  // TODO Auto-generated constructor stub
}

Model1D::~Model1D() {
  // TODO Auto-generated destructor stub
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

Model1D* Model1D::current_model = 0;

double Model1D::GSL_Integral(double xlow, double xhigh, double precision) {
  current_model = this;
  double result, error;
  size_t neval;
  gsl_function F;
  F.function = &gsl_func_wrapper;
  F.params = 0;
  if(gsl_integration_qng(&F, xlow, xhigh, precision, precision,
      &result, &error, &neval))
    result = Integral(xlow, xhigh, precision);
  return result;
}

double Model1D::Integral(double xlow, double xhigh, double precision) {
  // ok increase the number of divisions until precision is reached
  unsigned int divisions = 0;
  double current_precision = 0.0;
  double val = 0.0;
  double last_value = 0.0;
  do {
    divisions++;
    val = 0.0;
    double x[3];
    double division_width = (xhigh - xlow) / divisions;
    for (unsigned int i = 0; i < divisions; i++) {
      x[0] = xlow + division_width * (1.0 * i);
      x[1] = xlow + division_width * (1.0 * i + 0.5);
      x[2] = xlow + division_width * (1.0 * i + 1.0);

      // simpsons formula
      val += evaluate(&x[0]) + 4.0 * evaluate(&x[1]) + evaluate(&x[2]);
      //std::cout << evaluate(&x[0]) << " " << evaluate(&x[1]) << " "
      //    << evaluate(&x[2]) << std::endl;
    }
    val *= division_width / 6.0;
    if (val == 0.0)
      break;
    //std::cout << "integral = " << val << std::endl;
    current_precision = fabs((val - last_value) / val);
    last_value = val;
    //std::cout<<"using "<<divisions<<" divisions: "<<current_precision << " < " << precision <<std::endl;
  } while (current_precision > precision/1e1);
  //std::cout << "xrange: "<<xlow<<" - "<<xhigh<<"  returning integral = " << val << std::endl;
  return val;
}
