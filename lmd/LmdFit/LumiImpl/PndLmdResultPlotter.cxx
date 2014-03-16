/*
 * PndLmdResultPlotter.cxx
 *
 *  Created on: Mar 25, 2013
 *      Author: steve
 */

#include "PndLmdResultPlotter.h"

#include "PndLmdModelFactory.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdLumiFitResult.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"
#include "PndLmdResolution.h"
#include "ROOTDataHelper.h"
#include "PndLmdFitFacade.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iosfwd>
#include <ostream>

#include "TMath.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TLatex.h"
#include "TString.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TLegend.h"
#include "TEfficiency.h"
#include "TExec.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TColor.h"
#include "TPaletteAxis.h"

using std::stringstream;
using std::endl;

PndLmdResultPlotter::PndLmdResultPlotter() {
	text_font = 132;
	use_plot_range = false;
	theta_plot_range_low = 0.0;
	theta_plot_range_high = 0.0;
	text_leftpos = 0.20;
	text_toppos = 0.98;
	text_spacing = 0.08;
	text_size = 0.05;
	text_size2 = 0.05;
	label_offset_x = 0.007;
	label_offset_y = 0.007;
	title_offset_x = 1.0;
	title_offset_y = 1.5;

	gStyle->SetPadBorderMode(0);
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);
	gStyle->SetPadTopMargin(0.06);
	gStyle->SetPadBottomMargin(0.12);
	gStyle->SetPadLeftMargin(0.14);
	gStyle->SetPadRightMargin(0.1);
	gStyle->SetTextFont(text_font);
	TGaxis::SetMaxDigits(3);
}

PndLmdResultPlotter::~PndLmdResultPlotter() {
	// TODO Auto-generated destructor stub
}

void PndLmdResultPlotter::setThetaPlotRange(double theta_plot_range_low_,
		double theta_plot_range_high_) {
	use_plot_range = true;
	theta_plot_range_low = theta_plot_range_low_;
	theta_plot_range_high = theta_plot_range_high_;
}
void PndLmdResultPlotter::setTextLeftPos(double text_leftpos_) {
	text_leftpos = text_leftpos_;
}
void PndLmdResultPlotter::setTextTopPos(double text_toppos_) {
	text_toppos = text_toppos_;
}
void PndLmdResultPlotter::setTextSpacing(double text_spacing_) {
	text_spacing = text_spacing_;
}
void PndLmdResultPlotter::setTextSize(double text_size_) {
	text_size = text_size_;
}
void PndLmdResultPlotter::setLabelSize(double text_size2_) {
	text_size2 = text_size2_;
}

void PndLmdResultPlotter::setLabelOffsetX(double label_offset_x_) {
	label_offset_x = label_offset_x_;
}
void PndLmdResultPlotter::setLabelOffsetY(double label_offset_y_) {
	label_offset_y = label_offset_y_;
}
void PndLmdResultPlotter::setTitleOffsetX(double title_offset_x_) {
	title_offset_x = title_offset_x_;
}
void PndLmdResultPlotter::setTitleOffsetY(double title_offset_y_) {
	title_offset_y = title_offset_y_;
}

PndLmdResultPlotter::combined_values PndLmdResultPlotter::createCombinedValue(
		double x, double lumi, double err, double ref) {
	combined_values cv;
	cv.xval = x;
	cv.lumival = lumi;
	cv.lumierr = err;
	cv.lumiref = ref;
	return cv;
}

TGraphAsymmErrors* PndLmdResultPlotter::makeDifferenceGraph(
		TGraphAsymmErrors *g1, TGraphAsymmErrors *g2) {
	// this can only work when the graphs are equal in binning

	if (g1->GetN() == g2->GetN()) {
		TGraphAsymmErrors *gdiff = new TGraphAsymmErrors(g1->GetN());
		for (unsigned int i = 0; i < g1->GetN(); i++) {
			double x1;
			double y1;
			g1->GetPoint(i, x1, y1);
			double x1errup = g1->GetErrorXhigh(i);
			double x1errlow = g1->GetErrorXlow(i);
			double y1errup = g1->GetErrorYhigh(i);
			double y1errlow = g1->GetErrorYlow(i);

			for (unsigned int j = 0; j < g2->GetN(); j++) {
				double x2;
				double y2;
				g2->GetPoint(j, x2, y2);

				if (x1 == x2) {
					double x2errup = g2->GetErrorXhigh(i);
					double x2errlow = g2->GetErrorXlow(i);
					double y2errup = g2->GetErrorYhigh(i);
					double y2errlow = g2->GetErrorYlow(i);

					gdiff->SetPoint(i, x1, y1 - y2);
					gdiff->SetPointError(i, (x1errlow + x2errlow) / 2,
							(x1errup + x2errup) / 2, y1errlow + y2errlow, y1errup + y2errup);

					break;
				}
			}
		}
		return gdiff;
	}
	return 0;
}

TGraphAsymmErrors* PndLmdResultPlotter::makeRatioGraph(TGraphAsymmErrors *g1,
		TGraphAsymmErrors *g2) {
	// this can only work when the graphs are equal in binning

	if (g1->GetN() == g2->GetN()) {
		TGraphAsymmErrors *gdiff = new TGraphAsymmErrors(g1->GetN());
		for (unsigned int i = 0; i < g1->GetN(); i++) {
			double x1;
			double y1;
			g1->GetPoint(i, x1, y1);
			double x1errup = g1->GetErrorXhigh(i);
			double x1errlow = g1->GetErrorXlow(i);
			double y1errup = g1->GetErrorYhigh(i);
			double y1errlow = g1->GetErrorYlow(i);

			for (unsigned int j = 0; j < g2->GetN(); j++) {
				double x2;
				double y2;
				g2->GetPoint(j, x2, y2);

				if (x1 == x2) {
					double x2errup = g2->GetErrorXhigh(i);
					double x2errlow = g2->GetErrorXlow(i);
					double y2errup = g2->GetErrorYhigh(i);
					double y2errlow = g2->GetErrorYlow(i);

					if (y2 == 0.0) {
						gdiff->SetPoint(i, x1, 0.0);
						gdiff->SetPointError(i, 0.0, 0.0, 0.0, 0.0);
					} else {
						gdiff->SetPoint(i, x1, y1 / y2);

						gdiff->SetPointError(i, (x1errlow + x2errlow) / 2,
								(x1errup + x2errup) / 2,
								sqrt(
										pow(y1errlow / y2, 2.0)
												+ pow(y2errlow * y1 / y2 / y2, 2.0)),
								sqrt(
										pow(y1errup / y2, 2.0) + pow(y2errup * y1 / y2 / y2, 2.0)));

					}
					break;
				}
			}
		}
		return gdiff;
	}
	return 0;
}

std::pair<double, double> PndLmdResultPlotter::determinePlotRange(
		std::map<TString, std::vector<combined_values> > &result_map) {

	std::pair<double, double> range_pair;

	std::vector<double> y;
	for (std::map<TString, std::vector<combined_values> >::iterator it =
			result_map.begin(); it != result_map.end(); it++) {
		for (unsigned int i = 0; i < it->second.size(); i++) {
			y.push_back(it->second[i].lumival + it->second[i].lumierr);
		}
	}
	std::sort(y.begin(), y.end());
	range_pair.first = 0.97 * y[0];
	range_pair.second = 1.03 * y[y.size() - 1];

	return range_pair;
}

std::pair<double, double> PndLmdResultPlotter::determineDiffPlotRange(
		std::map<TString, std::vector<combined_values> > &result_map) {

	std::pair<double, double> range_pair;

	std::vector<double> y;
	for (std::map<TString, std::vector<combined_values> >::iterator it =
			result_map.begin(); it != result_map.end(); it++) {
		for (unsigned int i = 0; i < it->second.size(); i++) {
			y.push_back(
					it->second[i].lumival - it->second[i].lumiref
							+ it->second[i].lumierr);
			y.push_back(
					it->second[i].lumival - it->second[i].lumiref
							- it->second[i].lumierr);
		}
	}
	std::sort(y.begin(), y.end());
	if (y[0] > 0.0)
		range_pair.first = 0.97 * y[0];
	else
		range_pair.first = 1.03 * y[0];
	if (y[y.size() - 1] > 0.0)
		range_pair.second = 1.03 * y[y.size() - 1];
	else
		range_pair.second = 0.97 * y[y.size() - 1];

	return range_pair;
}

TGraphErrors* PndLmdResultPlotter::makeGraph(std::vector<combined_values> data,
		int color) {
	unsigned int size = data.size();
	TGraphErrors *graph = new TGraphErrors(size);

	for (unsigned int i = 0; i < size; i++) {
		graph->SetPoint(i, data[i].xval, data[i].lumival);
		graph->SetPointError(i, 0, data[i].lumival);
	}

	graph->SetLineColor(color);
	graph->SetMarkerColor(color);

	return graph;
}

void PndLmdResultPlotter::makeComparisonCanvas(TString name,
		std::map<TString, std::vector<combined_values> >& result_map) {

	std::vector<TGraphErrors*> graphs;
	int colors[] = { 1, 2, 8, 9, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 17 };
	TLegend* leg = new TLegend(0.50, 0.85 - 0.05 * result_map.size(), .9, .9);

	TMultiGraph *mg = new TMultiGraph();
	int counter = 0;

	for (std::map<TString, std::vector<combined_values> >::iterator it =
			result_map.begin(); it != result_map.end(); it++) {

		TGraphErrors* ge = makeGraph(it->second, colors[counter]);
		graphs.push_back(ge);
		mg->Add(ge, "*");

		leg->AddEntry(ge, it->first, "lp");
		counter++;
	}

	std::pair<double, double> plot_range = determineDiffPlotRange(result_map);

	if (graphs.size() > 0) {
		TCanvas *c = new TCanvas("lmd_comp_plot", "luminosity comparison", 1000,
				700);
		c->Divide(1, 2);
		c->cd(1);
		mg->Draw("a"); // draw first to be able to set axis stuff below without segfaulting
		mg->GetYaxis()->SetRangeUser(plot_range.first, plot_range.second);
		mg->GetXaxis()->SetTitle("# of bins");
		mg->GetYaxis()->SetTitle("int. luminosity");
		mg->GetYaxis()->SetTitleOffset(1.35);
		mg->Draw("a");

		c->cd(2);
		leg->Draw();

		/*if (nominal_lumi) {
		 std::cout << "nominal luminosity: " << nominal_lumi << std::endl;
		 TLine *line = new TLine(mg->GetXaxis()->GetXmin(), nominal_lumi,
		 mg->GetXaxis()->GetXmax(), nominal_lumi);
		 line->Draw();
		 }*/

		c->SaveAs(name + ".pdf");

		delete (c);
	}
}

std::pair<double, double> PndLmdResultPlotter::calulateLumiRelDiff(double lumi,
		double lumi_err, double lumi_ref) {
	return std::make_pair(100.0 * (lumi - lumi_ref) / lumi_ref,
			100.0 * lumi_err / lumi_ref);
}

double PndLmdResultPlotter::calculateYPos(double line, double text_toppos_,
		double text_spacing_, bool log) {
	if (log) {
		return pow(10,
				gPad->GetUymin()
						+ (gPad->GetUymax() - gPad->GetUymin())
								* (text_toppos_ - line * text_spacing_));
	} else {
		return gPad->GetUymin()
				+ (gPad->GetUymax() - gPad->GetUymin())
						* (text_toppos_ - line * text_spacing_);
	}
}

DataStructs::DimensionRange PndLmdResultPlotter::generatePlotRange(
		const PndLmdAbstractData &lmd_abs_data,
		const PndLmdLumiFitOptions &fit_options) const {
	PndLmdFitFacade fit_facade;
	DataStructs::DimensionRange plot_range;
	if (use_plot_range) {
		plot_range.range_low = theta_plot_range_low;
		plot_range.range_high = theta_plot_range_high;
	} else {
		plot_range.range_low =
				fit_facade.calcRange(lmd_abs_data, &fit_options).first;
		plot_range.range_high =
				fit_facade.calcRange(lmd_abs_data, &fit_options).second;
	}
	return plot_range;
}

TGraphAsymmErrors* PndLmdResultPlotter::createGraphFromFitResult(
		const PndLmdLumiFitOptions &fit_opt, PndLmdData &data) {

	PndLmdModelFactory model_factory;
	shared_ptr<Model1D> model = model_factory.generate1DModel(
			fit_opt.getFitModelOptions(), data.getLabMomentum());
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			data.getFitResult(fit_opt)->getModelFitResult());

	// ok just evaluate the function at 500 points in the range (is default);
	ModelVisualizationProperties1D vis_prop;

	vis_prop.setBinningFactor(data.getBinningFactor());
	vis_prop.setPlotRange(generatePlotRange(data, fit_opt));

	return root_plotter.createGraphFromModel1D(model, vis_prop);
}

TGraphAsymmErrors* PndLmdResultPlotter::createSmearingGraphFromFitResult(
		const PndLmdLumiFitOptions &fit_opt, PndLmdResolution &res_data) {
	PndLmdModelFactory model_factory;
	shared_ptr<Model1D> model = model_factory.generate1DResolutionModel(
			fit_opt.getFitModelOptions());
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			res_data.getFitResult(fit_opt)->getModelFitResult());

	// all parameters of the smearing model have to be freed otherwise the
	// parametrization model will overwrite the values during the evaluation
	model->getModelParameterSet().freeAllModelParameters();

	ModelVisualizationProperties1D vis_prop;

	vis_prop.setBinningFactor(res_data.getBinningFactor());
	vis_prop.setPlotRange(generatePlotRange(res_data, fit_opt));

	return root_plotter.createGraphFromModel1D(model, vis_prop);
}

std::pair<TGraphAsymmErrors*, TGraphAsymmErrors*> PndLmdResultPlotter::createResidual(
		const PndLmdLumiFitOptions &fit_opt, PndLmdData &data) {
	const TH1D *data_hist = data.get1DHistogram();

	PndLmdModelFactory model_factory;

	shared_ptr<Model1D> model = model_factory.generate1DModel(
			fit_opt.getFitModelOptions(), data.getLabMomentum());
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			data.getFitResult(fit_opt)->getModelFitResult());

	int counter = 0;
	double xvals[data_hist->GetNbinsX()];
	double yvals[data_hist->GetNbinsX()];
	double yerrs[data_hist->GetNbinsX()];
	double model_yvals[data_hist->GetNbinsX()];
	double model_errs_low[data_hist->GetNbinsX()];
	double model_errs_high[data_hist->GetNbinsX()];

	for (unsigned int i = 0; i < data_hist->GetNbinsX(); i++) {
		double eval_point = data_hist->GetBinCenter(i);
		if (!fit_opt.getEstimatorOptions().getFitRangeX().isDataWithinRange(
				eval_point)) {
			continue;
		}
		xvals[counter] = eval_point;
		yvals[counter] = data_hist->GetBinContent(i)
				- model->evaluate(&eval_point) * data.getBinningFactor();
		model_yvals[counter] = 0.0;
		yerrs[counter] = data_hist->GetBinError(i);

		if (yvals[counter] > 0.0) {
			model_errs_low[counter] = model->getUncertaincy(&eval_point).second
					* data.getBinningFactor();
			model_errs_high[counter] = model->getUncertaincy(&eval_point).first
					* data.getBinningFactor();
		} else {
			model_errs_low[counter] = model->getUncertaincy(&eval_point).first
					* data.getBinningFactor();
			model_errs_high[counter] = model->getUncertaincy(&eval_point).second
					* data.getBinningFactor();
		}
		counter++;
	}
	return std::make_pair(
			new TGraphAsymmErrors(counter, xvals, yvals, 0, 0, yerrs, yerrs),
			new TGraphAsymmErrors(counter, xvals, model_yvals, 0, 0, model_errs_low,
					model_errs_high));
}

TGraphAsymmErrors* PndLmdResultPlotter::createAcceptanceGraph(
		PndLmdAcceptance *acc) {
	TEfficiency *eff = acc->getAcceptance1D(); // false = angular acceptance
	TCanvas c;
	eff->Draw();
	c.Update();
	return new TGraphAsymmErrors(*eff->GetPaintedGraph());
}

PndLmdResultPlotter::graph_bundle PndLmdResultPlotter::makeAcceptanceBundle(
		PndLmdData& data, const PndLmdLumiFitOptions &fit_options) {
	graph_bundle acc_bundle;
	acc_bundle.acceptance_1d = 0;

	acc_bundle.is_acceptance = true;
	acc_bundle.is_resolution = false;
	acc_bundle.is_residual = false;

	PndLmdLumiFitResult *fit_result = data.getFitResult(fit_options);

	if (fit_options.getFitModelOptions().acceptance_correction_active) {
		if (fit_result)
			acc_bundle.plab = data.getLabMomentum();

		PndLmdAcceptance *acc = fit_options.getFitModelOptions().acceptance;
		TEfficiency *eff = acc->getAcceptance1D(); // false = angular acceptance
		TCanvas c;
		eff->Draw();
		c.Update();
		acc_bundle.acceptance_1d = new TGraphAsymmErrors(*eff->GetPaintedGraph());
		if (acc->getSecondaryDimension().is_active) {
			eff = acc->getAcceptance2D();
			eff->Draw("COLZ");
			c.Update();
			acc_bundle.hist2d = new TH2D(*(TH2D*) eff->GetPaintedHistogram());
		}
		acc_bundle.fit_options = fit_options;
	}
	return acc_bundle;
}

PndLmdResultPlotter::graph_bundle PndLmdResultPlotter::makeGraphBundle1D(
		PndLmdData& data, const PndLmdLumiFitOptions &fit_options) {
	std::cout << "Creating graph bundle!" << std::endl;

	double lumi_ref = data.getReferenceLuminosity();

	PndLmdLumiFitResult *fit_res = data.getFitResult(fit_options);

	graph_bundle lmd_graph_bundle;
	lmd_graph_bundle.is_resolution = false;
	lmd_graph_bundle.is_acceptance = false;
	lmd_graph_bundle.is_residual = false;
	lmd_graph_bundle.plab = data.getLabMomentum();

	lmd_graph_bundle.fit_options = fit_options;

	TH1D* hist = data.get1DHistogram();

	hist->SetTitle("");
	hist->SetStats(0);

	lmd_graph_bundle.hist1d = hist;

	if (fit_res->getModelFitResult().getFitStatus() == 0) {
		lmd_graph_bundle.model = createGraphFromFitResult(fit_options, data);
		lmd_graph_bundle.residual = createResidual(fit_options, data);
	} else {
		lmd_graph_bundle.model = 0;
		//TGraphAsymmErrors *temp = 0;
		//lmd_graph_bundle.residual = std::make_pair(temp,temp);
	}

	stringstream strstream;
	strstream.precision(3);

	strstream << "p_{lab} = " << lmd_graph_bundle.plab << " GeV";
	lmd_graph_bundle.labels.push_back(
			std::make_pair(TString(strstream.str()), 1));
	strstream.str("");

	if (fit_res->getModelFitResult().getFitStatus() == 0) {
		/*strstream << "#chi^{2}/NDF = " << fit_res->getRedChiSquare();
		 lmd_graph_bundle.labels.push_back(
		 std::make_pair(TString(strstream.str()), 1));
		 strstream.str("");

		 strstream << "lumi. diff. = " << fit_res->getLuminosity() - lumi_ref
		 << " #pm "
		 << sqrt(pow(fit_res->getLuminosityError(), 2.0) + pow(0.0, 2.0));
		 lmd_graph_bundle.labels.push_back(
		 std::make_pair(TString(strstream.str()), 1));
		 strstream.str("");*/

		strstream << "lumi. rel. diff. = "
				<< calulateLumiRelDiff(fit_res->getLuminosity(),
						fit_res->getLuminosityError(), lumi_ref).first << " #pm "
				<< calulateLumiRelDiff(fit_res->getLuminosity(),
						fit_res->getLuminosityError(), lumi_ref).second << " %";
		lmd_graph_bundle.labels.push_back(
				std::make_pair(TString(strstream.str()), 1));
		strstream.str("");
	}

	return lmd_graph_bundle;
}

PndLmdResultPlotter::graph_bundle PndLmdResultPlotter::makeResolutionGraphBundle1D(
		PndLmdResolution& res) {
	std::cout << "Creating resolution graph bundle!" << std::endl;

	const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>& fit_results =
			res.getFitResults();

	graph_bundle lmd_graph_bundle;
	lmd_graph_bundle.is_resolution = true;
	lmd_graph_bundle.is_acceptance = false;
	lmd_graph_bundle.is_residual = false;
	lmd_graph_bundle.plab = res.getLabMomentum();
	lmd_graph_bundle.model = 0;

	TH1D* hist = res.get1DHistogram();

	hist->SetTitle("");
	hist->SetStats(0);

	lmd_graph_bundle.hist1d = hist;

	if (fit_results.size() > 0) {
		lmd_graph_bundle.fit_options = fit_results.begin()->first;

		if (fit_results.begin()->second->getModelFitResult().getFitStatus() == 0) {
			lmd_graph_bundle.model = createSmearingGraphFromFitResult(
					fit_results.begin()->first, res);
		}
	}

	stringstream strstream;
	strstream.precision(3);

	strstream << "p_{lab} = " << lmd_graph_bundle.plab << " GeV";
	lmd_graph_bundle.labels.push_back(
			std::make_pair(TString(strstream.str()), 1));
	strstream.str("");

	strstream << "#theta_{MC} = "
			<< 1000
					* res.getPrimarySelectionDimension().dimension_range.getDimensionMean()
			<< " mrad";
	lmd_graph_bundle.labels.push_back(
			std::make_pair(TString(strstream.str()), 1));
	strstream.str("");

	return lmd_graph_bundle;
}

std::map<PndLmdLumiFitOptions, std::map<int, PndLmdResultPlotter::graph_bundle>,
		PndLmdResultPlotter::fit_options_compare> PndLmdResultPlotter::makeGraphBundles1D(
		std::vector<PndLmdData> &data_vec) {

	std::map<PndLmdLumiFitOptions,
			std::map<int, PndLmdResultPlotter::graph_bundle>,
			PndLmdResultPlotter::fit_options_compare> return_map;

	// model types
	LumiFit::PndLmdFitModelOptions fitop_tmctruth(LumiFit::MC, LumiFit::T);
	LumiFit::PndLmdFitModelOptions fitop_thmctruth(LumiFit::MC, LumiFit::THETA);
	LumiFit::PndLmdFitModelOptions fitop_mcacc(LumiFit::MC_ACC, LumiFit::THETA);
	LumiFit::PndLmdFitModelOptions fitop_normal(LumiFit::RECO, LumiFit::THETA);

	// go through data map
	for (std::vector<PndLmdData>::iterator top_it = data_vec.begin();
			top_it != data_vec.end(); top_it++) {

		// we only want IP info, so throw out everything else
		if (top_it->getPrimaryDimension().dimension_options.track_param_type
				!= LumiFit::IP) {
			continue;
		}
		// ok we are actually only interested in the dimension type and track type

		// loop over fit results
		map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> fit_results =
				top_it->getFitResults();
		for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::iterator it =
				fit_results.begin(); it != fit_results.end(); it++) {

			// PAD1: if we have MC data and momentum transfer and we fit that with mc_t model
			// skip this pad in the beginning to find the matching panels later due to different fit ranges

			// PAD2:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::MC
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				int pad_number = 2;

				if (fitop_thmctruth.equalBinaryOptions(
						it->first.getFitModelOptions())) {
					return_map[it->first][pad_number] = makeGraphBundle1D(*top_it,
							it->first);
				}
			}
			// PAD3:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::MC_ACC
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				int pad_number = 3;

				if (fitop_mcacc.equalBinaryOptions(it->first.getFitModelOptions())) {
					return_map[it->first][pad_number] = makeAcceptanceBundle(*top_it,
							it->first);
				}
			}
			// PAD4:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::MC_ACC
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				int pad_number = 4;

				if (fitop_mcacc.equalBinaryOptions(it->first.getFitModelOptions())) {
					return_map[it->first][pad_number] = makeGraphBundle1D(*top_it,
							it->first);
				}
			}
			// PAD5:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::RECO
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				int pad_number = 5;

				if (fitop_mcacc.equalBinaryOptions(it->first.getFitModelOptions())) {
					return_map[it->first][pad_number] = makeGraphBundle1D(*top_it,
							it->first);
				}
			}
			// PAD6:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::RECO
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				int pad_number = 6;

				if (fitop_normal.equalBinaryOptions(it->first.getFitModelOptions())) {
					return_map[it->first][pad_number] = makeGraphBundle1D(*top_it,
							it->first);
				}
			}
			// PAD7:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::MC_ACC
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				int pad_number = 7;

				if (fitop_mcacc.equalBinaryOptions(it->first.getFitModelOptions())) {
					return_map[it->first][pad_number] = makeGraphBundle1D(*top_it,
							it->first);
					return_map[it->first][pad_number].is_residual = true;
				}
			}
			// PAD8:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::RECO
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				int pad_number = 8;

				if (fitop_mcacc.equalBinaryOptions(it->first.getFitModelOptions())) {
					return_map[it->first][pad_number] = makeGraphBundle1D(*top_it,
							it->first);
					return_map[it->first][pad_number].is_residual = true;
				}
			}
			// PAD9:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::RECO
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				int pad_number = 9;

				if (fitop_normal.equalBinaryOptions(it->first.getFitModelOptions())) {
					return_map[it->first][pad_number] = makeGraphBundle1D(*top_it,
							it->first);
					return_map[it->first][pad_number].is_residual = true;
				}
			}
		}
	}

	// go through data map again and try to match the momentum transfer data fits
	for (std::vector<PndLmdData>::iterator top_it = data_vec.begin();
			top_it != data_vec.end(); top_it++) {

		// first determine which data objects we have here
		if (top_it->getPrimaryDimension().dimension_options.track_param_type
				!= LumiFit::IP) {
			return return_map;
		}
		// ok we are actually only interested in the dimension type and track type

		// PAD1: if we have MC data and momentum transfer and we fit that with mc_t model
		if (top_it->getPrimaryDimension().dimension_options.track_type
				== LumiFit::MC
				&& top_it->getPrimaryDimension().dimension_options.dimension_type
						== LumiFit::T) {
			int pad_number = 1;
			map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> fit_results =
					top_it->getFitResults();
			for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::iterator it =
					fit_results.begin(); it != fit_results.end(); it++) {
				if (fitop_tmctruth.equalBinaryOptions(it->first.getFitModelOptions())) {

					// loop over current return map and find appropriate canvas
					for (std::map<PndLmdLumiFitOptions,
							std::map<int, PndLmdResultPlotter::graph_bundle>,
							PndLmdResultPlotter::fit_options_compare>::iterator return_map_it =
							return_map.begin(); return_map_it != return_map.end();
							return_map_it++) {
						DataStructs::DimensionRange lmd_range =
								return_map_it->first.getEstimatorOptions().getFitRangeX();

						if (it->first.getEstimatorOptions().getFitRangeX().range_low
								== lumi_helper.getMomentumTransferFromTheta(
										top_it->getLabMomentum(), lmd_range.range_low)) {
							if (it->first.getEstimatorOptions().getFitRangeX().range_high
									== lumi_helper.getMomentumTransferFromTheta(
											top_it->getLabMomentum(), lmd_range.range_high)) {
								return_map_it->second[pad_number] = makeGraphBundle1D(*top_it,
										it->first);
								break;
							}
						}
					}
				}
			}

		}
	}

	return return_map;
}

void PndLmdResultPlotter::fillSinglePad(TCanvas *c, graph_bundle &gb,
		bool log_scale, bool labels_on) {
	if (c) {
		if (log_scale)
			gPad->SetLogy(1);
		else
			gPad->SetLogy(0);

		char ytitle[100];

		if (gb.is_residual) {
			TGraphAsymmErrors* graph = gb.residual.first;
			if (graph) {
				graph->SetTitle("");

				if (gb.fit_options.getFitModelOptions().momentum_transfer_active) {
					if (theta_plot_range_low < theta_plot_range_high) {
						graph->GetXaxis()->SetRangeUser(
								-lumi_helper.getMomentumTransferFromTheta(gb.plab,
										theta_plot_range_low),
								-lumi_helper.getMomentumTransferFromTheta(gb.plab,
										theta_plot_range_high));
					}
					graph->GetXaxis()->SetTitle("t [GeV^{2}/c^{2}]");
				} else {
					if (theta_plot_range_low < theta_plot_range_high) {
						graph->GetXaxis()->SetRangeUser(theta_plot_range_low,
								theta_plot_range_high);
					}
					if (gb.is_resolution)
						graph->GetXaxis()->SetTitle("#Delta#Theta [rad]");
					else
						graph->GetXaxis()->SetTitle("#Theta [rad]");
				}
				graph->GetYaxis()->SetTitle("Data-Model");

				graph->GetYaxis()->SetTitleFont(text_font);
				graph->GetXaxis()->SetTitleFont(text_font);
				graph->GetYaxis()->SetLabelFont(text_font);
				graph->GetXaxis()->SetLabelFont(text_font);
				graph->GetXaxis()->SetLabelOffset(label_offset_x);
				graph->GetYaxis()->SetLabelOffset(label_offset_y);
				graph->GetYaxis()->SetLabelSize(text_size2);
				graph->GetXaxis()->SetLabelSize(text_size2);
				graph->GetYaxis()->SetNoExponent(false);
				graph->GetYaxis()->SetTitleOffset(title_offset_y);
				graph->GetYaxis()->SetTitleSize(text_size2);
				graph->GetXaxis()->SetTitleSize(text_size2);
				graph->Draw("AP");

				gPad->Update();

				TGraphAsymmErrors* model_zero = gb.residual.second;
				model_zero->SetLineColor(2);
				model_zero->SetMarkerColor(2);
				model_zero->Draw("PSAME");
			}
		} else {
			TH1D* hist = gb.hist1d;
			if (hist) {
				hist->SetTitle("");

				if (gb.fit_options.getFitModelOptions().momentum_transfer_active) {
					if (theta_plot_range_low < theta_plot_range_high) {
						hist->GetXaxis()->SetRangeUser(
								-lumi_helper.getMomentumTransferFromTheta(gb.plab,
										theta_plot_range_low),
								-lumi_helper.getMomentumTransferFromTheta(gb.plab,
										theta_plot_range_high));
					}
					hist->GetXaxis()->SetTitle("t [GeV^{2}/c^{2}]");
					sprintf(ytitle, "# of events / %.3e GeV^{2}/c^{2}",
							hist->GetXaxis()->GetBinWidth(1));
				} else {
					if (theta_plot_range_low < theta_plot_range_high) {
						hist->GetXaxis()->SetRangeUser(theta_plot_range_low,
								theta_plot_range_high);
					}
					if (gb.is_resolution)
						hist->GetXaxis()->SetTitle("#Delta#Theta [rad]");
					else
						hist->GetXaxis()->SetTitle("#Theta [rad]");
					sprintf(ytitle, "# of events / %.3e mrad",
							hist->GetXaxis()->GetBinWidth(1));
				}
				hist->GetYaxis()->SetTitle(ytitle);

				hist->GetYaxis()->SetTitleFont(text_font);
				hist->GetXaxis()->SetTitleFont(text_font);
				hist->GetYaxis()->SetLabelFont(text_font);
				hist->GetXaxis()->SetLabelFont(text_font);
				hist->GetXaxis()->SetLabelOffset(label_offset_x);
				hist->GetYaxis()->SetLabelOffset(label_offset_y);
				hist->GetYaxis()->SetLabelSize(text_size2);
				hist->GetXaxis()->SetLabelSize(text_size2);
				hist->GetYaxis()->SetNoExponent(false);
				hist->GetYaxis()->SetTitleOffset(title_offset_y);
				hist->GetYaxis()->SetTitleSize(text_size2);
				hist->GetXaxis()->SetTitleSize(text_size2);
				hist->Draw("E1");
				hist->SetStats(0);

				if (gb.model) {
					TGraphAsymmErrors * model = gb.model;
					model->SetLineColor(2);
					model->SetMarkerColor(2);
					//model->Draw("4same");
					model->Draw("Csame");
				}

				gPad->Update();

				if (labels_on) {
					for (unsigned int i = 0; i < gb.labels.size(); i++) {
						TLatex *label = new TLatex(text_leftpos * gPad->GetUxmax(),
								calculateYPos(1.0 * i, text_toppos, text_spacing, log_scale),
								gb.labels[i].first);
						label->SetTextSize(text_size);
						label->SetTextColor(gb.labels[i].second);
						label->Draw();
					}
				}
			}
		}
	}
}

void PndLmdResultPlotter::fillAcceptanceInPad(graph_bundle acc_bundle) {
	if (0 == acc_bundle.acceptance_1d)
		return;

	TGraphAsymmErrors *acceptance = acc_bundle.acceptance_1d;

	acceptance->Draw("AP");
	acceptance->GetXaxis()->SetTitle("#Theta [mrad]");
	acceptance->GetYaxis()->SetTitle("acc. MC / MC");
	acceptance->GetXaxis()->SetTitleSize(text_size2);
	acceptance->GetYaxis()->SetTitleSize(text_size2);
	acceptance->GetXaxis()->SetLabelSize(text_size2);
	acceptance->GetYaxis()->SetLabelSize(text_size2);
	acceptance->GetYaxis()->SetTitleOffset(1.1);

	if (theta_plot_range_low < theta_plot_range_high) {
		acceptance->GetXaxis()->SetRangeUser(theta_plot_range_low,
				theta_plot_range_high);
	}
	acceptance->SetMarkerColor(kRed);
	acceptance->SetMarkerStyle(20);

	gPad->Update();
	TLine *line = new TLine(gPad->GetUxmin(), 1.0, gPad->GetUxmax(), 1.0);
	line->Draw();
}

void PndLmdResultPlotter::fill2DAcceptanceInPad(graph_bundle acc_bundle) {
	TH2D *acceptance = acc_bundle.hist2d;

	acceptance->Draw("COLZ");
	acceptance->GetXaxis()->SetTitle("#Theta [mrad]");
	acceptance->GetYaxis()->SetTitle("#Phi [rad]");
	acceptance->GetXaxis()->SetTitleSize(text_size2);
	acceptance->GetYaxis()->SetTitleSize(text_size2);
	acceptance->GetXaxis()->SetLabelSize(text_size2);
	acceptance->GetYaxis()->SetLabelSize(text_size2);
	acceptance->GetYaxis()->SetTitleOffset(1.1);

	if (theta_plot_range_low < theta_plot_range_high) {
		acceptance->GetXaxis()->SetRangeUser(theta_plot_range_low,
				theta_plot_range_high);
	}
	acceptance->SetMarkerColor(kRed);
	acceptance->SetMarkerStyle(20);

	gPad->Update();
}

void PndLmdResultPlotter::fillOverviewCanvas(TCanvas *c,
		std::map<int, PndLmdResultPlotter::graph_bundle> &graph_bundles) {

	bool print_labels = true;

	for (std::map<int, PndLmdResultPlotter::graph_bundle>::iterator it =
			graph_bundles.begin(); it != graph_bundles.end(); it++) {
		c->cd(it->first);
		if (it->second.is_acceptance) {
			fillAcceptanceInPad(it->second);
		} else {
			bool logscale = false;
			if (it->second.fit_options.getFitModelOptions().momentum_transfer_active
					|| !it->second.fit_options.getFitModelOptions().acceptance_correction_active)
				logscale = true;
			fillSinglePad(c, it->second, logscale, print_labels);
		}
	}
}

std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> > PndLmdResultPlotter::generateLmdGraphMap(
		std::vector<PndLmdLumiHelper::lmd_graph*> graphs) {

	std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> > return_map;

	for (unsigned int i = 0; i < graphs.size(); i++) {
		TString key(graphs[i]->dependency);
		for (std::map<std::string, double, ModelStructs::string_comp>::const_iterator dependency =
				graphs[i]->remaining_dependencies.begin();
				dependency != graphs[i]->remaining_dependencies.end(); dependency++) {

			stringstream strstream;
			strstream.precision(3);
			strstream << "_" << dependency->first << "-" << dependency->second;
			key = key + strstream.str();
		}
		std::cout << key << std::endl;
		std::cout << graphs[i]->fit_result << std::endl;
		return_map[key].push_back(graphs[i]);
	}
	for (std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> >::const_iterator it =
			return_map.begin(); it != return_map.end(); it++) {
		std::cout << it->first << std::endl;
		for (unsigned int i = 0; i < it->second.size(); i++) {
			std::cout << i << " " << it->second[i]->fit_result << std::endl;
		}
	}
	return return_map;
}

void PndLmdResultPlotter::plotDPMModelParts(double plab,
		DataStructs::DimensionRange& plot_range, bool log_scale) {
	PndLmdModelFactory model_factory;

	ROOTDataHelper data_helper;
	ModelVisualizationProperties1D vis_prop;
	vis_prop.setPlotRange(plot_range);

	LumiFit::PndLmdFitModelOptions fit_op_full;
	fit_op_full.dpm_elastic_parts = LumiFit::ALL;

	shared_ptr<Model1D> full_model = model_factory.generate1DModel(fit_op_full,
			plab);
	if (full_model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphAsymmErrors* full_model_graph = root_plotter.createGraphFromModel1D(
			full_model, vis_prop);

	LumiFit::PndLmdFitModelOptions fit_op_coul = fit_op_full;
	fit_op_coul.dpm_elastic_parts = LumiFit::COUL;

	shared_ptr<Model1D> coul_model = model_factory.generate1DModel(fit_op_coul,
			plab);
	if (full_model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphAsymmErrors* coul_model_graph = root_plotter.createGraphFromModel1D(
			coul_model, vis_prop);

	LumiFit::PndLmdFitModelOptions fit_op_int = fit_op_full;
	fit_op_int.dpm_elastic_parts = LumiFit::INT;

	shared_ptr<Model1D> int_model = model_factory.generate1DModel(fit_op_int,
			plab);
	if (full_model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphAsymmErrors* int_model_graph = root_plotter.createGraphFromModel1D(
			int_model, vis_prop);

	LumiFit::PndLmdFitModelOptions fit_op_had = fit_op_full;
	fit_op_had.dpm_elastic_parts = LumiFit::HAD;

	shared_ptr<Model1D> had_model = model_factory.generate1DModel(fit_op_had,
			plab);
	if (full_model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphAsymmErrors* had_model_graph = root_plotter.createGraphFromModel1D(
			had_model, vis_prop);

	TCanvas c("c", "", 1000, 700);
	c.SetLogy(log_scale);
	full_model_graph->Draw("AC");
	gPad->Update();
	double top_pos = gPad->GetUymax();
	if (log_scale)
		top_pos = pow(10, gPad->GetUymax());
	int_model_graph->Draw("AC");
	gPad->Update();
	double bottom_pos = gPad->GetUymin();
	if (log_scale)
		bottom_pos = pow(10, gPad->GetUymin());
	full_model_graph->GetYaxis()->SetRangeUser(bottom_pos, top_pos);

	full_model_graph->SetLineWidth(2);
	full_model_graph->Draw("AC");
	coul_model_graph->SetLineWidth(2);
	coul_model_graph->SetLineColor(2);
	coul_model_graph->Draw("CSAME");
	int_model_graph->SetLineWidth(2);
	int_model_graph->SetLineColor(8);
	int_model_graph->Draw("CSAME");
	had_model_graph->SetLineWidth(2);
	had_model_graph->SetLineColor(9);
	had_model_graph->Draw("CSAME");

	stringstream strstream;
	strstream.precision(3);

	strstream << "DPMModels_" << plab << ".pdf";
	c.SaveAs(strstream.str().c_str());
}

void PndLmdResultPlotter::makeFitResultBooky(
		std::map<PndLmdLumiFitOptions,
				std::map<int, PndLmdResultPlotter::graph_bundle>,
				PndLmdResultPlotter::fit_options_compare> &graph_bundle_map,
		TString filename) {

	TCanvas c("fit_overview", "fit_overview", 1000, 700);

	c.Divide(3, 3);

	c.Print(filename + "_overview.pdf["); // No actual print, just open file
	for (std::map<PndLmdLumiFitOptions,
			std::map<int, PndLmdResultPlotter::graph_bundle>,
			PndLmdResultPlotter::fit_options_compare>::iterator i =
			graph_bundle_map.begin(); i != graph_bundle_map.end(); i++) {
		fillOverviewCanvas(&c, i->second);
		c.cd();
		c.Print(filename + "_overview.pdf"); // actually print canvas to file
	}
	c.Print(filename + "_overview.pdf]");

}

void PndLmdResultPlotter::makeResolutionSummaryPlots(TFile *f) {
	PndLmdLumiHelper lmd_helper;

	bool create_plot = false;

	std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> > graphs =
			generateLmdGraphMap(lmd_helper.getResolutionModelResultsFromFile(f));

	for (std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> >::const_iterator it =
			graphs.begin(); it != graphs.end(); it++) {
		std::cout << it->first << std::endl;
		for (unsigned int i = 0; i < it->second.size(); i++) {
			std::cout << i << " " << it->second[i]->fit_result << std::endl;
		}
	}

	for (std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> >::const_iterator it =
			graphs.begin(); it != graphs.end(); it++) {
		TCanvas c("c", it->first, 1000, 700);
		if (it->second.size() > 0) {
			if (it->second[0]->fit_options->getFitModelOptions().smearing_model
					== LumiFit::GAUSSIAN)
				c.Divide(2, 2);
			else if (it->second[0]->fit_options->getFitModelOptions().smearing_model
					== LumiFit::DOUBLE_GAUSSIAN)
				c.Divide(3, 3);
			else
				c.Divide(3, 2);
		}

		create_plot = true;

		for (unsigned int i = 0; i < it->second.size(); i++) {
			std::cout << it->second[i]->fit_result << std::endl;
			c.cd(i + 1);
			it->second[i]->graph->SetTitle(
					TString(it->second[i]->parameter_name_stack[1].first) + ":"
							+ TString(it->second[i]->parameter_name_stack[1].second));
			it->second[i]->graph->Draw("A*");

			bool model_exists = true;
			PndLmdModelFactory model_factory; // construct model factory
			// specify which of type of smearing model we want to generate

			// generate the model
			shared_ptr<Model> model = model_factory.generate1DResolutionModel(
					it->second[i]->fit_options->getFitModelOptions());
			// get the model that we have to fit to the data
			for (std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator parameter_name =
					it->second[i]->parameter_name_stack.begin();
					parameter_name != it->second[i]->parameter_name_stack.end();
					parameter_name++) {
				ParametrizationProxy par_proxy =
						model->getModelParameterHandler().getParametrizationProxyForModelParameter(
								parameter_name->second.second);
				std::cout << "Finding parametrization model for "
						<< parameter_name->second.second << std::endl;

				if (par_proxy.hasParametrizationModel()) {
					model = par_proxy.getParametrizationModel()->getModel();
				} else {
					std::cout
							<< "ERROR: Not able to obtain parametrization model for parameter "
							<< parameter_name->second.second << "!" << std::endl;
					model_exists = false;
					break;
				}
			}

			if (model_exists && it->second[i]->fit_result) {
				std::cout << it->second[i]->fit_result << std::endl;
				std::cout << it->second[i]->fit_result->getFinalEstimatorValue()
						<< std::endl;
				model->getModelParameterHandler().initModelParametersFromFitResult(
						*it->second[i]->fit_result);
				model->getModelParameterSet().printInfo();

				ROOTDataHelper data_helper;

				shared_ptr<Data> data(new Data(1));
				data_helper.fillBinnedData(data, it->second[i]->graph);
				ModelVisualizationProperties1D vis_prop(data);

				DataStructs::DimensionRange plot_range =
						it->second[i]->fit_options->getEstimatorOptions().getFitRangeX();
				std::cout << "plotting range: " << plot_range.range_low << " - "
						<< plot_range.range_high << std::endl;
				vis_prop.setPlotRange(plot_range);
				TGraphAsymmErrors *model_graph = root_plotter.createGraphFromModel1D(
						model, vis_prop);
				model_graph->SetLineColor(2);
				model_graph->Draw("CSAME");

				create_plot = true;
			}
		}
		if (create_plot) {
			c.cd();
			TLatex title(0.2, 0.33, it->first);
			title.SetTextSize(0.05);
			title.Draw();
			c.SaveAs(TString("resolution_parameters_") + it->first + ".pdf");
		}
	}
}

void PndLmdResultPlotter::makeResolutionBooky(
		std::vector<PndLmdResolution> &res_vec, TString filename) {

	std::vector<PndLmdResultPlotter::graph_bundle> graph_bundles;

	for (unsigned int i = 0; i < res_vec.size(); i++) {
		graph_bundles.push_back(makeResolutionGraphBundle1D(res_vec[i]));
	}

	makeBooky(graph_bundles, filename, 5, 4);
}

void PndLmdResultPlotter::makeBooky(
		std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundles,
		TString filename, int x, int y) {
	std::cout << "attempting to fill canvas with " << graph_bundles.size()
			<< " graphs (filename: " << filename << ")." << std::endl;

	TCanvas c("overview", "overview", 1000, 700);
	c.Divide(x, y);
	unsigned int stepsize = 1;
	c.Print(filename + ".pdf["); // No actual print, just open file
	for (unsigned int i = 0; i < graph_bundles.size(); i = i + stepsize) {
		c.cd(i / stepsize % (x * y) + 1);
		fillSinglePad(&c, graph_bundles[i], false, true);
		if (i / stepsize % (x * y) + 1 == (x * y)
				|| i >= graph_bundles.size() - 1) {
			if (i >= graph_bundles.size() - 1) {
				// clear remaining pads
				for (unsigned int temp = i / stepsize % (x * y) + 2; temp <= (x * y);
						temp++) {
					c.cd(temp)->Clear();
				}
			}
			c.cd();
			c.Print(filename + ".pdf"); // actually print canvas to file
		}
	}
	c.Print(filename + ".pdf]");
}
