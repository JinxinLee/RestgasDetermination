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

	struct graph_bundle_1D {
		TGraphErrors *model;
		TH1D* data_hist;
		TGraphErrors *residual;
		double plab;
		const PndLmdLumiFitOptions* fit_options;
		bool is_resolution;
		std::vector<std::pair<TString, int> > labels;
	};

	struct acceptance_bundle {
		TGraphAsymmErrors *acceptance_1d;
		TH2D *acceptance_2d;
		double plab;
		bool is_angular;
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

	TGraphErrors* createGraphFromFitResult(PndLmdLumiFitResult *fit_res,
			PndLmdAcceptance *acc, PndLmdData *data);

	std::pair<double, double> calculatePlotRange(PndLmdDataInterface *data,
			const PndLmdLumiFitOptions *fit_options);

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

	std::vector<PndLmdData*> getDataFromPath(TString path);

	TGraphErrors* createSmearingGraphFromFitResult(PndLmdLumiFitResult *fit_res,
			PndLmdResolution *res_data);

	std::map<PndLmdLumiFitOptions,
			std::vector<PndLmdResultPlotter::graph_bundle_1D>,
			PndLmdResultPlotter::fit_options_compare> makeGraphBundles1D(
			PndLmdData *data, PndLmdAcceptance* acc);

	std::vector<PndLmdResultPlotter::graph_bundle_1D> makeResolutionGraphBundles1D(
			std::vector<PndLmdResolution*> &res_vec);

	acceptance_bundle makeAcceptanceBundle(PndLmdAcceptance* acc,
			bool is_momentum_transfer = false);

	std::pair<double, double> calulateLumiRelDiff(double lumi, double lumi_err,
			double lumi_ref);

	void fillAcceptanceInPad(acceptance_bundle acceptance_bundle);

	void fill2DAcceptanceInPad(acceptance_bundle acceptance_bundle);

	void fillSinglePad(TCanvas *c, graph_bundle_1D graph_bundle, bool residual, bool log_scale,
			bool labels_on = true);

	combined_values createCombinedValue(double x, double lumi, double err,
			double ref);

	std::map<std::string, std::vector<PndLmdResolution*>,
			ModelStructs::string_comp> createBookyMap(
			std::vector<PndLmdResolution*> &res_vec);

	void fillOverviewCanvas(TCanvas *c,
			std::vector<PndLmdResultPlotter::graph_bundle_1D> &graph_bundles,
			acceptance_bundle &acceptance_bundle);

	void makeFitResultBooky(
			std::map<PndLmdLumiFitOptions,
					std::vector<PndLmdResultPlotter::graph_bundle_1D>,
					PndLmdResultPlotter::fit_options_compare> &graph_bundle_map
			, acceptance_bundle &acc_bundle , TString filename="fitresults");

	void makeResolutionSummaryPlots(TFile *f);

	void makeResolutionBooky(std::vector<PndLmdResolution*> &res_vec,
			TString filename);

	void makeComparisonCanvas(
			TString name,
			std::map<TString, std::vector<PndLmdResultPlotter::combined_values> >& result_map);

	void plotDPMModelParts(double plab, std::pair<double, double> plot_range
			, bool log_scale = true);
};

#endif /* PNDLMDRESULTPLOTTER_H_ */
