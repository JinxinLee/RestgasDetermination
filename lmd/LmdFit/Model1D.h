/*
 * Model1D.h
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#ifndef MODEL1D_H_
#define MODEL1D_H_

#include "Model.h"

class IntegralStrategy1D;

class Model1D: public Model {
private:
  std::pair<double, double> domain_bounds;
  IntegralStrategy1D *integral_strategy;

public:
  Model1D();
  virtual ~Model1D();

  double Integral(double xlow, double xhigh, double precision);

  /**
   * Returns a vector of value pairs representing the domain of the model. The
   * first value of each pair is the lower, the second the upper bound of the
   * domain. Each entry in the vector represents one dimension.
   */
  const std::pair<double, double>& getDomain() const;

  /**
   * Sets the domain of the model. The domain is defined to be non-zero within
   * the domain and zero outside. Hence this setter should be used with care.
   * @param lower_bound is the lower bound of the domain
   * @param upper_bound is the upper bound of the domain
   */
  void setDomain(double lower_bound, double upper_bound);

  double getDomainRange();
  double getDomainLowerBound();
};

#endif /* MODEL1D_H_ */
