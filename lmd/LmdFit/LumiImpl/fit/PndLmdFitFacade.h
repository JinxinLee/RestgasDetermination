/*
 * PndLmdFitFacade.h
 *
 *  Created on: Aug 30, 2013
 *      Author: steve
 */

#ifndef PNDLMDFITFACADE_H_
#define PNDLMDFITFACADE_H_

#include "fit/data/ROOT/ROOTDataHelper.h"
#include "model/PndLmdModelFactory.h"
#include "fit/ModelFitFacade.h"
#include "LumiFitStructs.h"
#include "fit/PndLmdLumiFitOptions.h"
#include "PndLmdLumiHelper.h"
#include "data/PndLmdAcceptance.h"

#include <vector>

class PndLmdAbstractData;
class PndLmdAngularData;
class PndLmdVertexData;
class PndLmdHistogramData;

class PndLmdFitFacade {
private:
	// ROOT data helper class
	ROOTDataHelper data_helper;

	PndLmdLumiFitOptions fit_options_template;

	ModelFitFacade model_fit_facade;

	PndLmdModelFactory model_factory;

	std::set<PndLmdAcceptance> acceptance_pool;
	std::set<PndLmdAcceptance> reference_acceptance_pool;
	/*std::set<PndLmdLumiHelper::lmd_graph*> resolution_graph_pool;
	 std::set<PndLmdLumiHelper::lmd_graph*> referenc_resolution_graph_pool;*/

	std::map<PndLmdAngularData, PndLmdAngularData> fitted_angular_data;

	shared_ptr<Data> createData1D(const PndLmdHistogramData &lmd_hist_data) const;
	shared_ptr<Data> createData2D(const PndLmdHistogramData &lmd_hist_data) const;

public:
	struct findBySelectionDimensionsIgnoringSecondaryTrackFilters {
		findBySelectionDimensionsIgnoringSecondaryTrackFilters(
				const std::set<LumiFit::LmdDimension> &selection_dimensions_) {
			selection_dimensions = removeSecondaryTrackFilter(selection_dimensions_);
		}
		bool operator()(const PndLmdLumiHelper::lmd_graph* &lmd_res_graph) {
			std::set<LumiFit::LmdDimension> filtered_selection_dimensions =
					removeSecondaryTrackFilter(lmd_res_graph->remaining_selections);
			return filtered_selection_dimensions == selection_dimensions;
		}
		bool operator()(const PndLmdAcceptance &lmd_acc) {
			std::set<LumiFit::LmdDimension> filtered_selection_dimensions =
					removeSecondaryTrackFilter(lmd_acc.getSelectorSet());
			std::cout << "checking if " << filtered_selection_dimensions.size()
					<< " and " << selection_dimensions.size() << " match" << std::endl;
			return filtered_selection_dimensions == selection_dimensions;
		}
		static std::set<LumiFit::LmdDimension> removeSecondaryTrackFilter(
				std::set<LumiFit::LmdDimension> selection_dimensions_) {
			std::set<LumiFit::LmdDimension>::iterator it =
					selection_dimensions_.begin();
			while (it != selection_dimensions_.end()) {
				if (it->dimension_options.dimension_type == LumiFit::SECONDARY) {
					selection_dimensions_.erase(it);
					it = selection_dimensions_.begin();
				} else
					++it;
			}
			return selection_dimensions_;
		}

	private:
		std::set<LumiFit::LmdDimension> selection_dimensions;
	};

	PndLmdFitFacade();
	virtual ~PndLmdFitFacade();

	PndLmdLumiFitOptions& getFitOptionTemplate();

	void setModelFitOptions(LumiFit::PndLmdFitModelOptions& model_opt_);
	void setEstimatorOptions(EstimatorOptions& est_opt_);
	void addFreeFitParameter(std::string name);

	ModelFitFacade& getModelFitFacade();

	void addAcceptencesToPool(const std::vector<PndLmdAcceptance> &lmd_acc);
	void addReferenceAcceptancesToPool(
			const std::vector<PndLmdAcceptance> &lmd_acc);
	/*void addResolutionGraphsToPool(
	 const std::vector<PndLmdLumiHelper::lmd_graph*> &lmd_res_graphs);
	 void addReferenceResolutionGraphsToPool(
	 const std::vector<PndLmdLumiHelper::lmd_graph*> &lmd_res_graphs);*/

	void clearPools();

	void saveFittedObjectsToFile();

	std::vector<DataStructs::DimensionRange> calcRange(
			const PndLmdAbstractData &lmd_abs_data,
			const EstimatorOptions &est_options);

	std::vector<PndLmdLumiFitOptions> createFitOptions(
			PndLmdHistogramData &lmd_data);

	void initializeResolutionModelOptionsFromParametrizationFile(
			LumiFit::PndLmdFitModelOptions& model_opt, std::string res_file_url);

	void doLmdLumiFits(std::vector<PndLmdAngularData> &lmd_data_vec);

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
			vector<PndLmdHistogramData> &lmd_resolutions);

	PndLmdLumiFitResult doFit(PndLmdHistogramData &lmd_hist_data,
			const PndLmdLumiFitOptions &fit_options);

	void fitVertexData(std::vector<PndLmdVertexData> &lmd_data);

	void fitMultipleElasticPPbar(PndLmdAngularData &lmd_data,
			const std::vector<PndLmdLumiFitOptions> &fit_options_vec);

	void fit1DResolutionForSlice(PndLmdHistogramData &lmd_resolution,
			const PndLmdLumiFitOptions &fit_options);

	void fit2DResolutionForSlice(PndLmdHistogramData &lmd_resolution,
			const PndLmdLumiFitOptions &fit_options);
};

#endif /* PNDLMDFITFACADE_H_ */
