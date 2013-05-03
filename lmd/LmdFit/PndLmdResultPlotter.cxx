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

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TH1D.h"
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

PndLmdResultPlotter::PndLmdResultPlotter() {
	// TODO Auto-generated constructor stub

	gStyle->SetPadBorderMode(0);
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);
	gStyle->SetPadTopMargin(0.06);
	gStyle->SetPadBottomMargin(0.12);
	gStyle->SetPadLeftMargin(0.14);
	gStyle->SetPadRightMargin(0.1);
	gStyle->SetTextFont(132);
	TGaxis::SetMaxDigits(3);

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

void PndLmdResultPlotter::unifyPlotRange(TH1D* hist, TGraph *graph) {
	double hist_x_min, hist_x_max;
	double graph_x_min, graph_x_max;

	hist_x_min = hist->GetXaxis()->GetXmin();
	hist_x_max = hist->GetXaxis()->GetXmax();

	graph_x_min = graph->GetXaxis()->GetXmin();
	graph_x_max = graph->GetXaxis()->GetXmax();

	if (hist_x_min < graph_x_min) {
		hist->GetXaxis()->SetRangeUser(graph_x_min, hist_x_max);
		hist_x_min = graph_x_min;
	} else if (hist_x_min > graph_x_min) {
		graph->GetXaxis()->SetRangeUser(hist_x_min, graph_x_max);
		graph_x_min = hist_x_min;
	}

	if (hist_x_max > graph_x_max) {
		hist->GetXaxis()->SetRangeUser(hist_x_min, graph_x_max);
		hist_x_max = graph_x_max;
	} else if (hist_x_max < graph_x_max) {
		graph->GetXaxis()->SetRangeUser(graph_x_min, hist_x_max);
		graph_x_max = hist_x_max;
	}
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
	return std::make_pair(100.0 * (lumi - lumi_ref) / lumi,
			100.0 * lumi_err / lumi);
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
		PndLmdData *data, PndLmdLumiFitOptions *fit_options) {
	double range_low;
	double range_high;
	if (fit_options->isFitRaw()) {
		range_low = data->getTRangeLow();
		range_high = data->getTRangeHigh();
		if (fit_options->getTFitRangeLow() > range_low)
			range_low = fit_options->getTFitRangeLow();
		if (fit_options->getTFitRangeHigh() < range_high)
			range_high = fit_options->getTFitRangeHigh();
	} else {
		range_low = data->getThRangeLow();
		range_high = data->getThRangeHigh();
		if (fit_options->getThetaFitRangeLow() > range_low)
			range_low = fit_options->getThetaFitRangeLow();
		if (fit_options->getThetaFitRangeHigh() < range_high)
			range_high = fit_options->getThetaFitRangeHigh();
	}
	return std::make_pair(range_low, range_high);
}

TGraphErrors* PndLmdResultPlotter::createGraphFromFitResult(
		PndLmdLumiFitResult *fit_res, PndLmdAcceptance *acc, PndLmdData *data) {
	unsigned int num_evaluations = 500;

	TGraphErrors* graph = new TGraphErrors(num_evaluations);

	PndLmdModelFactory model_factory;
	shared_ptr<Model1D> model = model_factory.generate1DModel(
			fit_res->getLumiFitOptions(), data->getLabMomentum(), acc);
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	// ok just evaluate the function at 500 points in the range
	std::pair<double, double> plot_range = calculatePlotRange(data,
			fit_res->getLumiFitOptions());

	double stepsize = (plot_range.second - plot_range.first) / num_evaluations;
	double x;
	for (unsigned int i = 0; i < num_evaluations; i++) {
		x = plot_range.first + stepsize * i;
		graph->SetPoint(
				i,
				x,
				fit_res->getLuminosity() * model->evaluate(&x)
						* data->getBinningFactor(fit_res->getLumiFitOptions()));
		graph->SetPointError(i, 0, 0);
	}
	return graph;
}

std::vector<PndLmdResultPlotter::graph_bundle_1D> PndLmdResultPlotter::makeGraphBundles1D(
		PndLmdData *data, PndLmdAcceptance* acc) {
	std::vector<PndLmdResultPlotter::graph_bundle_1D> return_vector;

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
			lmd_graph_bundle.plab = momentum;
			lmd_graph_bundle.lumi_values.lumiref = lumi_ref;
			lmd_graph_bundle.fit_options = fit_res[i]->getLumiFitOptions();

			TH1D* hist = data->getMeasuredHist1D(fit_res[i]->getLumiFitOptions());
			hist->SetTitle("");
			hist->SetStats(0);

			TGraphErrors * model = createGraphFromFitResult(fit_res[i], acc, data);

			lmd_graph_bundle.data_hist = hist;
			lmd_graph_bundle.model = model;

			lmd_graph_bundle.chi2 = fit_res[i]->getChiSquare() / fit_res[i]->getNDF();
			lmd_graph_bundle.lumi_values.lumival = fit_res[i]->getLuminosity();
			lmd_graph_bundle.lumi_values.lumierr = fit_res[i]->getLuminosityError();

			return_vector.push_back(lmd_graph_bundle);
		}
	}
	return return_vector;
}

void PndLmdResultPlotter::fillSinglePad(TCanvas *c,
		graph_bundle_1D graph_bundle, bool log_scale, bool labels_on) {
	if (c) {
		if (log_scale)
			gPad->SetLogy(1);
		else
			gPad->SetLogy(0);

		TH1D* hist = graph_bundle.data_hist;
		char ytitle[50];
		hist->SetTitle("");
		if (graph_bundle.fit_options->isFitRaw()) {
			if (theta_plot_range_low < theta_plot_range_high) {
				PndLmdData tempdata;
				tempdata.setLabMomentum(graph_bundle.plab);
				hist->GetXaxis()->SetRangeUser(
						-tempdata.getMomentumTransferFromTheta(theta_plot_range_low),
						-tempdata.getMomentumTransferFromTheta(theta_plot_range_high));
			}
			hist->GetXaxis()->SetTitle("t [GeV^{2}/c^{2}]");
			sprintf(ytitle, "# of events / %.3e GeV^{2}/c^{2}",
					hist->GetXaxis()->GetBinWidth(1));

		} else {
			if (theta_plot_range_low < theta_plot_range_high) {
				hist->GetXaxis()->SetRangeUser(theta_plot_range_low,
						theta_plot_range_high);
			}
			hist->GetXaxis()->SetTitle("#Theta [mrad]");
			sprintf(ytitle, "# of events / %.3e mrad",
					hist->GetXaxis()->GetBinWidth(1));
		}
		hist->GetYaxis()->SetTitle(ytitle);

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
			char cmom[30];
			sprintf(cmom, "p_{lab} = %.1f GeV", graph_bundle.plab);
			TLatex *momlabel = new TLatex(text_leftpos * gPad->GetUxmax(),
					calculateYPos(1.0, text_toppos, text_spacing, log_scale), cmom);

			char cc[30];
			sprintf(cc, "#chi^{2}/NDF = %.2f", graph_bundle.chi2);
			TLatex *labelchi2 = new TLatex(text_leftpos * gPad->GetUxmax(),
					calculateYPos(2.0, text_toppos, text_spacing, log_scale), cc);

			double lumi = graph_bundle.lumi_values.lumival;
			double lumi_err = graph_bundle.lumi_values.lumierr;
			double lumi_ref = graph_bundle.lumi_values.lumiref;

			char cld[30];
			sprintf(cld, "lumi. diff. = %.2f #pm %.2f", lumi - lumi_ref,
					sqrt(pow(lumi_err, 2.0) + pow(0.0, 2.0)));
			TLatex *labellumidiff = new TLatex(text_leftpos * gPad->GetUxmax(),
					calculateYPos(3.0, text_toppos, text_spacing, log_scale), cld);

			char *clrd = new char[30];
			sprintf(clrd, "lumi. rel. diff. = %.2f #pm %.2f %%",
					calulateLumiRelDiff(lumi, lumi_err, lumi_ref).first,
					calulateLumiRelDiff(lumi, lumi_err, lumi_ref).second);
			TLatex *labellumireldiff = new TLatex(text_leftpos * gPad->GetUxmax(),
					calculateYPos(4.0, text_toppos, text_spacing, log_scale), clrd);

			momlabel->SetTextSize(text_size);
			labelchi2->SetTextSize(text_size);
			labellumidiff->SetTextSize(text_size);
			labellumireldiff->SetTextSize(text_size);
			labellumireldiff->SetTextColor(2);

			momlabel->Draw();
			labelchi2->Draw();
			labellumidiff->Draw();
			labellumireldiff->Draw();
		}
	}
}

void PndLmdResultPlotter::fillAcceptanceInPad(
		acceptance_bundle_1D acceptance_bundle) {
	TGraphAsymmErrors *acceptance = acceptance_bundle.acceptance;

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

TCanvas* PndLmdResultPlotter::makeOverviewCanvas(
		std::vector<PndLmdResultPlotter::graph_bundle_1D> &graph_bundles,
		acceptance_bundle_1D &acceptance_bundle) {
	TCanvas *c = new TCanvas("c", "c", 1000, 700);
	c->Divide(3, 2);

	PndLmdLumiFitOptions fitop_tmctruth(8, 0, 0);
	PndLmdLumiFitOptions fitop_thmctruth(0, 0, 0);
	PndLmdLumiFitOptions fitop_mcacc(2, 0, 0);
	PndLmdLumiFitOptions fitop_normal(3, 0, 0);

	//draw raw fit first
	for (unsigned int i = 0; i < graph_bundles.size(); i++) {
		if (graph_bundles[i].fit_options->getBinaryOptions()
				== fitop_tmctruth.getBinaryOptions()) {
			c->cd(1);
			fillSinglePad(c, graph_bundles[i], 1);
		} else if (graph_bundles[i].fit_options->getBinaryOptions()
				== fitop_thmctruth.getBinaryOptions()) {
			c->cd(2);
			fillSinglePad(c, graph_bundles[i], 1);
		} else if (graph_bundles[i].fit_options->getBinaryOptions()
				== fitop_mcacc.getBinaryOptions()) {
			c->cd(3);
			fillSinglePad(c, graph_bundles[i], 0);
		} else if (graph_bundles[i].fit_options->getBinaryOptions()
				== fitop_normal.getBinaryOptions()) {
			c->cd(4);
			fillSinglePad(c, graph_bundles[i], 0);
		}
	}
	c->cd(5);
	fillAcceptanceInPad(acceptance_bundle);
	return c;
}

PndLmdResultPlotter::acceptance_bundle_1D PndLmdResultPlotter::makeAcceptanceBundle1D(
		PndLmdAcceptance* acc, bool is_momentum_transfer) {
	acceptance_bundle_1D acceptance_bundle;
	acceptance_bundle.plab = acc->getLabMomentum();

	TEfficiency *eff = acc->getAcceptance1D(is_momentum_transfer); // false = angular acceptance
	TCanvas c;
	eff->Draw();
	c.Update();
	acceptance_bundle.acceptance = new TGraphAsymmErrors(*eff->GetPaintedGraph());

	acceptance_bundle.is_angular = !is_momentum_transfer;

	return acceptance_bundle;
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
