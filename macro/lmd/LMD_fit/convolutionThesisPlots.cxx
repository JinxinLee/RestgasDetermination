/*
 * convolutionThesisPlots.cxx
 *
 *  Created on: Jul 31, 2013
 *      Author: steve
 */

#include "PndLmdLumiHelper.h"
#include "PndLmdResultPlotter.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdAcceptance.h"
#include "PndLmdModelFactory.h"
#include "GaussianModel1D.h"

#include <iostream>               // for std::cout
#include <vector>
#include <sstream>

#include <tr1/memory>

#include "TCanvas.h"
#include "TLine.h"
#include "TFile.h"
#include "TString.h"

using std::tr1::shared_ptr;

void makeConvolutionThesisPlots(TString acceptance_file_dir) {
	// A small helper class that helps to construct lmddata objects
	PndLmdLumiHelper lumifit_helper;

	double fit_range_low = 0.0;
	double fit_range_high = 20.0;

	// ------ get files -------------------------------------------------------
	TFile *facc = new TFile(acceptance_file_dir + "/lmd_acc_data.root", "OPEN");
	// ------------------------------------------------------------------------

	// ---- acceptance or box gen data ---- //
	// same procedure as for the "real" data
	std::vector<PndLmdAcceptance*> my_lmd_acc_vec;

	my_lmd_acc_vec = lumifit_helper.getAcceptanceFromFile(facc);

	PndLmdAcceptance *acc;
	if (my_lmd_acc_vec.size() > 0) {
		acc = my_lmd_acc_vec[0];
	}
	if (!acc)
		return;

	PndLmdLumiFitOptions *fit_options;

	LumiFit::LmdBinaryFitOptions bit_fit_opt(0);
	bit_fit_opt.setFitAsRaw(false);
	bit_fit_opt.setAcceptanceCorrMode(true);
	fit_options = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, acc->getLabMomentum(),
			fit_range_low, fit_range_high);

	PndLmdModelFactory model_fac;
	shared_ptr<Model1D> unsmeared_model = model_fac.generate1DModel(fit_options,
			acc->getLabMomentum(), acc);
	unsmeared_model->getModelParameterSet().getModelParameter(
			std::make_pair("dpm_angular_1d", "luminosity"))->setValue(1.0);

	shared_ptr<Model1D> gauss1(new GaussianModel1D("gauss1"));
	gauss1->getModelParameterSet().getModelParameter("gauss_mean")->setValue(4.0);
	gauss1->getModelParameterSet().getModelParameter("gauss_sigma")->setValue(
			1.0);
	gauss1->getModelParameterSet().getModelParameter("gauss_amplitude")->setValue(
			1.0);

	shared_ptr<Model1D> gauss2(new GaussianModel1D("gauss2"));
	gauss2->getModelParameterSet().getModelParameter("gauss_mean")->setValue(2.5);
	gauss2->getModelParameterSet().getModelParameter("gauss_sigma")->setValue(
			1.0);
	gauss2->getModelParameterSet().getModelParameter("gauss_amplitude")->setValue(
			1.0);

	double xval = 4.0;
	double xval2 = 2.5;
	double height_model = unsmeared_model->evaluate(&xval);
	double height_gauss = gauss1->evaluate(&xval);
	double height_gauss2 = gauss1->evaluate(&xval2);

	ROOTPlotter plotter;
	ModelVisualizationProperties1D vp;
	vp.setPlotRange(std::make_pair(0.0, 10.0));

	TGraphErrors *graph_unsmeared_model = plotter.createGraphFromModel1D(
			unsmeared_model, vp);
	graph_unsmeared_model->SetLineColor(2);
	graph_unsmeared_model->SetLineWidth(1.0);
	TGraphErrors *graph_gauss1 = plotter.createGraphFromModel1D(gauss1, vp);
	graph_gauss1->SetLineColor(1);
	graph_gauss1->SetLineStyle(2);
	graph_gauss1->SetLineWidth(1.0);
	TGraphErrors *graph_gauss2 = plotter.createGraphFromModel1D(gauss2, vp);
	graph_gauss2->SetLineColor(1);
	graph_gauss2->SetLineWidth(1.0);

	TCanvas c;

	graph_unsmeared_model->SetTitle("");
	graph_unsmeared_model->Draw("AC");
	graph_unsmeared_model->GetXaxis()->SetTitle("#theta_{mc} [mrad]");
	TLine linem(4.0, 0.0, 4.0, height_model);
	linem.SetLineColor(2);
	linem.SetLineWidth(1.0);
	linem.Draw("SAME");
	graph_gauss1->Draw("CSAME");
	TLine line1(4.0, 0.0, 4.0, height_gauss2);
	line1.SetLineColor(1);
	line1.SetLineWidth(1.0);
	line1.Draw("SAME");
	graph_gauss2->Draw("CSAME");
	TLine line2(2.5, 0.0, 2.5, height_gauss2);
	line2.SetLineColor(1);
	line2.SetLineStyle(2);
	line2.SetLineWidth(1.0);
	line2.Draw("SAME");

	c.SaveAs("plot.pdf");
}

int main(int argc, char* argv[]) {
	if (argc == 2) {
		makeConvolutionThesisPlots(argv[1]);
	}
	return 0;
}
