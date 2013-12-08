/*
 * PndLmdLumiFitBase.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#ifndef PNDLMDLUMIHELPER_H_
#define PNDLMDLUMIHELPER_H_

#ifndef __CINT__
#include "ModelFitResult.h"
#include "ModelStructs.h"
#include "Chi2Estimator.h"
#endif /* __CINT __ */

#include <vector>
#include <map>

#ifdef HAS_SHAREDPOINTER
using std::shared_ptr;
#else
using std::tr1::shared_ptr;
#endif


#include "TString.h"
#include "TGraphErrors.h"
#include "TDatabasePDG.h"

class PndLmdAcceptance;
class PndLmdData;
class PndLmdDataInterface;
class PndLmdResolution;
class PndLmdLumiFitOptions;

class TFile;
class TVector3;
class TH1D;
class TH2D;


/**
 * \brief This class contains a few functions helping the user with some basics.
 *
 * Main use of this class will be the actual interface to the data created by the lmd macros for filling #PndLmdDataBase objects.
 */
class PndLmdLumiHelper {
	/*TODO:
	 - save the histograms read in from the panda data simulation in a separate root file so
	 that we don't have to read in the data every time we change only the fit type...
	 - inform: how to make the things faster... threads via std::threads or boost::threads
	 and then really how to speed up things on a cluster via openmpi??*/
public:
	enum acceptance_type {
		FULL, GEOMETRY_ONLY, NO_BACKTRACKING
	};
	enum data_type {
		DATA, ACCEPTANCE, RESOLUTION
	};

	struct lmd_graph: public TObject {
		TGraphErrors* graph;
		std::vector<std::pair<double, ModelStructs::minimization_parameter> > data;

		ModelFitResult* fit_result;
		const PndLmdLumiFitOptions* fit_options;

		std::map<unsigned int, std::pair<std::string, std::string> > parameter_name_stack;

		std::map<std::string, double, ModelStructs::string_comp> remaining_dependencies;
		TString dependency;

	ClassDef(lmd_graph, 1)
		;
	};

private:
	TDatabasePDG *pdg;
	std::vector<PndLmdData*> registered_data;
	std::vector<PndLmdAcceptance*> registered_acceptances;
	std::vector<PndLmdResolution*> registered_resolutions;

	void clearRegisters(data_type data_mode);

	void removeFinished(std::vector<PndLmdDataInterface*> *lmd_vec,
			int num_events);
	int getNextMinEventIndex(std::vector<PndLmdDataInterface*> *lmd_vec);

	void finalizeLmdGraphObjects(
			std::map<
					double,
					std::map<std::pair<std::string, std::string>
							, PndLmdLumiHelper::lmd_graph* , ModelStructs::stringpair_comp> > &graph_map
			, int mode);

public:
	PndLmdLumiHelper();

	static double getMomentumTransferFromTheta(double plab, double theta);

	int registerData(PndLmdData* data);
	int registerData(std::vector<PndLmdData*> &data_vec);
	int registerAcceptance(PndLmdAcceptance* acc);
	int registerAcceptances(std::vector<PndLmdAcceptance*> &acc_vec);
	int registerResolution(PndLmdResolution* res);
	int registerResolutions(std::vector<PndLmdResolution*> &res_vec);

	void fillData(double plab, TString dir_path, data_type data_mode,
			acceptance_type acc_mode = FULL, TString MC_file = "Lumi_MC_*.root",
			TString geane_file = "Lumi_Geane_*.root", TString track_file =
					"Lumi_Track_*.root");

	double calcHistIntegral(TH1D* hist, std::pair<double, double> range);

	std::vector<PndLmdData*> getDataFromFile(TFile *f);
	std::vector<PndLmdAcceptance*> getAcceptanceFromFile(TFile *f);

#ifndef __CINT__
	void removePointsAboveChi2(
			std::map<std::string, TGraphErrors*, ModelStructs::string_comp> &graph_map
			, double chi2);

	void fitResolutionForSlice(PndLmdResolution* lmd_resolution,
			const PndLmdLumiFitOptions *fit_options);

	void fitParametrizationModelToData(PndLmdLumiHelper::lmd_graph *graph);

	std::map<double, ModelFitResult*> checkFitParameters(
			const std::map<PndLmdResolution*, ModelFitResult*> &fit_results) const;

	int initResolutionParametrizationFromFile(TFile *f,
			shared_ptr<Model> resolution_model);
#endif /* __CINT __ */

	std::vector<PndLmdLumiHelper::lmd_graph*> filterLmdGraphs(
			std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs, TString dep);

	std::vector<PndLmdResolution*> getFittedResolutionsFromPath(TFile *f);
	std::vector<PndLmdLumiHelper::lmd_graph*> getResolutionModelResultsFromFile(
			TFile* f);

	std::vector<PndLmdLumiHelper::lmd_graph*> generateNewLmdGraphs(
			std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs);

	std::vector<PndLmdLumiHelper::lmd_graph*> generateLmdGraphs(
			std::vector<PndLmdResolution*> resolutions);

	void saveLmdGraphsToFile(
			std::vector<PndLmdLumiHelper::lmd_graph*>& graph_vec);

	void fitParametrizationModelToGraphs(
			std::vector<PndLmdLumiHelper::lmd_graph*> &lmd_graphs);

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
			std::vector<PndLmdResolution*> &lmd_resolutions,
			const PndLmdLumiFitOptions *fit_options);
}
;

#endif /* PNDLMDLUMIHELPER_H_ */
