/*
 * PndLmdLumiFitBase.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#ifndef PNDLMDLUMIHELPER_H_
#define PNDLMDLUMIHELPER_H_

#if !defined(__CINT__)
#include "ModelFitResult.h"
#include "ModelStructs.h"

#include "SharedPtr.h"

#endif /* __CINT __ */

#include "LumiFitStructs.h"

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

class Model;

class PndLmdAcceptance;
class PndLmdData;
class PndLmdAbstractData;
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
	public:
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
		void finalizeLmdGraphObjects(
				std::map<
						double,
						std::map<std::pair<std::string, std::string>
								, PndLmdLumiHelper::lmd_graph* , ModelStructs::stringpair_comp> > &graph_map
				, int mode);

	public:
		PndLmdLumiHelper();

		static double getMomentumTransferFromTheta(double plab, double theta);

		double calcHistIntegral(const TH1D* hist, std::pair<double, double> range);

#ifndef __CINT__
		void removePointsAboveChi2(
				std::map<std::string, TGraphErrors*, ModelStructs::string_comp> &graph_map
				, double chi2);

		void fitParametrizationModelToData(PndLmdLumiHelper::lmd_graph *graph);

		std::map<double, ModelFitResult*> checkFitParameters(
				const std::map<PndLmdResolution*, ModelFitResult*> &fit_results) const;

		int initResolutionParametrizationFromFile(TFile *f,
				shared_ptr<Model> resolution_model);
#endif /* __CINT __ */

		std::vector<PndLmdLumiHelper::lmd_graph*> filterLmdGraphs(
				std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs,
				TString dep);

		std::vector<PndLmdLumiHelper::lmd_graph*> getResolutionModelResultsFromFile(
				TFile* f);

		std::vector<PndLmdLumiHelper::lmd_graph*> generateNewLmdGraphs(
				std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs);

		std::vector<PndLmdLumiHelper::lmd_graph*> generateLmdGraphs(
				std::vector<PndLmdResolution>& resolutions,
				DataStructs::DimensionRange& fit_range);

		void saveLmdGraphsToFile(
				std::vector<PndLmdLumiHelper::lmd_graph*>& graph_vec);

		void fitParametrizationModelToGraphs(
				std::vector<PndLmdLumiHelper::lmd_graph*> &lmd_graphs);

}
;

#endif /* PNDLMDLUMIHELPER_H_ */
