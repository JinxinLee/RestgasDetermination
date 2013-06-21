/*
 * PndLmdModelFactory.h
 *
 *  Created on: Dec 18, 2012
 *      Author: steve
 */

#ifndef PNDLMDMODELFACTORY_H_
#define PNDLMDMODELFACTORY_H_

#include "Model1D.h"
#include "Model2D.h"

class PndLmdLumiFitOptions;
class PndLmdAcceptance;
class PndLmdLumiFitResult;

/**
 * Class for creating Models. User is supposed to only use this factory to create
 * his models.
 * An inherited form of this class should basically handle the options of the model
 * via the #generateModel() function, which has to be implemented
 */
class PndLmdModelFactory {
public:
	PndLmdModelFactory();
	~PndLmdModelFactory();

	shared_ptr<Model1D> generate1DResolutionModel(const
			PndLmdLumiFitOptions *fit_options);

	/**
	 * 1D Model generator method
	 * @param fit_options are the options which model will be built and returned
	 */
	shared_ptr<Model1D> generate1DModel(const PndLmdLumiFitOptions *fit_options,
			double plab, const PndLmdAcceptance *acceptance = 0);

	/**
	 * 2D Model generator method
	 * @param fit_options are the options which model will be built and returned
	 */
	/*Model2D& generate2DModel(PndLmdLumiFitOptions *fit_options);*/
};

#endif /* PNDLMDMODELFACTORY_H_ */
