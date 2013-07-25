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

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TLatex.h"
#include "TString.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLegend.h"
#include "TEfficiency.h"
#include "TExec.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TColor.h"
#include "TPaletteAxis.h"
#include <iosfwd>
#include <ostream>

using std::ostringstream;
using std::endl;

PndLmdResultPlotter::PndLmdResultPlotter() {
	text_font = 132;
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

std::vector<PndLmdData*> PndLmdResultPlotter::getDataFromPath(TString path) {
	std::vector<PndLmdData*> vec_lmd_data;

	TFile *f = new TFile(path + "/fit_results.root", "OPEN");

	TIter next(f->GetListOfKeys());
	TDirectory *dir;
	while ((dir = (TDirectory*) next())) {
		std::cout << "Found dir " << dir->GetName() << std::endl;
		TString tname(dir->GetName());
		PndLmdData *data;
		f->GetObject(tname + "/lmddata", data);
		if (data) {
			std::cout << "adding " << tname + "/lmddata" << std::endl;
			vec_lmd_data.push_back(data);
		}
	}
	return vec_lmd_data;
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
		return pow(
				10,
				gPad->GetUymin()
						+ (gPad->GetUymax() - gPad->GetUymin())
								* (text_toppos_ - line * text_spacing_));
	} else {
		return gPad->GetUymin()
				+ (gPad->GetUymax() - gPad->GetUymin())
						* (text_toppos_ - line * text_spacing_);
	}
}

std::pair<double, double> PndLmdResultPlotter::calculatePlotRange(
		PndLmdDataInterface *data, const PndLmdLumiFitOptions *fit_options) {
	double range_low;
	double range_high;
	if (fit_options->getModelBinaryOptions().isFitRaw()) {
		range_low = data->getTDimension().range_low;
		range_high = data->getTDimension().range_high;
		if (fit_options->getTFitRangeLow() > range_low)
			range_low = fit_options->getTFitRangeLow();
		if (fit_options->getTFitRangeHigh() < range_high)
			range_high = fit_options->getTFitRangeHigh();
	} else {
		range_low = data->getThetaDimension().range_low;
		range_high = data->getThetaDimension().range_high;
		if (fit_options->getThetaFitRangeLow() > range_low)
			range_low = fit_options->getThetaFitRangeLow();
		if (fit_options->getThetaFitRangeHigh() < range_high)
			range_high = fit_options->getThetaFitRangeHigh();
	}
	return std::make_pair(range_low, range_high);
}

TGraphErrors* PndLmdResultPlotter::createGraphFromFitResult(
		PndLmdLumiFitResult *fit_res, PndLmdAcceptance *acc, PndLmdData *data) {

	PndLmdModelFactory model_factory;
	shared_ptr<Model1D> model = model_factory.generate1DModel(
			fit_res->getLumiFitOptions(), data->getLabMomentum(), acc);
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			*fit_res->getModelFitResult());

	// ok just evaluate the function at 500 points in the range
	ROOTDataHelper data_helper;
	ModelVisualizationProperties1D vis_prop(
			data_helper.createBinnedData(
					data->getMeasuredHist1D(fit_res->getLumiFitOptions())));

	vis_prop.setBinningFactor(
			data->getBinningFactor(fit_res->getLumiFitOptions()));
	vis_prop.setPlotRange(calculatePlotRange(data, fit_res->getLumiFitOptions()));

	return root_plotter.createGraphFromModel1D(model, vis_prop);
}

TGraphErrors* PndLmdResultPlotter::createSmearingGraphFromFitResult(
		PndLmdLumiFitResult *fit_res, PndLmdResolution *res_data) {
	PndLmdModelFactory model_factory;
	shared_ptr<Model1D> model = model_factory.generate1DResolutionModel(
			fit_res->getLumiFitOptions());
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			*fit_res->getModelFitResult());

	// all parameters of the smearing model have to be freed otherwise the
	// parametrization model will overwrite the values during the evaluation
	model->getModelParameterSet().freeAllModelParameters();

	ROOTDataHelper data_helper;
	ModelVisualizationProperties1D vis_prop(
			data_helper.createBinnedData(res_data->getResolutionHistogram1D()));
	vis_prop.setPlotRange(
			std::make_pair(res_data->getThetaDimension().range_low,
					res_data->getThetaDimension().range_high));

	return root_plotter.createGraphFromModel1D(model, vis_prop);
}

TGraphErrors* createResidual(PndLmdLumiFitResult *fit_res,
		PndLmdAcceptance *acc, PndLmdData *data) {
	TH1D *data_hist = data->getMeasuredHist1D(fit_res->getLumiFitOptions());

	PndLmdModelFactory model_factory;

	shared_ptr<Model1D> model = model_factory.generate1DModel(
			fit_res->getLumiFitOptions(), data->getLabMomentum(), acc);
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			*fit_res->getModelFitResult());

	int counter = 0;
	double xvals[data_hist->GetNbinsX()];
	double yvals[data_hist->GetNbinsX()];
	double yerrs[data_hist->GetNbinsX()];

	for (unsigned int i = 0; i < data_hist->GetNbinsX(); i++) {
		double eval_point = data_hist->GetBinCenter(i);
		if (fit_res->getLumiFitOptions()->getModelBinaryOptions().isFitRaw()) {
			if (fit_res->getLumiFitOptions()->getTFitRangeLow() > eval_point
					|| fit_res->getLumiFitOptions()->getTFitRangeHigh() < eval_point) {
				continue;
			}
		} else {
			if (fit_res->getLumiFitOptions()->getThetaFitRangeLow() > eval_point
					|| fit_res->getLumiFitOptions()->getThetaFitRangeHigh() < eval_point) {
				continue;
			}
		}
		xvals[counter] = data_hist->GetBinCenter(i);
		yvals[counter] = data_hist->GetBinContent(i) - model->evaluate(&eval_point)*data->getBinningFactor(fit_res->getLumiFitOptions());
		yerrs[counter] = data_hist->GetBinError(i);
		counter++;
	}
	return new TGraphErrors(counter, xvals, yvals, 0, yerrs);
}

std::map<PndLmdLumiFitOptions,
		std::vector<PndLmdResultPlotter::graph_bundle_1D>,
		PndLmdResultPlotter::fit_options_compare> PndLmdResultPlotter::makeGraphBundles1D(
		PndLmdData *data, PndLmdAcceptance* acc) {
	std::map<PndLmdLumiFitOptions,
			std::vector<PndLmdResultPlotter::graph_bundle_1D>,
			PndLmdResultPlotter::fit_options_compare> return_map;

	std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >& fit_map =
			data->getFitMap();

	std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >::iterator iter =
			fit_map.find(acc);

	if (iter != fit_map.end()) {
		std::vector<PndLmdLumiFitResult*> fit_res = iter->second;

		double lumi_ref = data->getReferenceLuminosity();
		double momentum = data->getLabMomentum();

		for (unsigned int i = 0; i < fit_res.size(); i++) {
			graph_bundle_1D lmd_graph_bundle;
			lmd_graph_bundle.is_resolution = false;
			lmd_graph_bundle.plab = momentum;

			lmd_graph_bundle.fit_options = fit_res[i]->getLumiFitOptions();

			TH1D* hist = data->getMeasuredHist1D(fit_res[i]->getLumiFitOptions());

			hist->SetTitle("");
			hist->SetStats(0);

			TGraphErrors * model = createGraphFromFitResult(fit_res[i], acc, data);

			lmd_graph_bundle.data_hist = hist;
			lmd_graph_bundle.model = model;
			lmd_graph_bundle.residual = createResidual(fit_res[i], acc, data);

			ostringstream strstream;
			strstream.precision(2);

			strstream << "p_{lab} = " << lmd_graph_bundle.plab << " GeV";
			lmd_graph_bundle.labels.push_back(
					std::make_pair(TString(strstream.str()), 1));
			strstream.str("");

			strstream << "#chi^{2}/NDF = " << fit_res[i]->getRedChiSquare();
			lmd_graph_bundle.labels.push_back(
					std::make_pair(TString(strstream.str()), 1));
			strstream.str("");

			strstream << "lumi. diff. = " << fit_res[i]->getLuminosity() - lumi_ref
					<< " #pm "
					<< sqrt(pow(fit_res[i]->getLuminosityError(), 2.0) + pow(0.0, 2.0));
			lmd_graph_bundle.labels.push_back(
					std::make_pair(TString(strstream.str()), 1));
			strstream.str("");

			strstream
					<< "lumi. rel. diff. = "
					<< calulateLumiRelDiff(fit_res[i]->getLuminosity(),
							fit_res[i]->getLuminosityError(), lumi_ref).first
					<< " #pm "
					<< calulateLumiRelDiff(fit_res[i]->getLuminosity(),
							fit_res[i]->getLuminosityError(), lumi_ref).second << " %";
			lmd_graph_bundle.labels.push_back(
					std::make_pair(TString(strstream.str()), 2));
			strstream.str("");

			return_map[*(fit_res[i]->getLumiFitOptions())].push_back(
					lmd_graph_bundle);
		}
	}
	return return_map;
}

std::vector<PndLmdResultPlotter::graph_bundle_1D> PndLmdResultPlotter::makeResolutionGraphBundles1D(
		std::vector<PndLmdResolution*> &res_vec) {

	std::vector<PndLmdResultPlotter::graph_bundle_1D> return_vector;

	for (unsigned int i = 0; i < res_vec.size(); i++) {
		const std::set<PndLmdLumiFitResult*> &fit_set = res_vec[i]->getFitResults();
		std::cout << "number of fit results for " << res_vec[i]->getName() << ": "
				<< fit_set.size() << std::endl;
		if (fit_set.size() > 0) {
			graph_bundle_1D graph_bundle;
			graph_bundle.data_hist = res_vec[i]->getResolutionHistogram1D();
			graph_bundle.plab = res_vec[i]->getLabMomentum();

			for (std::set<PndLmdLumiFitResult*>::iterator fit_result =
					fit_set.begin(); fit_result != fit_set.end(); fit_result++) {
				graph_bundle.model = createSmearingGraphFromFitResult(*fit_result,
						res_vec[i]);

				graph_bundle.is_resolution = true;
				graph_bundle.fit_options = (*fit_result)->getLumiFitOptions();

				char cc[50];
				sprintf(cc, "p_{lab} = %.1f GeV", graph_bundle.plab);
				graph_bundle.labels.push_back(std::make_pair(TString(cc), 1));
				sprintf(cc, "#chi^{2}/NDF = %.2f", (*fit_result)->getRedChiSquare());
				graph_bundle.labels.push_back(std::make_pair(TString(cc), 1));
				sprintf(cc, "#bar{#Theta}_{MC} = %.2f mrad",
						res_vec[i]->getThetaSliceMean());
				graph_bundle.labels.push_back(std::make_pair(TString(cc), 1));
				if ((*fit_result)->getLumiFitOptions()->getSmearingModelType() == 1) {
					sprintf(
							cc,
							"#sigma_{n} = %.2f mrad",
							(*fit_result)->getModelFitResult()->getFitParameter(
									"gauss_sigma_narrow").value);
					graph_bundle.labels.push_back(std::make_pair(TString(cc), 1));
					sprintf(
							cc,
							"#sigma_{w} = %.2f mrad",
							(*fit_result)->getModelFitResult()->getFitParameter(
									"gauss_sigma_narrow").value
									/ (*fit_result)->getModelFitResult()->getFitParameter(
											"gauss_sigma_ratio_narrow_wide").value);
					graph_bundle.labels.push_back(std::make_pair(TString(cc), 1));
					sprintf(
							cc,
							"#mu_{n} = %.2f mrad",
							(*fit_result)->getModelFitResult()->getFitParameter(
									"gauss_mean_narrow").value);
					graph_bundle.labels.push_back(std::make_pair(TString(cc), 1));
					sprintf(
							cc,
							"#mu_{w} = %.2f mrad",
							(*fit_result)->getModelFitResult()->getFitParameter(
									"gauss_mean_wide").value);
					graph_bundle.labels.push_back(std::make_pair(TString(cc), 1));
					sprintf(
							cc,
							"r = %.2f mrad",
							(*fit_result)->getModelFitResult()->getFitParameter(
									"gauss_ratio_narrow_wide").value);
					graph_bundle.labels.push_back(std::make_pair(TString(cc), 1));
				}
			}
			std::cout << "adding " << res_vec[i]->getName() << " to graph bundle..."
					<< std::endl;
			return_vector.push_back(graph_bundle);
		}
	}
	return return_vector;
}

void PndLmdResultPlotter::fillSinglePad(TCanvas *c,
		graph_bundle_1D graph_bundle, bool residual, bool log_scale,
		bool labels_on) {
	if (c) {
		if (log_scale)
			gPad->SetLogy(1);
		else
			gPad->SetLogy(0);

		char ytitle[50];

		if (residual) {
			TGraphErrors* graph = graph_bundle.residual;
			graph->SetTitle("");

			if (graph_bundle.fit_options->getModelBinaryOptions().isFitRaw()) {
				if (theta_plot_range_low < theta_plot_range_high) {
					graph->GetXaxis()->SetRangeUser(
							-lumi_helper.getMomentumTransferFromTheta(graph_bundle.plab,
									theta_plot_range_low),
							-lumi_helper.getMomentumTransferFromTheta(graph_bundle.plab,
									theta_plot_range_high));
				}
				graph->GetXaxis()->SetTitle("t [GeV^{2}/c^{2}]");
			} else {
				if (theta_plot_range_low < theta_plot_range_high) {
					graph->GetXaxis()->SetRangeUser(theta_plot_range_low,
							theta_plot_range_high);
				}
				if (graph_bundle.is_resolution)
					graph->GetXaxis()->SetTitle("#Delta#Theta [mrad]");
				else
					graph->GetXaxis()->SetTitle("#Theta [mrad]");
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

			TLine *zero = new TLine(gPad->GetUxmin(), 0, gPad->GetUxmax(), 0);
			zero->SetLineColor(2);
			zero->Draw();

		} else {
			TH1D* hist = graph_bundle.data_hist;
			hist->SetTitle("");

			if (graph_bundle.fit_options->getModelBinaryOptions().isFitRaw()) {
				if (theta_plot_range_low < theta_plot_range_high) {
					hist->GetXaxis()->SetRangeUser(
							-lumi_helper.getMomentumTransferFromTheta(graph_bundle.plab,
									theta_plot_range_low),
							-lumi_helper.getMomentumTransferFromTheta(graph_bundle.plab,
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
				if (graph_bundle.is_resolution)
					hist->GetXaxis()->SetTitle("#Delta#Theta [mrad]");
				else
					hist->GetXaxis()->SetTitle("#Theta [mrad]");
				sprintf(ytitle, "# of events / %.3e mrad",
						hist->GetXaxis()->GetBinWidth(1));
			}

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

			TGraphErrors * model = graph_bundle.model;
			model->SetLineColor(2);
			model->SetMarkerColor(2);
			model->Draw("Csame");

			gPad->Update();

			if (labels_on) {
				for (unsigned int i = 0; i < graph_bundle.labels.size(); i++) {
					TLatex *label = new TLatex(text_leftpos * gPad->GetUxmax(),
							calculateYPos(1.0 * i, text_toppos, text_spacing, log_scale),
							graph_bundle.labels[i].first);
					label->SetTextSize(text_size);
					label->SetTextColor(graph_bundle.labels[i].second);
					label->Draw();
				}
			}
		}
	}
}

void PndLmdResultPlotter::fillAcceptanceInPad(acceptance_bundle acc_bundle) {
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

void PndLmdResultPlotter::fill2DAcceptanceInPad(acceptance_bundle acc_bundle) {
	TH2D *acceptance = acc_bundle.acceptance_2d;

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
		std::vector<PndLmdResultPlotter::graph_bundle_1D> &graph_bundles,
		acceptance_bundle &acc_bundle) {

	//c->Divide(3, 3);

	PndLmdLumiFitOptions fitop_tmctruth(8, 0, 0);
	PndLmdLumiFitOptions fitop_thmctruth(0, 0, 0);
	PndLmdLumiFitOptions fitop_mcacc(2, 0, 0);
	PndLmdLumiFitOptions fitop_normal(3, 0, 0);

	//draw raw fit first
	for (unsigned int i = 0; i < graph_bundles.size(); i++) {
		if (graph_bundles[i].fit_options->getModelBinaryOptions().getBinaryOptions()
				== fitop_tmctruth.getModelBinaryOptions().getBinaryOptions()) {
			c->cd(1);
			fillSinglePad(c, graph_bundles[i], false, 1);
		} else if (graph_bundles[i].fit_options->getModelBinaryOptions().getBinaryOptions()
				== fitop_thmctruth.getModelBinaryOptions().getBinaryOptions()) {
			c->cd(2);
			fillSinglePad(c, graph_bundles[i], false, 1);
		} else if (graph_bundles[i].fit_options->getModelBinaryOptions().getBinaryOptions()
				== fitop_mcacc.getModelBinaryOptions().getBinaryOptions()) {
			if (graph_bundles[i].fit_options->getDataBinaryOptions().getBinaryOptions()
					== fitop_mcacc.getModelBinaryOptions().getBinaryOptions()) {
				c->cd(4);
				fillSinglePad(c, graph_bundles[i], false, 0);
				c->cd(7);
				fillSinglePad(c, graph_bundles[i], true, 0); // draw residual
			} else if (graph_bundles[i].fit_options->getDataBinaryOptions().getBinaryOptions()
					== fitop_normal.getModelBinaryOptions().getBinaryOptions()) {
				c->cd(5);
				fillSinglePad(c, graph_bundles[i], false, 0);
			}
		} else if (graph_bundles[i].fit_options->getModelBinaryOptions().getBinaryOptions()
				== fitop_normal.getModelBinaryOptions().getBinaryOptions()) {
			c->cd(6);
			fillSinglePad(c, graph_bundles[i], false, 0);
			c->cd(9);
			fillSinglePad(c, graph_bundles[i], true, 0); // draw residual
		}
	}
	c->cd(3);
	fillAcceptanceInPad(acc_bundle);
	/*if (acc_bundle.is_angular) {
	 c->cd(6);
	 fill2DAcceptanceInPad(acc_bundle);
	 }*/
}

PndLmdResultPlotter::acceptance_bundle PndLmdResultPlotter::makeAcceptanceBundle(
		PndLmdAcceptance* acc, bool is_momentum_transfer) {
	acceptance_bundle acc_bundle;
	acc_bundle.plab = acc->getLabMomentum();

	TEfficiency *eff = acc->getAcceptance1D(is_momentum_transfer); // false = angular acceptance
	TCanvas c;
	eff->Draw();
	c.Update();
	acc_bundle.acceptance_1d = new TGraphAsymmErrors(*eff->GetPaintedGraph());
	if (!is_momentum_transfer) {
		eff = acc->getAcceptance2D();
		eff->Draw("COLZ");
		c.Update();
		acc_bundle.acceptance_2d = new TH2D(*(TH2D*) eff->GetPaintedHistogram());
	}
	acc_bundle.is_angular = !is_momentum_transfer;

	return acc_bundle;
}

std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> > PndLmdResultPlotter::generateLmdGraphMap(
		std::vector<PndLmdLumiHelper::lmd_graph*> graphs) {

	std::map<TString, std::vector<PndLmdLumiHelper::lmd_graph*> > return_map;

	for (unsigned int i = 0; i < graphs.size(); i++) {
		TString key(graphs[i]->dependency);
		for (std::map<std::string, double, ModelStructs::string_comp>::const_iterator dependency =
				graphs[i]->remaining_dependencies.begin();
				dependency != graphs[i]->remaining_dependencies.end(); dependency++) {

			ostringstream strstream;
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
		std::pair<double, double> plot_range, bool log_scale) {
	PndLmdModelFactory model_factory;

	ROOTDataHelper data_helper;
	ModelVisualizationProperties1D vis_prop;
	vis_prop.setPlotRange(plot_range);

	LumiFit::LmdBinaryFitOptions bit_fit_opt(0);
	PndLmdLumiFitOptions fit_op_full(bit_fit_opt, 0, 0);

	shared_ptr<Model1D> full_model = model_factory.generate1DModel(&fit_op_full,
			plab);
	if (full_model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphErrors* full_model_graph = root_plotter.createGraphFromModel1D(
			full_model, vis_prop);

	PndLmdLumiFitOptions fit_op_coul = fit_op_full;
	fit_op_coul.setDpmElasticModelParts(1);

	shared_ptr<Model1D> coul_model = model_factory.generate1DModel(&fit_op_coul,
			plab);
	if (full_model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphErrors* coul_model_graph = root_plotter.createGraphFromModel1D(
			coul_model, vis_prop);

	PndLmdLumiFitOptions fit_op_int = fit_op_full;
	fit_op_int.setDpmElasticModelParts(2);

	shared_ptr<Model1D> int_model = model_factory.generate1DModel(&fit_op_int,
			plab);
	if (full_model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphErrors* int_model_graph = root_plotter.createGraphFromModel1D(int_model,
			vis_prop);

	PndLmdLumiFitOptions fit_op_had = fit_op_full;
	fit_op_had.setDpmElasticModelParts(3);

	shared_ptr<Model1D> had_model = model_factory.generate1DModel(&fit_op_had,
			plab);
	if (full_model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphErrors* had_model_graph = root_plotter.createGraphFromModel1D(had_model,
			vis_prop);

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

	ostringstream strstream;
	strstream.precision(3);

	strstream << "DPMModels_" << plab << ".pdf";
	c.SaveAs(strstream.str().c_str());
}

void PndLmdResultPlotter::makeFitResultBooky(
		std::map<PndLmdLumiFitOptions,
				std::vector<PndLmdResultPlotter::graph_bundle_1D>,
				PndLmdResultPlotter::fit_options_compare> &graph_bundle_map
		, acceptance_bundle &acc_bundle , TString filename) {
	TCanvas c("fit_overview", "fit_overview", 1000, 700);

	c.Divide(3, 3);

	c.Print(filename + "_overview.pdf["); // No actual print, just open file
	for (std::map<PndLmdLumiFitOptions,
			std::vector<PndLmdResultPlotter::graph_bundle_1D>,
			PndLmdResultPlotter::fit_options_compare>::iterator i =
			graph_bundle_map.begin(); i != graph_bundle_map.end(); i++) {
		fillOverviewCanvas(&c, i->second, acc_bundle);
		c.cd();
		c.Print(filename + "_overview.pdf"); // actually print canvas to file
	}
	c.Print(filename + "_overview.pdf]");

}

void PndLmdResultPlotter::makeResolutionSummaryPlots(TFile *f) {
	PndLmdLumiHelper lmd_helper;

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
			if (it->second[0]->fit_options->getSmearingModelType() == 0)
				c.Divide(2, 2);
			if (it->second[0]->fit_options->getSmearingModelType() == 1)
				c.Divide(3, 3);
		}
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
					it->second[i]->fit_options);
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
				std::cout << it->second[i]->fit_result->getChiSquare() << std::endl;
				model->getModelParameterHandler().initModelParametersFromFitResult(
						*it->second[i]->fit_result);
				model->getModelParameterSet().printInfo();

				ROOTDataHelper data_helper;

				ModelVisualizationProperties1D vis_prop(
						data_helper.createBinnedData(it->second[i]->graph));
				vis_prop.setPlotRange(std::make_pair(4.0, 7.2));
				TGraphErrors *model_graph = root_plotter.createGraphFromModel1D(model,
						vis_prop);
				model_graph->SetLineColor(2);
				model_graph->Draw("CSAME");
			}
		}
		c.cd();
		TLatex title(0.2, 0.33, it->first);
		title.SetTextSize(0.05);
		title.Draw();
		c.SaveAs(TString("resolution_parameters_") + it->first + ".pdf");
	}
}

std::map<std::string, std::vector<PndLmdResolution*>, ModelStructs::string_comp> PndLmdResultPlotter::createBookyMap(
		std::vector<PndLmdResolution*> &res_vec) {
	std::map<std::string, std::vector<PndLmdResolution*>,
			ModelStructs::string_comp> return_map;

	for (unsigned int i = 0; i < res_vec.size(); i++) {
		res_vec[i]->getThetaSliceMean();
		ostringstream strstrm;
		strstrm.precision(3);
//		strstrm << "theta_" << res_vec[i]->getThetaSliceMean() << endl;
//		return_map[strstrm.str()].push_back(res_vec[i]);
//		std::cout << strstrm.str() << endl;
		strstrm << "phi_" << res_vec[i]->getPhiSliceMean() << endl;
		return_map[strstrm.str()].push_back(res_vec[i]);
//		std::cout << strstrm.str() << endl;
	}
	return return_map;
}

void PndLmdResultPlotter::makeResolutionBooky(
		std::vector<PndLmdResolution*> &res_vec, TString filename) {

	std::map<std::string, std::vector<PndLmdResolution*>,
			ModelStructs::string_comp> res_map = createBookyMap(res_vec);

	for (std::map<std::string, std::vector<PndLmdResolution*>,
			ModelStructs::string_comp>::iterator it = res_map.begin();
			it != res_map.end(); it++) {

		std::vector<PndLmdResultPlotter::graph_bundle_1D> graph_bundles =
				makeResolutionGraphBundles1D(it->second);

		TCanvas c("res_overview", "res_overview", 1000, 700);
		c.Divide(4, 3);
		unsigned int stepsize = 1;
		c.Print(filename + "_overview_" + TString(it->first) + ".pdf["); // No actual print, just open file
		for (unsigned int i = 0; i < graph_bundles.size(); i = i + stepsize) {
			c.cd(i / stepsize % 12 + 1);
			fillSinglePad(&c, graph_bundles[i], false, true);
			if (i / stepsize % 12 + 1 == 12 || i >= graph_bundles.size() - 1) {
				if (i >= graph_bundles.size() - 1) {
					// clear remaining pads
					for (unsigned int temp = i / stepsize % 12 + 2; temp <= 12; temp++) {
						c.cd(temp)->Clear();
					}
				}
				c.cd();
				TLatex title(0.25, 0.33, it->first.c_str());
				title.SetTextSize(0.05);
				title.Draw();
				c.Print(filename + "_overview_" + TString(it->first) + ".pdf"); // actually print canvas to file
			}
		}
		c.Print(filename + "_overview_" + TString(it->first) + ".pdf]");

		TCanvas c1("res_booky", "res_booky", 1000, 700);
		c1.SetTitle(it->first.c_str());
		c1.Print(filename + "_booky_" + TString(it->first) + ".pdf["); // No actual print, just open file
		std::cout << graph_bundles.size() << std::endl;
		for (unsigned int i = 0; i < graph_bundles.size(); i++) {
			fillSinglePad(&c1, graph_bundles[i], false, true);
			c1.cd();
			/*TLatex title(0.2, 0.5, it->first.c_str());
			 title.SetTextSize(0.05);
			 title.Draw();*/
			c1.Print(filename + "_booky_" + TString(it->first) + ".pdf"); // actually print canvas to file
		}
		c1.Print(filename + "_booky_" + TString(it->first) + ".pdf]");
	}
}

/*
 void make2DExampleOverviewCanvas(TString name, PndLmdData *data,
 PndLmdAcceptance *acc, PndLmdLumiFitOptions *opt) {

 // ok before we continue check that this fit is actually 2d otherwise return
 if (opt->getFitDimension() == 1)
 return;

 // load color palette stuff
 gROOT->Macro("~/.rootlogon.C");

 gStyle->SetPadBorderMode(0);
 gStyle->SetOptStat(0);
 gStyle->SetPadTopMargin(0.04);
 gStyle->SetPadBottomMargin(0.085);
 gStyle->SetPadLeftMargin(0.08);
 gStyle->SetPadRightMargin(0.012);

 int numbercolors = 41;

 // fit corresponding fit result
 PndLmdLumiFitResult* fit_res = data->getFitResult(acc, opt);

 // now make a histogram from our tf2
 TH2D* fitfunc_hist = makeHistogramFromFunction(
 fit_res->getROOTFitFunction2D(), data);

 // determine max value
 if (fitfunc_hist->GetMaximum() > data->getMeasuredHist2D(opt)->GetMaximum())
 data->getMeasuredHist2D(opt)->SetMaximum(fitfunc_hist->GetMaximum());
 else
 fitfunc_hist->SetMaximum(data->getMeasuredHist2D(opt)->GetMaximum());

 TString s("setDiffColorStyle(");
 s += numbercolors;
 s += ");";

 TCanvas *c = new TCanvas("c", "c", 1000, 700);
 c->Divide(3, 2);

 TCanvas *seperate = new TCanvas("csep", "csep", 1000, 700);

 int colors[] = { 8, 1, 2, 9, 3, 4, 5 };
 TLegend* leg = new TLegend(0.70, 0.85 - 0.05 * 3, .9, .9);

 std::vector<PndLmdLumiFitResult*> fit_res_vec = data->getFitResults(acc);

 TGaxis::SetMaxDigits(3);
 gStyle->SetTextFont(132);
 double text_size = 0.055;
 double text_xpos = 0.1;
 double top = 0.7;
 double spacing = 0.06;

 double lumi_ref = 0.0;
 double lumi_err_ref = 0.0;

 // ok to determine the reference luminosity value we take the 1d mc truth t fit (in case this is not found no references will be available)
 PndLmdLumiFitOptions fitop_tmctruth(10000, 0.0, 0.0, 0.0); // t 1d

 //draw raw fit first
 for (int i = 0; i < fit_res_vec.size(); i++) {
 if (fit_res_vec[i]->getLumiFitOptions()->getMode()
 == fitop_tmctruth.getMode()) {
 lumi_ref = fit_res_vec[i]->getLuminosity();
 lumi_err_ref = fit_res_vec[i]->getLuminosityError();
 }
 }

 // data in first subpad
 c->cd(1);
 gPad->SetRightMargin(1.0);
 TH2D* hist = data->getMeasuredHist2D(opt);
 hist->SetTitle("");
 hist->GetXaxis()->SetTitle("#Theta [mrad]");
 hist->GetYaxis()->SetTitle("#Phi [rad]");
 hist->GetYaxis()->SetLabelOffset(0.007);
 hist->GetXaxis()->SetLabelOffset(0.007);
 hist->GetYaxis()->SetLabelSize(0.035);
 hist->GetXaxis()->SetLabelSize(0.035);
 hist->GetYaxis()->SetNoExponent(false);
 hist->GetYaxis()->SetTitleOffset(1.05);
 hist->GetYaxis()->SetTitleSize(0.04);
 hist->GetXaxis()->SetTitleSize(0.04);
 hist->Draw("COLZ");
 hist->SetStats(0);
 hist->SetContour(numbercolors);
 //TExec *ex1 = new TExec("ex1", "setNormalColorStyle();");
 TExec *ex1 = new TExec("ex1", "gStyle->SetPalette(1);");
 ex1->Draw();
 hist->Draw("colz same");

 seperate->cd();
 seperate->SetRightMargin(1.2);
 hist->Draw("COLZ");
 seperate->SaveAs("fit_overview_2d_data.pdf");

 //draw fit function histogram in second subpad
 c->cd(2);
 gPad->SetRightMargin(1.0);
 fitfunc_hist->SetTitle("");
 fitfunc_hist->GetXaxis()->SetTitle("#Theta [mrad]");
 fitfunc_hist->GetYaxis()->SetTitle("#Phi [rad]");
 fitfunc_hist->GetYaxis()->SetLabelOffset(0.007);
 fitfunc_hist->GetXaxis()->SetLabelOffset(0.007);
 fitfunc_hist->GetYaxis()->SetLabelSize(0.035);
 fitfunc_hist->GetXaxis()->SetLabelSize(0.035);
 fitfunc_hist->GetYaxis()->SetNoExponent(false);
 fitfunc_hist->GetYaxis()->SetTitleOffset(1.05);
 fitfunc_hist->GetYaxis()->SetTitleSize(0.04);
 fitfunc_hist->GetXaxis()->SetTitleSize(0.04);
 fitfunc_hist->Draw("COLZ");
 fitfunc_hist->SetStats(0);
 fitfunc_hist->SetContour(numbercolors);
 //TExec *ex1 = new TExec("ex1", "setNormalColorStyle();");
 TExec *ex2 = new TExec("ex2", "gStyle->SetPalette(1);");
 ex2->Draw();
 fitfunc_hist->Draw("colz same");

 seperate->cd();
 seperate->SetRightMargin(1.2);
 fitfunc_hist->Draw("COLZ");
 seperate->SaveAs("fit_overview_2d_fitfunc.pdf");

 c->cd(3);
 gPad->SetRightMargin(1.0);
 TH2D* diffhist = new TH2D(*hist);
 diffhist->Add(fitfunc_hist, -1.0);
 diffhist->SetTitle("");
 diffhist->GetXaxis()->SetTitle("#Theta [mrad]");
 diffhist->GetYaxis()->SetTitle("#Phi [rad]");
 diffhist->GetYaxis()->SetLabelOffset(0.007);
 diffhist->GetXaxis()->SetLabelOffset(0.007);
 diffhist->GetYaxis()->SetLabelSize(0.035);
 diffhist->GetXaxis()->SetLabelSize(0.035);
 diffhist->GetYaxis()->SetNoExponent(false);
 diffhist->GetYaxis()->SetTitleOffset(1.05);
 diffhist->GetYaxis()->SetTitleSize(0.04);
 diffhist->GetXaxis()->SetTitleSize(0.04);
 double max = diffhist->GetMaximum();
 if (-diffhist->GetMinimum() > max)
 max = -diffhist->GetMinimum();
 diffhist->GetZaxis()->SetRangeUser(-max, max);
 diffhist->Draw("COLZ");
 diffhist->SetStats(0);
 diffhist->SetContour(numbercolors);
 TExec *ex3 = new TExec("ex3", s);
 ex3->Draw();
 diffhist->Draw("colz same");
 //TPaletteAxis *palette = (TPaletteAxis*)diffhist->GetListOfFunctions()->FindObject("palette");
 //palette->SetY2NDC(0.7);

 seperate->cd();
 seperate->SetRightMargin(1.2);
 diffhist->Draw("COLZ");
 ex3->Draw();
 diffhist->Draw("colz same");
 seperate->SaveAs("fit_overview_2d_diff.pdf");

 c->cd(4);
 gPad->SetRightMargin(1.0);
 TH2D* reldiffhist = new TH2D(*diffhist);
 TH2D* denominator = new TH2D(*hist);
 denominator->Add(hist, fitfunc_hist, 0.5, 0.5);
 reldiffhist->Divide(denominator);
 reldiffhist->SetTitle("");
 reldiffhist->GetXaxis()->SetTitle("#Theta [mrad]");
 reldiffhist->GetYaxis()->SetTitle("#Phi [rad]");
 reldiffhist->GetYaxis()->SetLabelOffset(0.007);
 reldiffhist->GetXaxis()->SetLabelOffset(0.007);
 reldiffhist->GetYaxis()->SetLabelSize(0.035);
 reldiffhist->GetXaxis()->SetLabelSize(0.035);
 reldiffhist->GetYaxis()->SetNoExponent(false);
 reldiffhist->GetYaxis()->SetTitleOffset(1.05);
 reldiffhist->GetYaxis()->SetTitleSize(0.04);
 reldiffhist->GetXaxis()->SetTitleSize(0.04);
 max = reldiffhist->GetZaxis()->GetXmax();
 if (-reldiffhist->GetZaxis()->GetXmin() > max)
 max = -reldiffhist->GetZaxis()->GetXmin();
 std::cout << max << std::endl;
 reldiffhist->GetZaxis()->SetRangeUser(-max, max);
 reldiffhist->Draw("COLZ");
 reldiffhist->SetStats(0);
 reldiffhist->SetContour(numbercolors);
 TExec *ex4 = new TExec("ex4", s);
 ex4->Draw();
 reldiffhist->Draw("colz same");
 //TPaletteAxis *palette = (TPaletteAxis*)diffhist->GetListOfFunctions()->FindObject("palette");
 //palette->SetY2NDC(0.7);

 seperate->cd();
 seperate->SetRightMargin(1.2);
 reldiffhist->Draw("COLZ");
 ex3->Draw();
 diffhist->Draw("colz same");
 seperate->SaveAs("fit_overview_2d_reldiff.pdf");

 c->cd(5);
 gPad->SetRightMargin(1.0);
 TEfficiency *eff = acc->getAcceptance2D();
 eff->Draw("COLZ");
 c->Update();
 eff->GetPaintedHistogram()->GetXaxis()->SetTitle("#Theta [mrad]");
 eff->GetPaintedHistogram()->GetXaxis()->SetTitleSize(0.04);
 eff->GetPaintedHistogram()->GetYaxis()->SetTitle("#Phi [rad]");
 eff->GetPaintedHistogram()->GetYaxis()->SetTitleSize(0.04);
 eff->GetPaintedHistogram()->GetXaxis()->SetRangeUser(data->getThRangeLow(),
 data->getThRangeHigh());
 eff->GetPaintedHistogram()->SetContour(numbercolors);
 //TExec *ex1 = new TExec("ex1", "setNormalColorStyle();");
 TExec *ex5 = new TExec("ex5", "gStyle->SetPalette(1);");
 ex2->Draw();
 eff->GetPaintedHistogram()->Draw("colz same");

 seperate->cd();
 eff->Draw("AP");
 seperate->SaveAs("acceptance_2d.pdf");

 c->Update();

 c->cd(6);
 TLatex *labeld = new TLatex(text_xpos * gPad->GetUxmax(),
 gPad->GetUymax() * (top), "data: accepted mc truth");
 TLatex *labelf = new TLatex(text_xpos * gPad->GetUxmax(),
 gPad->GetUymax() * (top - 1.0 * spacing), "fit: model + acc. corr.");
 char cc[30];
 sprintf(
 cc,
 "#chi^{2}/NDF = %.2f",
 fit_res->getROOTFitFunction2D()->GetChisquare()
 / fit_res->getROOTFitFunction2D()->GetNDF());
 TLatex *labelchi2 = new TLatex(text_xpos * gPad->GetUxmax(),
 gPad->GetUymax() * (top - 2.0 * spacing), cc);

 double lumi = fit_res->getLuminosity();
 double lumi_err = fit_res->getLuminosityError();

 char cld[30];
 sprintf(cld, "lumi. diff. = %.2f #pm %.3f", lumi - lumi_ref,
 sqrt(pow(lumi_err, 2.0) + pow(lumi_err_ref, 2.0)));
 TLatex *labellumidiff = new TLatex(text_xpos * gPad->GetUxmax(),
 gPad->GetUymax() * (top - 3.0 * spacing), cld);
 char clrd[30];
 sprintf(
 clrd,
 "lumi. rel. diff. = %.2f #pm %.2f %%",
 100.0 * (lumi - lumi_ref) / lumi_ref,
 100.0
 * sqrt(
 pow(lumi_err / lumi_ref, 2.0)
 + pow(lumi_err_ref * lumi_err / lumi_ref / lumi_ref, 2.0)));
 TLatex *labellumireldiff = new TLatex(text_xpos * gPad->GetUxmax(),
 gPad->GetUymax() * (top - 4.0 * spacing), clrd);

 char ct[30];
 sprintf(ct, "acceptance # of bins = %i", acc->getThBins());
 TLatex *labelacc = new TLatex(text_xpos * gPad->GetUxmax(),
 gPad->GetUymax() * (top - 5.0 * spacing), ct);
 labelacc->SetTextSize(text_size);
 labelacc->Draw();

 labeld->SetTextSize(text_size);
 labelf->SetTextSize(text_size);
 labelchi2->SetTextSize(text_size);
 labellumidiff->SetTextSize(text_size);
 labellumireldiff->SetTextSize(text_size);
 labellumireldiff->SetTextColor(2);

 labeld->Draw();
 labelf->Draw();
 labelchi2->Draw();
 labellumidiff->Draw();
 labellumireldiff->Draw();

 c->SaveAs("fit_overviews_2d.pdf");

 delete (c);
 }*/
