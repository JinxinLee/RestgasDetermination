/*
 * PndLmdResultPlotter.h
 *
 *  Created on: Mar 25, 2013
 *      Author: steve
 */

#ifndef PNDLMDRESULTPLOTTER_H_
#define PNDLMDRESULTPLOTTER_H_

#include "PndLmdLumiHelper.h"
#include "fit/PndLmdLumiFitOptions.h"
#include "data/PndLmdVertexData.h"
#ifndef __CINT__
#include "visualization/ROOT/ROOTPlotter.h"
#endif /* __CINT __ */

#include <map>

#include "TString.h"

class PndLmdAngularData;
class PndLmdAcceptance;
class PndLmdResolution;
class PndLmdLumiFitResult;

class TH1D;
class TH2D;
class TGraph;
class TGraphErrors;
class TGraphAsymmErrors;
class TCanvas;

class PndLmdResultPlotter {
public:
	struct combined_values {
		double xval;
		double lumival;
		double lumierr;
		double lumiref;
	};

	struct graph_bundle {
		TH1D *hist1d;
		TH2D *hist2d;
		TGraphAsymmErrors *data_graph_1D;

		TGraphAsymmErrors *model;
		std::pair<TGraphAsymmErrors*, TGraphAsymmErrors*> residual; //1d case only

		bool is_residual;

		EstimatorOptions est_options;

		std::vector<std::pair<TString, int> > labels;

		bool draw_labels;
		bool log_scale;

		std::vector<double> x_parallel_lines;
		std::vector<double> y_parallel_lines;

		graph_bundle() :
				hist1d(0), hist2d(0), data_graph_1D(0), model(0), is_residual(false), draw_labels(
						true), log_scale(false) {

		}
	};

	struct fit_options_compare {
		bool operator()(const PndLmdLumiFitOptions& lhs,
				const PndLmdLumiFitOptions& rhs) const {
			return (lhs.getEstimatorOptions() < rhs.getEstimatorOptions());
		}
	};

	static bool mc_dimension_compare(const PndLmdVertexData &vertex_data) {
		return (vertex_data.getPrimaryDimension().dimension_options.track_type
				== LumiFit::MC);
	}

	static bool reco_dimension_compare(const PndLmdVertexData &vertex_data) {
		return (vertex_data.getPrimaryDimension().dimension_options.track_type
				== LumiFit::RECO);
	}

private:
	PndLmdLumiHelper lumi_helper;
#ifndef __CINT__
	ROOTPlotter root_plotter;
#endif /* __CINT __ */
	int text_font;

	bool use_plot_range;
	double theta_plot_range_low;
	double theta_plot_range_high;

	double text_leftpos;
	double text_toppos;
	double text_spacing;
	double text_size;
	double text_size2;

	double label_offset_x;
	double label_offset_y;
	double title_offset_x;
	double title_offset_y;

	std::pair<double, double> determinePlotRange(
			std::map<TString, std::vector<combined_values> > &result_map);
	std::pair<double, double> determineDiffPlotRange(
			std::map<TString, std::vector<combined_values> > &result_map);

	TGraphErrors* makeGraph(std::vector<combined_values> data, int color);

	double calculateYPos(double line, double text_toppos, double text_spacing,
			bool log);

	TGraphAsymmErrors* createGraphFromFitResult(
			const PndLmdLumiFitOptions &fit_opt, PndLmdAngularData &data);

	DataStructs::DimensionRange generatePlotRange(
			const PndLmdAbstractData &lmd_abs_data,
			const PndLmdLumiFitOptions &fit_options) const;

	std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> > generateLmdGraphMap(
			std::vector<PndLmdLumiHelper::lmd_graph*> graphs);

public:
	PndLmdResultPlotter();
	virtual ~PndLmdResultPlotter();

	void setThetaPlotRange(double theta_plot_range_low_,
			double theta_plot_range_high_);
	void setTextLeftPos(double text_leftpos_);
	void setTextTopPos(double text_toppos_);
	void setTextSpacing(double text_spacing_);
	void setTextSize(double text_size_);
	void setLabelSize(double text_size2_);

	void setLabelOffsetX(double label_offset_x_);
	void setLabelOffsetY(double label_offset_y_);
	void setTitleOffsetX(double title_offset_x_);
	void setTitleOffsetY(double title_offset_y_);

	TGraphAsymmErrors* makeDifferenceGraph(TGraphAsymmErrors *g1,
			TGraphAsymmErrors *g2);

	TGraphAsymmErrors* makeRatioGraph(TGraphAsymmErrors *g1,
			TGraphAsymmErrors *g2);

	TGraphAsymmErrors* createVertexGraphFromFitResult(
			const PndLmdLumiFitOptions &fit_opt, PndLmdVertexData &data);

	TGraphAsymmErrors* createSmearingGraphFromFitResult(
			const PndLmdLumiFitOptions &fit_opt, PndLmdResolution &res_data);

	std::pair<TGraphAsymmErrors*, TGraphAsymmErrors*> createResidual(
			const PndLmdLumiFitOptions &fit_opt, PndLmdAngularData &data);

	TGraphAsymmErrors* createAcceptanceGraph(PndLmdAcceptance *acc);

	void drawMultipleGraphsOnCanvas(
			std::vector<std::pair<TGraphErrors*, TString> >& graphs);

	std::map<PndLmdLumiFitOptions,
			std::map<int, PndLmdResultPlotter::graph_bundle>,
			PndLmdResultPlotter::fit_options_compare> makeGraphBundles1D(
			std::vector<PndLmdAngularData> &data_vec);

	std::map<int, PndLmdResultPlotter::graph_bundle> makeVertexGraphBundles1D(
			std::vector<PndLmdVertexData> &data_vec);

	PndLmdResultPlotter::graph_bundle makeAcceptanceBundle(
			PndLmdAngularData& data, const PndLmdLumiFitOptions &fit_options);

	PndLmdResultPlotter::graph_bundle makeGraphBundle1D(PndLmdAngularData& data,
			const PndLmdLumiFitOptions &fit_options);

	PndLmdResultPlotter::graph_bundle makeResolutionGraphBundle1D(
			PndLmdResolution& res);

	PndLmdResultPlotter::graph_bundle makeVertexGraphBundle1D(
			PndLmdVertexData& data);

	std::pair<TGraphErrors*, TGraphErrors*> makeIPParameterDependencyGraphBundle(
			std::vector<PndLmdVertexData> &vertex_data,
			const LumiFit::LmdSimIPParameters &dependency, TString ytitle_subscript);

	std::vector<PndLmdResultPlotter::graph_bundle> createGraphDifferenceBundle(
			std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundle,
			std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundle_ref);

	std::vector<PndLmdResultPlotter::graph_bundle> createGraphRatioBundle(
			std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundle,
			std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundle_ref);

	std::pair<double, double> calulateLumiRelDiff(double lumi, double lumi_err,
			double lumi_ref);

	void fillSinglePad(TCanvas *c, graph_bundle &graph_bundle, bool use_2d = false);

	combined_values createCombinedValue(double x, double lumi, double err,
			double ref);

	void fillOverviewCanvas(TCanvas *c,
			std::map<int, PndLmdResultPlotter::graph_bundle> &graph_bundles);

	void makeFitResultBooky(
			std::map<PndLmdLumiFitOptions,
					std::map<int, PndLmdResultPlotter::graph_bundle>,
					PndLmdResultPlotter::fit_options_compare> &graph_bundle_map,
			TString filename = "fitresults");

	void makeVertexFitResultBooky(
			std::vector<std::map<int, PndLmdResultPlotter::graph_bundle> > &graph_bundles,
			TString filename = "ip-fit_results");

	void makeResolutionSummaryPlots(TFile *f);

	void makeResolutionBooky(std::vector<PndLmdResolution> &res_vec,
			TString filename);

	void makeResolutionDifferencesBooky(std::vector<PndLmdResolution> &res_vec,
			std::vector<PndLmdResolution> &res_vec_ref);

	void makeVertexDifferencesBooky(std::vector<PndLmdVertexData> &res_vec,
			std::vector<PndLmdVertexData> &res_vec_ref);

	void makeBooky(std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundles,
			TString filename, int x = 4, int y = 3);

	void makeComparisonCanvas(TString name,
			std::map<TString, std::vector<PndLmdResultPlotter::combined_values> >& result_map);

	TGraphAsymmErrors* generateDPMModelPartGraph(double plab,
			LumiFit::DPMElasticParts dpm_elastic_part,
			DataStructs::DimensionRange& plot_range);

	void plotXYOverviewGraph(std::vector<PndLmdVertexData> &vertex_data);

	TGraphAsymmErrors* makeXYOverviewGraph(
			std::vector<PndLmdVertexData> &vertex_data, double error_scaling_factor,
			int color);

	void plotIPDependencyGraphs(
			std::map<LumiFit::LmdSimIPParameters,
					std::map<LumiFit::LmdSimIPParameters,
							std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > > &ip_data_clusters);
};

#endif /* PNDLMDRESULTPLOTTER_H_ */
