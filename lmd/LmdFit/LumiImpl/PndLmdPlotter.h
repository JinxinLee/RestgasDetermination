#ifndef PNDLMDPLOTTER_H_
#define PNDLMDPLOTTER_H_

#include "ROOTPlotHelper.hpp"
#include "PndLmdLumiHelper.h"
#include "fit/PndLmdLumiFitOptions.h"
#include "PndLmdComparisonStructs.h"

#ifndef __CINT__
#include "visualization/ROOT/ROOTPlotter.h"
#include "visualization/ModelVisualizationProperties1D.h"
#endif /* __CINT __ */

#include <map>

#include "TString.h"

class PndLmdAngularData;
class PndLmdAcceptance;
class PndLmdVertexData;
class PndLmdLumiFitResult;

class TCanvas;

namespace LumiFit {

class PndLmdPlotter {
public:
	DataStructs::DimensionRange primary_dimension_plot_range;
	DataStructs::DimensionRange secondary_dimension_plot_range;

	struct LumiValues {
		NeatPlotting::GraphPoint position;
		double lumival;
		double lumierr;
		double lumiref;

		LumiValues(double x, double y, double lumival_, double lumierr_,
				double lumiref_) :
				lumival(lumival_), lumierr(lumierr_), lumiref(lumiref_) {
			position.x = x;
			position.y = y;
		}
	};

private:
	PndLmdLumiHelper lumi_helper;
	NeatPlotting::GraphAndHistogramHelper neat_plot_helper;

#ifndef __CINT__
	ROOTPlotter root_plotter;
#endif /* __CINT __ */

	/*std::pair<double, double> determinePlotRange(
	 std::map<TString, std::vector<LumiValues> > &result_map);
	 std::pair<double, double> determineDiffPlotRange(
	 std::map<TString, std::vector<combined_values> > &result_map);*/

	double calculateYPos(double line, double text_toppos, double text_spacing,
			bool log);

	std::vector<DataStructs::DimensionRange> generatePlotRange(
			const PndLmdAbstractData &lmd_abs_data,
			const EstimatorOptions &est_options) const;

	std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> > generateLmdGraphMap(
			std::vector<PndLmdLumiHelper::lmd_graph*> graphs);

	void applyPlotRanges(NeatPlotting::PlotBundle &plot_bundle) const;

#ifndef __CINT__
	std::pair<ModelVisualizationProperties1D, ModelVisualizationProperties1D> create2DVisualizationProperties(
			const EstimatorOptions &est_options,
			const PndLmdAbstractData &lmd_abs_data) const;

	ModelVisualizationProperties1D create1DVisualizationProperties(
			const EstimatorOptions &est_options,
			const PndLmdAbstractData &lmd_abs_data) const;
#endif /* __CINT __ */

public:
	PndLmdPlotter();
	virtual ~PndLmdPlotter();

	std::pair<double, double> calulateLumiRelDiff(double lumi, double lumi_err,
			double lumi_ref) const;

	TGraphAsymmErrors* createGraphFromFitResult(
			const PndLmdLumiFitOptions &fit_opt, PndLmdAngularData &data) const;

	// create graphs or histograms representing the model
	TH2D* create2DHistogramFromFitResult(const PndLmdLumiFitOptions &fit_opt,
			const PndLmdAngularData &data) const;

	TGraphAsymmErrors* createVertexGraphFromFitResult(
			const PndLmdLumiFitOptions &fit_opt, const PndLmdVertexData &data) const;

	TH2D* create2DHistogramFromFitResult(const PndLmdLumiFitOptions &fit_opt,
			const PndLmdHistogramData &res_data) const;

	TGraphAsymmErrors* createSmearingGraphFromFitResult(
			const PndLmdLumiFitOptions &fit_opt,
			const PndLmdHistogramData &res_data) const;

	/*std::pair<TGraphAsymmErrors*, TGraphAsymmErrors*> createResidual(
	 const PndLmdLumiFitOptions &fit_opt, PndLmdAngularData &data);*/

	TGraphAsymmErrors* createAcceptanceGraph(const PndLmdAcceptance *acc) const;

	TGraphAsymmErrors* generateDPMModelPartGraph(double plab,
			LumiFit::DPMElasticParts dpm_elastic_part,
			DataStructs::DimensionRange& plot_range);

	// the plot bundle creation functions ===========================================

	NeatPlotting::PlotBundle makeAcceptanceBundle1D(PndLmdAngularData& data,
			const PndLmdLumiFitOptions &fit_options) const;

	NeatPlotting::PlotBundle makeAcceptanceBundle2D(PndLmdAngularData& data,
			const PndLmdLumiFitOptions &fit_options) const;

	NeatPlotting::PlotBundle makeGraphBundle1D(PndLmdAngularData& data,
			const PndLmdLumiFitOptions &fit_options, bool draw_model =true);

	NeatPlotting::PlotBundle makeResidualPlotBundle1D(PndLmdAngularData& data,
			const PndLmdLumiFitOptions &fit_options);

	NeatPlotting::PlotBundle makeResolutionGraphBundle(
			const PndLmdHistogramData& res) const;

	/*NeatPlotting::PlotBundle2D makeResolutionGraphBundle2D(
	 PndLmdHistogramData& res);*/

	NeatPlotting::PlotBundle makeVertexGraphBundle1D(
			const PndLmdVertexData& data) const;

	NeatPlotting::PlotBundle makeIPXYOverviewGraphBundle(
			const std::vector<PndLmdVertexData> &vertex_data) const;

	NeatPlotting::PlotBundle makeXYOverviewHistogram(
			const std::map<LumiFit::LmdSimIPParameters, PndLmdAngularData> &sim_settings_ip_data_map) const;

	// the plot bundles creation functions ===========================================

	/*
	 std::pair<TGraphErrors*, TGraphErrors*> makeIPParameterDependencyGraphBundle(
	 std::vector<PndLmdVertexData> &vertex_data,
	 const LumiFit::LmdSimIPParameters &dependency, TString ytitle_subscript);


	 std::map<PndLmdLumiFitOptions, NeatPlotting::PlotBundle1D> makeGraphBundles1D(
	 std::vector<PndLmdAngularData> &data_vec,
	 LumiFit::PndLmdFitModelOptions &fitop_tmctruth);

	 std::map<PndLmdLumiFitOptions,
	 std::map<int, NeatPlotting::PlotBundle1D>,
	 LumiFit::Comparisons::fit_options_compare> makeGraphBundles1D(
	 std::vector<PndLmdAngularData> &data_vec);
	 */

	// systematics helper functions ===========================================
	NeatPlotting::PlotBundle createLowerFitRangeDependencyPlotBundle(
			const std::vector<PndLmdAngularData> &data_vec,
			LumiFit::PndLmdFitModelOptions model_opt) const;

	std::map<PndLmdHistogramData, std::vector<PndLmdAngularData> > clusterIntoGroups(
			const std::vector<PndLmdAngularData> &data_vec) const;

	TGraphAsymmErrors* makeXYOverviewGraph(
			const std::vector<PndLmdVertexData> &data_vec,
			double error_scaling_factor) const;

	TGraphAsymmErrors* createLowerFitRangeDependencyGraph(
			const std::vector<PndLmdAngularData> &prefiltered_data,
			LumiFit::PndLmdFitModelOptions model_opt) const;

	NeatPlotting::SystematicsAnalyser::SystematicDependencyGraphBundle createLowerFitRangeDependencyGraphBundle(
			const std::vector<PndLmdAngularData> &prefiltered_data,
			LumiFit::PndLmdFitModelOptions model_opt) const;

	// booky routines =======================================================

#ifndef __CINT__
	NeatPlotting::Booky makeLumiFitResultOverviewBooky(
			std::vector<PndLmdAngularData> &data_vec);

	void createAcceptanceComparisonBooky(
			std::vector<std::pair<PndLmdAcceptance, PndLmdAcceptance> > &acc_matches);

	NeatPlotting::Booky makeVertexFitResultBooky(
			const std::vector<PndLmdVertexData> &data_vec);

	void makeVertexDifferencesBooky(std::vector<PndLmdVertexData> &res_vec,
			std::vector<PndLmdVertexData> &res_vec_ref);

	NeatPlotting::Booky makeResolutionFitResultBooky(
			const std::vector<PndLmdHistogramData> &data_vec, int x = 5,
			int y = 4) const;

	NeatPlotting::PlotBundle createResolutionParameterizationPlot(
			PndLmdLumiHelper::lmd_graph* graph);

	NeatPlotting::Booky createResolutionParameterizationOverviewPlot(
			std::vector<PndLmdLumiHelper::lmd_graph*> &graph_vec);

#endif /* __CINT __ */

	NeatPlotting::PlotBundle createLumiFit2DPlotBundle(TH2D* hist) const;

	NeatPlotting::Booky create2DFitResultPlots(
			const std::vector<PndLmdAngularData> &data_vec) const;

	/*	 std::map<LumiFit::PndLmdFitModelOptions,
	 std::map<std::set<LumiFit::LmdDimension>,
	 std::vector<PndLmdLumiHelper::lmd_graph*> > > sortLmdGraphs(
	 const std::vector<PndLmdLumiHelper::lmd_graph*> &graphs);

	 void createResolutionParameterizationDepedencyOverviewPlots(
	 std::map<std::set<LumiFit::LmdDimension>,
	 std::vector<PndLmdLumiHelper::lmd_graph*> > &data_selection_sorted_graphs,
	 TString dependency_name);

	 void createResolutionParameterizationComparisonPlots(
	 std::vector<PndLmdLumiHelper::lmd_graph*> &graphs,
	 std::vector<PndLmdLumiHelper::lmd_graph*> &reference);

	 void makeFitResultBooky(
	 std::map<PndLmdLumiFitOptions,
	 std::map<int, NeatPlotting::PlotBundle1D>,
	 LumiFit::Comparisons::fit_options_compare> &graph_bundle_map,
	 TString filename = "fitresults");


	 void makeResolutionSummaryPlots(const std::string &file_url);

	 void makeResolutionBooky(std::vector<PndLmdHistogramData> &res_vec,
	 TString filename);

	 void makeResolutionDifferencesBooky(std::vector<PndLmdHistogramData> &res_vec,
	 std::vector<PndLmdHistogramData> &res_vec_ref);

	 void makeComparisonCanvas(TString name,
	 std::map<TString, std::vector<NeatPlotting::GraphPoint> >& result_map);


	 void plotIPDependencyGraphs(
	 std::map<LumiFit::LmdSimIPParameters,
	 std::map<LumiFit::LmdSimIPParameters,
	 std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > > &ip_data_clusters);
	 */
};
}

#endif /* PNDLMDPLOTTER_H_ */
