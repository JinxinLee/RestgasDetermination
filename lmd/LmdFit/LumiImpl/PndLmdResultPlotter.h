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
#ifndef __CINT__
#include "ROOTPlotter.h"
#endif /* __CINT __ */
#include <map>

#include "TString.h"

class PndLmdData;
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
				TGraphAsymmErrors *model;
				TH1D *hist1d;
				TH2D *hist2d;
				std::pair<TGraphAsymmErrors*, TGraphAsymmErrors*> residual; //1d case only
				TGraphAsymmErrors *acceptance_1d;
				double plab;
				PndLmdLumiFitOptions fit_options;
				bool is_resolution;
				bool is_acceptance;
				bool is_residual;
				std::vector<std::pair<TString, int> > labels;
		};

		struct fit_options_compare {
				bool operator()(const PndLmdLumiFitOptions& lhs,
						const PndLmdLumiFitOptions& rhs) const {
					return lhs.lessThanNonBinaryOptions(rhs);
				}
		};

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
				const PndLmdLumiFitOptions &fit_opt, PndLmdData &data);

		DataStructs::dimension_range generatePlotRange(
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

		TGraphAsymmErrors* createSmearingGraphFromFitResult(
				const PndLmdLumiFitOptions &fit_opt, PndLmdResolution &res_data);

		std::pair<TGraphAsymmErrors*, TGraphAsymmErrors*> createResidual(
				const PndLmdLumiFitOptions &fit_opt, PndLmdData &data);

		TGraphAsymmErrors* createAcceptanceGraph(PndLmdAcceptance *acc);

		std::map<PndLmdLumiFitOptions,
				std::map<int, PndLmdResultPlotter::graph_bundle>,
				PndLmdResultPlotter::fit_options_compare> makeGraphBundles1D(
				std::vector<PndLmdData> &data_vec);

		PndLmdResultPlotter::graph_bundle makeAcceptanceBundle(PndLmdData& data,
				const PndLmdLumiFitOptions &fit_options);

		PndLmdResultPlotter::graph_bundle makeGraphBundle1D(PndLmdData& data,
				const PndLmdLumiFitOptions &fit_options);

		PndLmdResultPlotter::graph_bundle makeResolutionGraphBundle1D(PndLmdResolution& res);

		std::pair<double, double> calulateLumiRelDiff(double lumi, double lumi_err,
				double lumi_ref);

		void fillAcceptanceInPad(graph_bundle acceptance_bundle);

		void fill2DAcceptanceInPad(graph_bundle acceptance_bundle);

		void fillSinglePad(TCanvas *c, graph_bundle &graph_bundle, bool log_scale,
				bool labels_on = true);

		combined_values createCombinedValue(double x, double lumi, double err,
				double ref);

		void fillOverviewCanvas(TCanvas *c,
				std::map<int, PndLmdResultPlotter::graph_bundle> &graph_bundles);

		void makeFitResultBooky(
				std::map<PndLmdLumiFitOptions,
						std::map<int, PndLmdResultPlotter::graph_bundle>,
						PndLmdResultPlotter::fit_options_compare> &graph_bundle_map
				, TString filename = "fitresults");

		void makeResolutionSummaryPlots(TFile *f);

		void makeResolutionBooky(std::vector<PndLmdResolution> &res_vec,
				TString filename);

		void makeBooky(
				std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundles,
				TString filename, int x = 4, int y = 3);

		void makeComparisonCanvas(
				TString name,
				std::map<TString, std::vector<PndLmdResultPlotter::combined_values> >& result_map);

		void plotDPMModelParts(double plab, std::pair<double, double> plot_range
				, bool log_scale = true);
};

#endif /* PNDLMDRESULTPLOTTER_H_ */
