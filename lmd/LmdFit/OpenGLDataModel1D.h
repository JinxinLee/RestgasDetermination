/*
 * OpenGLDataModel1D.h
 *
 *  Created on: Jan 17, 2013
 *      Author: steve
 */

#ifndef OPENGLDATAMODEL1D_H_
#define OPENGLDATAMODEL1D_H_

#include "Model1D.h"
#include <GL/gl.h>
#include <GL/glu.h>

class OpenGLDataModel1D: public Model1D {
private:
	// ok array that stores all of the control points (points from the acceptance data)
  unsigned int nsize;
  GLfloat **ctrlpoints; // 2d array with first index number of entries and second index the dimension (here also 3 but 1 is kept at 0)

	enum interpolation_type {
		NONE, LINEAR, SPLINE3
	} intpol_type;

public:
	OpenGLDataModel1D(unsigned int nsize_, double **data_,
	    interpolation_type type = LINEAR);
	virtual ~OpenGLDataModel1D();

	double evaluate(double *x, const double *par);
};

#endif /* OPENGLDATAMODEL1D_H_ */
