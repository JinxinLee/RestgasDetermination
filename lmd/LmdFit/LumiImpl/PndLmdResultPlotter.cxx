/*
 * PndLmdResultPlotter.cxx
 *
 *  Created on: Mar 25, 2013
 *      Author: steve
 */

#include "PndLmdResultPlotter.h"

#include "fit/data/ROOT/ROOTDataHelper.h"
#include "visualization/ModelVisualizationProperties1D.h"
#include "model/PndLmdModelFactory.h"
#include "fit/PndLmdLumiFitResult.h"
#include "data/PndLmdAngularData.h"
#include "data/PndLmdAcceptance.h"
#include "data/PndLmdResolution.h"
#include "fit/PndLmdFitFacade.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iosfwd>
#include <ostream>
#include <iomanip>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/regex.hpp"

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

using boost::filesystem::path;
using boost::filesystem::directory_iterator;
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

void PndLmdResultPlotter::drawMultipleGraphsOnCanvas(
		std::vector<std::pair<TGraphErrors*, TString> >& graphs) {
	int colors[] = { 1, 2, 8, 9, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 17 };
	TLegend* leg = new TLegend(0.75, 0.85 - 0.05 * graphs.size(), .9, .9);

	TMultiGraph *mg = new TMultiGraph();

	for (unsigned int i = 0; i < graphs.size(); i++) {
		graphs[i].first->SetMarkerColor(colors[i]);
		graphs[i].first->SetLineColor(colors[i]);
		graphs[i].first->SetMarkerStyle(8);
		graphs[i].first->SetMarkerSize(1.0);
		mg->Add(graphs[i].first);

		leg->AddEntry(graphs[i].first, graphs[i].second, "lp");
	}

	mg->Draw("AP"); // draw first to be able to set axis stuff below without segfaulting

	if (graphs.size() > 0) {
		mg->GetXaxis()->SetTitle(graphs[0].first->GetXaxis()->GetTitle());
		mg->GetYaxis()->SetTitle(graphs[0].first->GetYaxis()->GetTitle());
	}
	mg->GetYaxis()->SetTitleOffset(1.35);

	leg->Draw();
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
		const PndLmdLumiFitOptions &fit_opt, PndLmdAngularData &data) {

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

TGraphAsymmErrors* PndLmdResultPlotter::createVertexGraphFromFitResult(
		const PndLmdLumiFitOptions &fit_opt, PndLmdVertexData &data) {

	PndLmdModelFactory model_factory;
	shared_ptr<Model1D> model = model_factory.generate1DVertexModel(
			fit_opt.getFitModelOptions());
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
		const PndLmdLumiFitOptions &fit_opt, PndLmdAngularData &data) {
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
		PndLmdAngularData& data, const PndLmdLumiFitOptions &fit_options) {
	graph_bundle acc_bundle;
	acc_bundle.data_graph_1D = 0;

	PndLmdLumiFitResult *fit_result = data.getFitResult(fit_options);

	if (fit_options.getFitModelOptions().acceptance_correction_active) {
		PndLmdAcceptance *acc = fit_options.getFitModelOptions().acceptance;
		if (acc) {
			TEfficiency *eff = acc->getAcceptance1D(); // false = angular acceptance
			TCanvas c;
			eff->Draw();
			c.Update();
			acc_bundle.data_graph_1D = new TGraphAsymmErrors(*eff->GetPaintedGraph());
			acc_bundle.data_graph_1D->GetXaxis()->SetTitle("#Theta [rad]");
			acc_bundle.data_graph_1D->GetYaxis()->SetTitle("acc. MC / MC");

			acc_bundle.data_graph_1D->SetMarkerColor(kRed);
			acc_bundle.data_graph_1D->SetMarkerStyle(20);

			acc_bundle.x_parallel_lines.push_back(1.0);

			if (acc->getSecondaryDimension().is_active) {
				eff = acc->getAcceptance2D();
				eff->Draw("COLZ");
				c.Update();
				acc_bundle.hist2d = new TH2D(*(TH2D*) eff->GetPaintedHistogram());
				acc_bundle.hist2d->GetXaxis()->SetTitle("#Theta [rad]");
				acc_bundle.hist2d->GetYaxis()->SetTitle("#Phi [rad]");
				acc_bundle.hist2d->GetZaxis()->SetTitle("acc. MC / MC");
			}
			acc_bundle.est_options = fit_options.getEstimatorOptions();
		}
	}
	return acc_bundle;
}

PndLmdResultPlotter::graph_bundle PndLmdResultPlotter::makeGraphBundle1D(
		PndLmdAngularData& data, const PndLmdLumiFitOptions &fit_options) {
	std::cout << "Creating graph bundle!" << std::endl;

	double lumi_ref = data.getReferenceLuminosity();

	PndLmdLumiFitResult *fit_res = data.getFitResult(fit_options);

	graph_bundle lmd_graph_bundle;

	lmd_graph_bundle.est_options = fit_options.getEstimatorOptions();

	TH1D* hist = data.get1DHistogram();

	hist->SetTitle("");
	hist->SetStats(0);

	std::stringstream ss;
	if (fit_options.getFitModelOptions().momentum_transfer_active) {
		if (theta_plot_range_low < theta_plot_range_high) {
			hist->GetXaxis()->SetRangeUser(
					-lumi_helper.getMomentumTransferFromTheta(data.getLabMomentum(),
							theta_plot_range_low),
					-lumi_helper.getMomentumTransferFromTheta(data.getLabMomentum(),
							theta_plot_range_high));
		}
		hist->GetXaxis()->SetTitle("t [GeV^{2}/c^{2}]");
		ss << "# of tracks / " << hist->GetXaxis()->GetBinWidth(1)
				<< " GeV^{2}/c^{2}";
	} else {
		if (theta_plot_range_low < theta_plot_range_high) {
			hist->GetXaxis()->SetRangeUser(theta_plot_range_low,
					theta_plot_range_high);
		}
		hist->GetXaxis()->SetTitle("#Theta [rad]");
		ss << "# of tracks / " << 1000.0 * hist->GetXaxis()->GetBinWidth(1)
				<< " mrad";
	}
	hist->GetYaxis()->SetTitle(ss.str().c_str());

	lmd_graph_bundle.hist1d = hist;

	if (0 == fit_res->getModelFitResult().getFitStatus()) {
		lmd_graph_bundle.model = createGraphFromFitResult(fit_options, data);
		lmd_graph_bundle.model->SetLineColor(2);
		lmd_graph_bundle.model->SetMarkerColor(2);

		lmd_graph_bundle.residual = createResidual(fit_options, data);
		lmd_graph_bundle.residual.first->GetXaxis()->SetTitle("#Theta [rad]");
		lmd_graph_bundle.residual.first->GetYaxis()->SetTitle("Data-Model");
		lmd_graph_bundle.residual.second->SetLineColor(2);
		lmd_graph_bundle.residual.second->SetMarkerColor(2);
	}

	stringstream strstream;
	strstream.precision(3);

	strstream << "p_{lab} = " << data.getLabMomentum() << " GeV";
	lmd_graph_bundle.labels.push_back(
			std::make_pair(TString(strstream.str()), 1));
	strstream.str("");

	if (fit_res->getModelFitResult().getFitStatus() == 0) {
		strstream << std::setprecision(3) << "#Delta L/L = "
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

	TH1D* hist = res.get1DHistogram();

	hist->SetTitle("");
	hist->SetStats(0);

	hist->GetXaxis()->SetTitle("#Delta#Theta [rad]");

	lmd_graph_bundle.hist1d = hist;

	if (fit_results.size() > 0) {
		lmd_graph_bundle.est_options =
				fit_results.begin()->first.getEstimatorOptions();

		if (fit_results.begin()->second->getModelFitResult().getFitStatus() == 0) {
			lmd_graph_bundle.model = createSmearingGraphFromFitResult(
					fit_results.begin()->first, res);
			lmd_graph_bundle.model->SetLineColor(2);
			lmd_graph_bundle.model->SetMarkerColor(2);
		}
	}

	stringstream strstream;
	strstream.precision(3);

	strstream << "p_{lab} = " << res.getLabMomentum() << " GeV";
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

PndLmdResultPlotter::graph_bundle PndLmdResultPlotter::makeVertexGraphBundle1D(
		PndLmdVertexData& data) {
	std::cout << "Creating vertex graph bundle!" << std::endl;

	const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>& fit_results =
			data.getFitResults();

	graph_bundle lmd_graph_bundle;

	TH1D* hist = data.get1DHistogram();

	hist->SetTitle("");
	hist->SetStats(0);

	std::stringstream ss;
	if (LumiFit::X == data.getPrimaryDimension().dimension_options.dimension_type)
		ss << "x";
	else if (LumiFit::Y
			== data.getPrimaryDimension().dimension_options.dimension_type)
		ss << "y";
	else if (LumiFit::Z
			== data.getPrimaryDimension().dimension_options.dimension_type)
		ss << "z";

	if (LumiFit::RECO == data.getPrimaryDimension().dimension_options.track_type)
		ss << "_{rec}";
	else if (LumiFit::MC
			== data.getPrimaryDimension().dimension_options.track_type)
		ss << "_{MC}";

	ss << " [cm]";

	hist->GetXaxis()->SetTitle(ss.str().c_str());

	ss.str("");
	ss << "# of tracks / " << hist->GetXaxis()->GetBinWidth(1) << " cm";
	hist->GetYaxis()->SetTitle(ss.str().c_str());

	lmd_graph_bundle.hist1d = hist;

	std::cout << "num fit results: " << fit_results.size() << std::endl;
	if (fit_results.size() > 0) {
		lmd_graph_bundle.est_options =
				fit_results.begin()->first.getEstimatorOptions();

		if (fit_results.begin()->second->getModelFitResult().getFitStatus() == 0) {
			lmd_graph_bundle.model = createVertexGraphFromFitResult(
					fit_results.begin()->first, data);
			lmd_graph_bundle.model->SetLineColor(2);
			lmd_graph_bundle.model->SetMarkerColor(2);
		}
	}

	return lmd_graph_bundle;
}

std::map<PndLmdLumiFitOptions, std::map<int, PndLmdResultPlotter::graph_bundle>,
		PndLmdResultPlotter::fit_options_compare> PndLmdResultPlotter::makeGraphBundles1D(
		std::vector<PndLmdAngularData> &data_vec) {

	std::map<PndLmdLumiFitOptions,
			std::map<int, PndLmdResultPlotter::graph_bundle>,
			PndLmdResultPlotter::fit_options_compare> return_map;

// model types
	LumiFit::PndLmdFitModelOptions fitop_tmctruth(LumiFit::MC, LumiFit::T);
	LumiFit::PndLmdFitModelOptions fitop_thmctruth(LumiFit::MC, LumiFit::THETA);
	LumiFit::PndLmdFitModelOptions fitop_mcacc(LumiFit::MC_ACC, LumiFit::THETA);
	LumiFit::PndLmdFitModelOptions fitop_normal(LumiFit::RECO, LumiFit::THETA);

// go through data map
	for (std::vector<PndLmdAngularData>::iterator top_it = data_vec.begin();
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
	for (std::vector<PndLmdAngularData>::iterator top_it = data_vec.begin();
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

std::map<int, PndLmdResultPlotter::graph_bundle> PndLmdResultPlotter::makeVertexGraphBundles1D(
		std::vector<PndLmdVertexData> &data_vec) {
	std::map<int, PndLmdResultPlotter::graph_bundle> return_map;

	for (unsigned int i = 0; i < data_vec.size(); i++) {
		if (data_vec[i].getName().EqualTo("mc_x")) {
			return_map[1] = makeVertexGraphBundle1D(data_vec[i]);
			return_map[1].log_scale = true;
		} else if (data_vec[i].getName().EqualTo("mc_y")) {
			return_map[2] = makeVertexGraphBundle1D(data_vec[i]);
			return_map[2].log_scale = true;
		} else if (data_vec[i].getName().EqualTo("mc_z")) {
			return_map[3] = makeVertexGraphBundle1D(data_vec[i]);
		} else if (data_vec[i].getName().EqualTo("reco_x")) {
			return_map[4] = makeVertexGraphBundle1D(data_vec[i]);
		} else if (data_vec[i].getName().EqualTo("reco_y")) {
			return_map[5] = makeVertexGraphBundle1D(data_vec[i]);
		} else if (data_vec[i].getName().EqualTo("reco_z")) {
			return_map[6] = makeVertexGraphBundle1D(data_vec[i]);
		}
	}

	return return_map;
}

std::pair<TGraphErrors*, TGraphErrors*> PndLmdResultPlotter::makeIPParameterDependencyGraphBundle(
		std::vector<PndLmdVertexData> &vertex_data,
		const LumiFit::LmdSimIPParameters &dependency, TString ytitle_subscript) {

	TGraphErrors *mean = 0;
	TGraphErrors *width = 0;

	if (vertex_data.size() > 0) {
		std::vector<double> x;
		std::vector<double> y_mean;
		std::vector<double> y_err_mean;

		std::vector<double> y_width;
		std::vector<double> y_err_width;

		for (unsigned int i = 0; i < vertex_data.size(); i++) {
			PndLmdLumiFitResult *fit_result =
					vertex_data[i].getFitResults().begin()->second;
			if (fit_result && fit_result->getModelFitResult().getFitStatus() == 0) {
				x.push_back(
						vertex_data[i].getSimulationIPParameters().getDependencyValue(
								dependency));

				double ideal_mean(0.0);
				double ideal_width(0.0);

				if (vertex_data[i].getPrimaryDimension().dimension_options.dimension_type
						== LumiFit::X) {
					ideal_mean = vertex_data[i].getSimulationIPParameters().offset_x_mean;
					ideal_width =
							vertex_data[i].getSimulationIPParameters().offset_x_width;
				} else if (vertex_data[i].getPrimaryDimension().dimension_options.dimension_type
						== LumiFit::X) {
					ideal_mean = vertex_data[i].getSimulationIPParameters().offset_y_mean;
					ideal_width =
							vertex_data[i].getSimulationIPParameters().offset_y_width;
				} else if (vertex_data[i].getPrimaryDimension().dimension_options.dimension_type
						== LumiFit::X) {
					ideal_mean = vertex_data[i].getSimulationIPParameters().offset_z_mean;
					ideal_width =
							vertex_data[i].getSimulationIPParameters().offset_z_width;
				}

				y_mean.push_back(
						fit_result->getModelFitResult().getFitParameter("gauss_mean").value
								- ideal_mean);
				y_err_mean.push_back(
						fit_result->getModelFitResult().getFitParameter("gauss_mean").error);

				y_width.push_back(
						fit_result->getModelFitResult().getFitParameter("gauss_sigma").value
								- ideal_width);
				y_err_width.push_back(
						fit_result->getModelFitResult().getFitParameter("gauss_sigma").error);
			}
		}

		mean = new TGraphErrors(x.size(), &x[0], &y_mean[0], 0, &y_err_mean[0]);
		mean->GetXaxis()->SetTitle(
				vertex_data[0].getSimulationIPParameters().getDependencyName(dependency).c_str());
		mean->GetYaxis()->SetTitle(
				TString("#delta#mu_{") + ytitle_subscript + "} [cm]");
		width = new TGraphErrors(x.size(), &x[0], &y_width[0], 0, &y_err_width[0]);
		width->GetXaxis()->SetTitle(
				vertex_data[0].getSimulationIPParameters().getDependencyName(dependency).c_str());
		width->GetYaxis()->SetTitle(
				TString("#delta#sigma_{") + ytitle_subscript + "} [cm]");

	}
	return std::make_pair(mean, width);
}

std::vector<PndLmdResultPlotter::graph_bundle> PndLmdResultPlotter::createGraphDifferenceBundle(
		std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundle,
		std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundle_ref) {

	std::vector<PndLmdResultPlotter::graph_bundle> diff_graph_bundle;

	if (graph_bundle.size() == graph_bundle_ref.size()) {
		for (unsigned int i = 0; i < graph_bundle.size(); i++) {
			PndLmdResultPlotter::graph_bundle gb;
			gb.hist1d = new TH1D(*graph_bundle[i].hist1d);
			gb.hist1d->Add(graph_bundle_ref[i].hist1d, -1.0);

			diff_graph_bundle.push_back(gb);
		}
	}

	return diff_graph_bundle;
}

std::vector<PndLmdResultPlotter::graph_bundle> PndLmdResultPlotter::createGraphRatioBundle(
		std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundle,
		std::vector<PndLmdResultPlotter::graph_bundle> &graph_bundle_ref) {

	std::vector<PndLmdResultPlotter::graph_bundle> diff_graph_bundle;

	if (graph_bundle.size() == graph_bundle_ref.size()) {
		for (unsigned int i = 0; i < graph_bundle.size(); i++) {
			PndLmdResultPlotter::graph_bundle gb;
			gb.hist1d = new TH1D(*graph_bundle[i].hist1d);
			double int_num = graph_bundle[i].hist1d->Integral();
			double int_denom = graph_bundle_ref[i].hist1d->Integral();

			gb.hist1d->Divide(graph_bundle[i].hist1d, graph_bundle_ref[i].hist1d,
					int_denom, int_num, "B");

			diff_graph_bundle.push_back(gb);
		}
	}

	return diff_graph_bundle;
}

void PndLmdResultPlotter::fillSinglePad(TCanvas *c, graph_bundle &gb,
		bool use_2d) {
	if (c) {
		// set logarithmic scale
		if (gb.log_scale)
			gPad->SetLogy(1);
		else
			gPad->SetLogy(0);

		TAxis *xaxis;
		TAxis *yaxis;

		if (gb.is_residual) {
			gb.residual.first->Draw("AP");
			TGraphAsymmErrors* model_zero = gb.residual.second;
			model_zero->Draw("PSAME");

			xaxis = gb.residual.first->GetXaxis();
			yaxis = gb.residual.first->GetYaxis();
		} else {
			if (use_2d && gb.hist2d) {
				gb.hist2d->Draw("COLZ");
				xaxis = gb.hist2d->GetXaxis();
				yaxis = gb.hist2d->GetYaxis();
			} else {
				if (gb.data_graph_1D) {
					gb.data_graph_1D->Draw("AP");

					xaxis = gb.data_graph_1D->GetXaxis();
					yaxis = gb.data_graph_1D->GetYaxis();
				} else if (gb.hist1d) {
					gb.hist1d->Draw("E1");

					xaxis = gb.hist1d->GetXaxis();
					yaxis = gb.hist1d->GetYaxis();
				}
			}
			if (gb.model) {
				gb.model->Draw("Csame");
			}
		}

		xaxis->SetTitleFont(text_font);
		yaxis->SetTitleFont(text_font);
		xaxis->SetLabelFont(text_font);
		yaxis->SetLabelFont(text_font);
		xaxis->SetLabelOffset(label_offset_x);
		yaxis->SetLabelOffset(label_offset_y);
		xaxis->SetLabelSize(text_size2);
		yaxis->SetLabelSize(text_size2);
		yaxis->SetNoExponent(false);
		yaxis->SetTitleOffset(title_offset_y);
		xaxis->SetTitleSize(text_size2);
		yaxis->SetTitleSize(text_size2);

		gPad->Update();
		for (unsigned int i = 0; i < gb.x_parallel_lines.size(); i++) {
			TLine *line = new TLine(gPad->GetUxmin(), gb.x_parallel_lines[i],
					gPad->GetUxmax(), gb.x_parallel_lines[i]);
			line->Draw();
		}
		for (unsigned int i = 0; i < gb.y_parallel_lines.size(); i++) {
			TLine *line = new TLine(gb.y_parallel_lines[i], gPad->GetUymin(),
					gb.y_parallel_lines[i], gPad->GetUymax());
			line->Draw();
		}

		if (gb.draw_labels) {
			for (unsigned int i = 0; i < gb.labels.size(); i++) {
				TLatex *label = new TLatex(text_leftpos * gPad->GetUxmax(),
						calculateYPos(1.0 * i, text_toppos, text_spacing, gb.log_scale),
						gb.labels[i].first);
				label->SetTextSize(text_size);
				label->SetTextColor(gb.labels[i].second);
				label->Draw();
			}
		}
	}
}

void PndLmdResultPlotter::fillOverviewCanvas(TCanvas *c,
		std::map<int, PndLmdResultPlotter::graph_bundle> &graph_bundles) {

	for (std::map<int, PndLmdResultPlotter::graph_bundle>::iterator it =
			graph_bundles.begin(); it != graph_bundles.end(); it++) {
		c->cd(it->first);
		fillSinglePad(c, it->second);
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

TGraphAsymmErrors* PndLmdResultPlotter::generateDPMModelPartGraph(double plab,
		LumiFit::DPMElasticParts dpm_elastic_part,
		DataStructs::DimensionRange& plot_range) {
	PndLmdModelFactory model_factory;

	ROOTDataHelper data_helper;
	ModelVisualizationProperties1D vis_prop;
	vis_prop.setPlotRange(plot_range);

	LumiFit::PndLmdFitModelOptions fit_op_full;
	fit_op_full.dpm_elastic_parts = dpm_elastic_part;

	shared_ptr<Model1D> model = model_factory.generate1DModel(fit_op_full, plab);
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphAsymmErrors* model_graph = root_plotter.createGraphFromModel1D(model,
			vis_prop);

	return model_graph;
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

void PndLmdResultPlotter::makeVertexFitResultBooky(
		std::vector<std::map<int, PndLmdResultPlotter::graph_bundle> > &graph_bundles,
		TString filename) {

	TCanvas c("ip_fit_overview", "ip_fit_overview", 1000, 700);

	c.Divide(3, 2);

	c.Print(filename + "_overview.pdf["); // No actual print, just open file
	for (unsigned int i = 0; i < graph_bundles.size(); i++) {
		fillOverviewCanvas(&c, graph_bundles[i]);
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

void PndLmdResultPlotter::makeResolutionDifferencesBooky(
		std::vector<PndLmdResolution> &res_vec,
		std::vector<PndLmdResolution> &res_vec_ref) {

	std::vector<PndLmdResultPlotter::graph_bundle> graph_bundles;
	for (unsigned int i = 0; i < res_vec.size(); i++) {
		graph_bundles.push_back(makeResolutionGraphBundle1D(res_vec[i]));
	}

	std::vector<PndLmdResultPlotter::graph_bundle> graph_bundles_ref;
	for (unsigned int i = 0; i < res_vec_ref.size(); i++) {
		graph_bundles_ref.push_back(makeResolutionGraphBundle1D(res_vec_ref[i]));
	}

	std::vector<PndLmdResultPlotter::graph_bundle> diff_graph_bundles =
			createGraphRatioBundle(graph_bundles, graph_bundles_ref);

	makeBooky(diff_graph_bundles, "resolution_difference", 5, 4);
}

void PndLmdResultPlotter::makeVertexDifferencesBooky(
		std::vector<PndLmdVertexData> &res_vec,
		std::vector<PndLmdVertexData> &res_vec_ref) {

	std::vector<PndLmdResultPlotter::graph_bundle> graph_bundles;
	for (unsigned int i = 0; i < res_vec.size(); i++) {
		graph_bundles.push_back(makeVertexGraphBundle1D(res_vec[i]));
	}

	std::vector<PndLmdResultPlotter::graph_bundle> graph_bundles_ref;
	for (unsigned int i = 0; i < res_vec.size(); i++) {
		graph_bundles_ref.push_back(makeVertexGraphBundle1D(res_vec[i]));
	}

	std::vector<PndLmdResultPlotter::graph_bundle> diff_graph_bundles =
			createGraphDifferenceBundle(graph_bundles, graph_bundles_ref);

	makeBooky(diff_graph_bundles, "vertex_difference", 3, 2);
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
		fillSinglePad(&c, graph_bundles[i]);
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

void PndLmdResultPlotter::plotXYOverviewGraph(
		std::vector<PndLmdVertexData> &vertex_data) {

	//unsigned int num_values = 5;
	// define scales
	//double scales[] = { 1.0, 10.0, 100.0, 1e3, 1e4 };
	// define colors for scales
	//int colors[] = { kRed + 2, kRed, kViolet, kViolet + 7, kBlue };

	unsigned int num_values = 1;
	double scales[] = { 50.0 };
	int colors[] = { kRed };

	TMultiGraph *mg = new TMultiGraph();
	TLegend* leg = new TLegend(0.9, 0.85 - 0.05 * num_values, .98, .9);

	TGraphAsymmErrors *first;

	std::vector<TLatex*> labels;

	for (unsigned int i = 0; i < num_values; i++) {
		TGraphAsymmErrors *gae = makeXYOverviewGraph(vertex_data, scales[i],
				colors[i]);
		// draw precision labels on points
		for (unsigned int point = 0; point < gae->GetN(); point++) {
			double x, y;
			gae->GetPoint(point, x, y);

			double errorx = gae->GetErrorXhigh(point);
			if (errorx < gae->GetErrorXlow(point))
				errorx = gae->GetErrorXlow(point);
			double errory = gae->GetErrorYhigh(point);
			if (errory < gae->GetErrorYlow(point))
				errory = gae->GetErrorYlow(point);
			errorx = errorx * 10000.0 / scales[i];
			errory = errory * 10000.0 / scales[i];

			std::stringstream ss;
			ss << std::fixed << std::setprecision(0) << "(" << errorx << "#mum, "
					<< errory << "#mum)";
			TLatex *label = new TLatex(x - 0.15, y + 0.03, ss.str().c_str());
			label->SetTextSize(0.02);
			labels.push_back(label);
		}

		if (i == 0)
			first = gae;
		mg->Add(gae);
		std::stringstream ss;
		ss << scales[i];
		leg->AddEntry(gae, ss.str().c_str(), "l");
	}

	mg->Draw("AP");
	mg->GetXaxis()->SetTitle(first->GetXaxis()->GetTitle());
	mg->GetYaxis()->SetTitle(first->GetYaxis()->GetTitle());
	leg->Draw();
	for (unsigned int i = 0; i < labels.size(); i++) {
		labels[i]->Draw();
	}
}

TGraphAsymmErrors* PndLmdResultPlotter::makeXYOverviewGraph(
		std::vector<PndLmdVertexData> &vertex_data, double error_scaling_factor,
		int color) {
	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> x_err_low;
	std::vector<double> x_err_high;
	std::vector<double> y_err_low;
	std::vector<double> y_err_high;

	double current_x;
	double current_y;
	double current_x_err_low;
	double current_x_err_high;
	double current_y_err_low;
	double current_y_err_high;

	double max_offset = 0.4;

	// first group all vertex data plots of some specific ip property setting together
	std::map<LumiFit::LmdSimIPParameters, std::vector<PndLmdVertexData> > sim_settings_ip_data_map;
	for (unsigned int i = 0; i < vertex_data.size(); i++) {
		sim_settings_ip_data_map[vertex_data[i].getSimulationIPParameters()].push_back(
				vertex_data[i]);
	}

	std::map<LumiFit::LmdSimIPParameters, std::vector<PndLmdVertexData> >::iterator ip_setting_case;
	for (ip_setting_case = sim_settings_ip_data_map.begin();
			ip_setting_case != sim_settings_ip_data_map.end(); ip_setting_case++) {
		current_x = ip_setting_case->first.offset_x_mean;
		current_y = ip_setting_case->first.offset_y_mean;
		current_x_err_low = 0.0;
		current_x_err_high = 0.0;
		current_y_err_low = 0.0;
		current_y_err_high = 0.0;

		std::cout << "vertex data count for " << ip_setting_case->first.getLabel()
				<< ": " << ip_setting_case->second.size() << std::endl;

		for (unsigned int i = 0; i < ip_setting_case->second.size(); i++) {
			if (reco_dimension_compare(ip_setting_case->second[i])) {
				if (ip_setting_case->second[i].getPrimaryDimension().dimension_options.dimension_type
						== LumiFit::X) {
					PndLmdLumiFitResult *fit_result =
							ip_setting_case->second[i].getFitResults().begin()->second;

					if (fit_result) {
						double diff =
								error_scaling_factor
										* (fit_result->getModelFitResult().getFitParameter(
												"gauss_mean").value - current_x);
						double absdiff = fabs(diff);

						std::cout << diff << " " << absdiff << std::endl;
						//	if (absdiff > 0.1 * max_offset && absdiff < max_offset) {
						if (diff < 0.0)
							current_x_err_low = absdiff;
						else
							current_x_err_high = absdiff;
						//	}
					}
				} else if (ip_setting_case->second[i].getPrimaryDimension().dimension_options.dimension_type
						== LumiFit::Y) {
					PndLmdLumiFitResult *fit_result =
							ip_setting_case->second[i].getFitResults().begin()->second;

					if (fit_result) {
						double diff =
								error_scaling_factor
										* (fit_result->getModelFitResult().getFitParameter(
												"gauss_mean").value - current_y);
						double absdiff = fabs(diff);

						std::cout << diff << " " << absdiff << std::endl;
						//	if (absdiff > 0.1 * max_offset && absdiff < max_offset) {
						if (diff < 0.0)
							current_y_err_low = absdiff;
						else
							current_y_err_high = absdiff;
						//	}
					}
				}
			}
		}

		// add values to vector
		x.push_back(current_x);
		y.push_back(current_y);
		x_err_low.push_back(current_x_err_low);
		x_err_high.push_back(current_x_err_high);
		y_err_low.push_back(current_y_err_low);
		y_err_high.push_back(current_y_err_high);
	}

	TGraphAsymmErrors *graph = new TGraphAsymmErrors(x.size(), &x[0], &y[0],
			&x_err_low[0], &x_err_high[0], &y_err_low[0], &y_err_high[0]);
	graph->GetXaxis()->SetTitle("#mu_{x} [cm]");
	graph->GetYaxis()->SetTitle("#mu_{y} [cm]");
	graph->SetTitle("");
	graph->SetMarkerStyle(9);
	graph->SetLineColor(color);

	return graph;
}

void PndLmdResultPlotter::plotIPDependencyGraphs(
		std::map<LumiFit::LmdSimIPParameters,
				std::map<LumiFit::LmdSimIPParameters,
						std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > > &ip_data_clusters) {

	std::cout << "looping over clustered ip data map..." << std::endl;
	std::map<LumiFit::LmdSimIPParameters,
			std::map<LumiFit::LmdSimIPParameters,
					std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > >::iterator dependency_it;
	for (dependency_it = ip_data_clusters.begin();
			dependency_it != ip_data_clusters.end(); dependency_it++) {

		std::cout << "dependency: " << dependency_it->first.getDependencyLabel()
				<< std::endl;

		std::map<LumiFit::LmdSimIPParameters,
				std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > >::iterator ip_params_it;
		for (ip_params_it = dependency_it->second.begin();
				ip_params_it != dependency_it->second.end(); ip_params_it++) {

			std::cout << "remaining parameters: " << ip_params_it->first.getLabel()
					<< std::endl;

			std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> >::iterator dimension_type_it;
			for (dimension_type_it = ip_params_it->second.begin();
					dimension_type_it != ip_params_it->second.end();
					dimension_type_it++) {

				// get all mc objects
				std::vector<PndLmdVertexData> mc_ip_data;
				std::vector<PndLmdVertexData>::iterator start =
						dimension_type_it->second.begin();
				while (start != dimension_type_it->second.end()) {
					std::vector<PndLmdVertexData>::iterator it = std::find_if(start,
							dimension_type_it->second.end(), mc_dimension_compare);
					if (it != dimension_type_it->second.end()) {
						mc_ip_data.push_back(*it);
						it++;
					}
					start = it;
				}
				std::cout << "collected " << mc_ip_data.size()
						<< " mc ip data for these settings!" << std::endl;

				// get all reco objects
				std::vector<PndLmdVertexData> reco_ip_data;
				start = dimension_type_it->second.begin();
				while (start != dimension_type_it->second.end()) {
					std::vector<PndLmdVertexData>::iterator it = std::find_if(start,
							dimension_type_it->second.end(), reco_dimension_compare);
					if (it != dimension_type_it->second.end()) {
						reco_ip_data.push_back(*it);
						it++;
					}
					start = it;
				}
				std::cout << "collected " << reco_ip_data.size()
						<< " reco ip data for these settings!" << std::endl;

				std::stringstream dimension_str;
				if (dimension_type_it->first == LumiFit::X) {
					dimension_str << "x";
				} else if (dimension_type_it->first == LumiFit::Y) {
					dimension_str << "y";
				} else if (dimension_type_it->first == LumiFit::Z) {
					dimension_str << "z";
				}

				std::pair<TGraphErrors*, TGraphErrors*> mc_graph_pair =
						makeIPParameterDependencyGraphBundle(mc_ip_data,
								dependency_it->first, dimension_str.str());

				std::pair<TGraphErrors*, TGraphErrors*> reco_graph_pair =
						makeIPParameterDependencyGraphBundle(reco_ip_data,
								dependency_it->first, dimension_str.str());

				//combine reco and mc graph of each type
				std::vector<std::pair<TGraphErrors*, TString> > mean_graphs;
				mean_graphs.push_back(std::make_pair(mc_graph_pair.first, "MC"));
				mean_graphs.push_back(std::make_pair(reco_graph_pair.first, "Reco"));

				std::vector<std::pair<TGraphErrors*, TString> > width_graphs;
				width_graphs.push_back(std::make_pair(mc_graph_pair.second, "MC"));
				width_graphs.push_back(std::make_pair(reco_graph_pair.second, "Reco"));

				std::cout << "about to draw stuff!" << std::endl;
				//then generate new filename and save to pdf
				std::stringstream dir_path_str;
				dir_path_str << boost::filesystem::current_path().c_str() << "/";
				dir_path_str << "ip-tilt/"; // basedir
				dir_path_str << dependency_it->first.getDependencyLabel() << "/";
				dir_path_str << ip_params_it->first.getLabel();

				std::cout << "creating directory " << dir_path_str.str()
						<< " before saving..." << std::endl;

				const path dir_path(dir_path_str.str());
				if (!exists(dir_path)) {
					if (!boost::filesystem::create_directories(dir_path)) {
						std::cout << "could not create directories... exiting...";
						return;
					}
				}

				TCanvas c;
				drawMultipleGraphsOnCanvas(mean_graphs);
				std::stringstream file_url;
				file_url << dir_path_str.str() << "/" << dimension_str.str()
						<< "_mean.pdf";

				c.SaveAs(file_url.str().c_str());

				c.Clear();
				drawMultipleGraphsOnCanvas(width_graphs);
				file_url.str("");
				file_url << dir_path_str.str() << "/" << dimension_str.str()
						<< "_width.pdf";

				c.SaveAs(file_url.str().c_str());
			}
		}
	}
}
