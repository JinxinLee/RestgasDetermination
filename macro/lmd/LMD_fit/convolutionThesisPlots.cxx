/*
 * convolutionThesisPlots.cxx
 *
 *  Created on: Jul 31, 2013
 *      Author: steve
 */

#include "PndLmdDataFacade.h"
#include "PndLmdResultPlotter.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdAcceptance.h"
#include "PndLmdModelFactory.h"
#include "GaussianModel1D.h"
#include "AsymmetricGaussianModel1D.h"

#include <iostream>               // for std::cout
#include <vector>
#include <sstream>

#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TFile.h"
#include "TString.h"

void makeConvolutionThesisPlots(TString acceptance_file_dir) {
	PndLmdDataFacade lmd_data_facade;

	LumiFit::LmdDimensionRange fit_range;
	fit_range.setUnitPrefix(DataStructs::MILLI);
	fit_range.setRangeLow(0.0);
	fit_range.setRangeHigh(20.0);

	// ------ get files -------------------------------------------------------
	TFile *facc = new TFile(acceptance_file_dir + "/lmd_acc_data.root", "OPEN");
	// ------------------------------------------------------------------------

	// ---- acceptance or box gen data ---- //
	// same procedure as for the "real" data
	std::vector<PndLmdAcceptance> lmd_acc_vec = lmd_data_facade.getDataFromFile<
			PndLmdAcceptance>(facc);

	PndLmdAcceptance *acc;
	if (lmd_acc_vec.size() > 0)
		acc = &lmd_acc_vec[0];
	if (!acc)
		return;

	PndLmdLumiFitOptions *fit_options = new PndLmdLumiFitOptions();

	LumiFit::LmdBinaryFitOptions bit_fit_opt(0);
	bit_fit_opt.setFitAsRaw(false);
	bit_fit_opt.setAcceptanceCorrMode(true);
	bit_fit_opt.setSmearingMode(false);
	fit_options->setModelBinaryOptions(bit_fit_opt);
	fit_options->setAcceptance(acc);
	fit_options->setPrimaryDimensionFitRange(fit_range);

	PndLmdModelFactory model_fac;
	std::cout << acc->getLabMomentum() << std::endl;
	shared_ptr<Model1D> unsmeared_model = model_fac.generate1DModel(fit_options,
			acc->getLabMomentum());
	unsmeared_model->getModelParameterSet().getModelParameter(
			std::make_pair("dpm_angular_1d", "luminosity"))->setValue(1.0);

	unsmeared_model->getModelParameterSet().printInfo();

	bit_fit_opt.setFitAsRaw(false);
	bit_fit_opt.setAcceptanceCorrMode(true);
	bit_fit_opt.setSmearingMode(true);
	fit_options->setSmearingModelType(12);
	fit_options->setResolutionParametrizationFileUrl(
			acceptance_file_dir + "/resolution_params_1.root");
	fit_options->setModelBinaryOptions(bit_fit_opt);
	fit_options->setAcceptance(acc);
	fit_options->setPrimaryDimensionFitRange(fit_range);

	std::cout << acc->getLabMomentum() << std::endl;
	shared_ptr<Model1D> smeared_model = model_fac.generate1DModel(fit_options,
			acc->getLabMomentum());
	smeared_model->getModelParameterSet().getModelParameter(
			std::make_pair("dpm_angular_1d", "luminosity"))->setValue(1.0);

	smeared_model->getModelParameterSet().printInfo();

	std::vector<std::pair<double, double> > temp_vec_range;
	temp_vec_range.push_back(std::make_pair(0.001, 0.012));

	shared_ptr<Model1D> asymmgauss(new AsymmetricGaussianModel1D("asymmgauss1"));
	asymmgauss->getModelParameterSet().getModelParameter("asymm_gauss_mean")->setValue(0.004);
	asymmgauss->getModelParameterSet().getModelParameter("asymm_gauss_sigma_left")->setValue(
			0.001);
	asymmgauss->getModelParameterSet().getModelParameter("asymm_gauss_sigma_right")->setValue(
			0.002);
	asymmgauss->getModelParameterSet().getModelParameter("asymm_gauss_amplitude")->setValue(
			1.0);
	std::cout<<"integral of aymm gauss: "<<asymmgauss->Integral(temp_vec_range, 1e-3)<<std::endl;

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


	double int_unsmeared = unsmeared_model->Integral(temp_vec_range, 1e-3);
	double int_smeared = smeared_model->Integral(temp_vec_range, 1e-3);
	std::cout << int_unsmeared << " vs "
			<< int_smeared<<std::endl;

	double xval = 4.0;
	double xval2 = 2.5;
	double height_model = unsmeared_model->evaluate(&xval);
	double height_gauss = gauss1->evaluate(&xval);
	double height_gauss2 = gauss1->evaluate(&xval2);

	ROOTPlotter plotter;
	ModelVisualizationProperties1D vp;
	DataStructs::dimension_range plot_range;
	plot_range.range = std::make_pair(0.0, 0.012);
	vp.setPlotRange(plot_range);

	TGraphAsymmErrors *graph_unsmeared_model = plotter.createGraphFromModel1D(
			unsmeared_model, vp);
	graph_unsmeared_model->SetLineColor(2);
	graph_unsmeared_model->SetLineWidth(1.0);

	TGraphAsymmErrors *graph_smeared_model = plotter.createGraphFromModel1D(
			smeared_model, vp);
	graph_smeared_model->SetLineColor(1);
	graph_smeared_model->SetLineWidth(1.0);

	TGraphAsymmErrors *graph_gauss1 = plotter.createGraphFromModel1D(gauss1, vp);
	graph_gauss1->SetLineColor(1);
	graph_gauss1->SetLineStyle(2);
	graph_gauss1->SetLineWidth(1.0);
	TGraphAsymmErrors *graph_gauss2 = plotter.createGraphFromModel1D(gauss2, vp);
	graph_gauss2->SetLineColor(1);
	graph_gauss2->SetLineWidth(1.0);

	TCanvas c;

	graph_unsmeared_model->SetTitle("");
	graph_unsmeared_model->Draw("AC");
	graph_unsmeared_model->GetXaxis()->SetTitle("#theta_{mc} [mrad]");
	graph_smeared_model->Draw("CSAME");
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
