/*
 * PndLmdFitFacade.h
 *
 *  Created on: Aug 30, 2013
 *      Author: steve
 */

#ifndef PNDLMDFITFACADE_H_
#define PNDLMDFITFACADE_H_

#include "ROOTDataHelper.h"
#include "PndLmdModelFactory.h"
#include "LumiFitStructs.h"
#include "ModelFitFacade.h"
#include "PndLmdLumiFitOptions.h"

#include <vector>

class PndLmdData;
class PndLmdResolution;
class PndLmdAbstractData;

class PndLmdFitFacade {
	private:
		// ROOT data helper class
		ROOTDataHelper data_helper;

		PndLmdLumiFitOptions fit_options_template;

		ModelFitFacade model_fit_facade;

		PndLmdModelFactory model_factory;

		std::pair<double, double> calcActualRange(
				const PndLmdAbstractData &lmd_data,
				const PndLmdLumiFitOptions *fit_options);

		shared_ptr<Data> createData1D(const PndLmdData &lmd_data) const;

	public:
		PndLmdFitFacade();
		virtual ~PndLmdFitFacade();

		PndLmdLumiFitOptions& getFitOptionTemplate();

		void setModelFitOptions(LumiFit::PndLmdFitModelOptions& model_opt_);
		void setEstimatorOptions(EstimatorOptions& est_opt_);
		void addFreeFitParameter(std::string& name);

		ModelFitFacade& getModelFitFacade();

		std::pair<double, double> calcRange(const PndLmdAbstractData &lmd_data,
				const PndLmdLumiFitOptions *fit_options);

		PndLmdLumiFitOptions* cloneFitOptions() const;

		PndLmdLumiFitOptions* createFitOptions(PndLmdData &lmd_data);

		void initializeResolutionModelOptionsFromParametrizationFile(
				LumiFit::PndLmdFitModelOptions& model_opt, std::string res_file_url);

		void doLmdLumiFits(std::vector<PndLmdData> &lmd_data_vec);

		/**
		 * This function determines and saves the parameters of the
		 * resolution/smearing function that is later on used for the luminosity fit.
		 *
		 * The idea is to cut down the dimensionality of the problem by using the
		 * parametrization models. So models that describe the evolution of
		 * parameters of the parent model.
		 *
		 * More precisely:
		 * Fit each of these resolution objects with the base smearing model, which
		 * returns fit results for the parameters of this model. Then for all of the
		 * parameters of the smearing model exist values for the dimensionality of
		 * the problem (for the lmd case: plab, theta and phi).
		 *
		 * Next for fixed values of all dimensions variables, except one, the model
		 * parameters are parametrized with so called parametrization models.
		 * These have their own parameters again, which are then parametrized as a
		 * function of the next dimension variable, and so on...
		 *
		 * So in the case for lmd:
		 * gauss(gauss_params(phi; phi_params(theta; theta_params(plab; plab_params))))
		 *
		 * I think this is the best (probably only) way to handle this complex
		 * parametrization. Finally these parameters are save to a file, that can be
		 * read in later for actual fits with the smearing model.
		 *
		 * TODO is if there is a way to generalize this so that
		 * this procedure can be applied to any kind of model that exists
		 * (assuming this model has a structure as described above).
		 * maybe generate the parametrization model code from a string
		 */
		void fitSmearingModelToResolutions(
				vector<PndLmdResolution> &lmd_resolutions);

		PndLmdLumiFitResult* doFit(PndLmdAbstractData &lmd_abs_data,
				const PndLmdLumiFitOptions *fit_options);

		void fitVertexData(std::vector<PndLmdData*> lmd_data);

		void fitMultipleElasticPPbar(std::vector<PndLmdData> &lmd_data);

		void fitResolutionForSlice(PndLmdResolution &lmd_resolution,
				const PndLmdLumiFitOptions *fit_options);
};

#endif /* PNDLMDFITFACADE_H_ */
