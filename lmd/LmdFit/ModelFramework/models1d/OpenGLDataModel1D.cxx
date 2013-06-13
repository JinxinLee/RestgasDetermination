/*
 * OpenGLDataModel1D.cxx
 *
 *  Created on: Jan 17, 2013
 *      Author: steve
 */

#include "OpenGLDataModel1D.h"

#include <GL/gl.h>
#include <GL/glu.h>

OpenGLDataModel1D::OpenGLDataModel1D(std::string name_, unsigned int nsize_,
		double **data, interpolation_type type) :
		Model1D(name_) {
	nsize = nsize_;
	// create ctrl point array
	for (int i = 0; i < nsize; i++) {
		ctrlpoints[i] = new GLfloat[3];
		// fill with points
		ctrlpoints[i][0] = data[i][0];
		ctrlpoints[i][1] = data[i][1];
		ctrlpoints[i][2] = data[i][2];
	}

	intpol_type = type;

	if (intpol_type == NONE) {

	} else if (intpol_type == LINEAR) {

	} else {
		glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]); // cubic spline
	}
	glEnable(GL_MAP1_VERTEX_3);
}

OpenGLDataModel1D::~OpenGLDataModel1D() {
	// TODO Auto-generated destructor stub
}

void OpenGLDataModel1D::initModelParameters() {
}

double OpenGLDataModel1D::eval(const double *x) const {
	// so here what we need to do is first find out the closest control points to the current position
	/*glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 1, &ctrlpoints[index of first closest point...][0]);// linear "spline"
	// then use these points to build the spline of a given order (probably need to make it multimodel1d again)
	// and finally evaluate this spline at the given u=[0-1] value that would correspond to the current position...
	// that should be it...
	glEvalCoord2f((float) i / 100.0, (float) j / 100.0);*/
}

void OpenGLDataModel1D::updateDomain() {
}
