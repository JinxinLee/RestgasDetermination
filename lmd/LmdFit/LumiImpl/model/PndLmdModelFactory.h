/*
 * PndLmdModelFactory.h
 *
 *  Created on: Dec 18, 2012
 *      Author: steve
 */

#ifndef PNDLMDMODELFACTORY_H_
#define PNDLMDMODELFACTORY_H_

#include "core/Model1D.h"
#include "core/Model2D.h"

namespace LumiFit {
class PndLmdFitModelOptions;
}
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

	shared_ptr<Model1D> generate1DVertexModel(
			const LumiFit::PndLmdFitModelOptions& model_options) const;

	shared_ptr<Model1D> generate1DResolutionModel(
			const LumiFit::PndLmdFitModelOptions& model_options) const;

	shared_ptr<Model2D> generate2DResolutionModel(
			const LumiFit::PndLmdFitModelOptions& model_options) const;

	shared_ptr<Model> generateModel(
			const LumiFit::PndLmdFitModelOptions& model_options, double plab) const;

	/**
	 * 1D Model generator method
	 * @param fit_options are the options which model will be built and returned
	 */
	shared_ptr<Model1D> generate1DModel(
			const LumiFit::PndLmdFitModelOptions& model_options, double plab) const;

	/**
	 * 2D Model generator method
	 * @param fit_options are the options which model will be built and returned
	 */
	shared_ptr<Model2D> generate2DModel(
			const LumiFit::PndLmdFitModelOptions& model_options, double plab) const;
};

#endif /* PNDLMDMODELFACTORY_H_ */
