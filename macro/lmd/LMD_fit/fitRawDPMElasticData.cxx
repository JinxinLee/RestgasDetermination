/*
 * fitRawDPMElasticData.cxx
 *
 *  Created on: Nov 7, 2013
 *      Author: steve
 */

#include "fit/ModelFitFacade.h"
#include "fit/estimatorImpl/Chi2Estimator.h"
#include "fit/data/ROOT/ROOTDataHelper.h"
#include "fit/minimizerImpl/ROOT/ROOTMinimizer.h"
#include "fit/data/DataStructs.h"
#include "visualization/ROOT/ROOTPlotter.h"
#include "visualization/ModelVisualizationProperties1D.h"
#include "model/PndLmdModelFactory.h"
#include "fit/PndLmdLumiFitOptions.h"
#include "fit/PndLmdLumiFitResult.h"
#include "PndLmdLumiHelper.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>

#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TMath.h"
#include "TH1D.h"
#include "TGraphAsymmErrors.h"

void fitRawDPMElasticData(TString data_path, bool is_filelist_path,
		double momentum, int step, int index, int num_events, double genlumi) {
	unsigned int num_events_to_process = 0;

	TChain tree("data");

	if (is_filelist_path) {
		std::stringstream ss;
		ss << data_path << "/filelist_" << index << ".txt";
		std::cout << "Opening " << ss.str() << std::endl;
		std::ifstream infile(ss.str().c_str());
		std::string line;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);

			std::cout << "Adding " << line.c_str() << std::endl;
			tree.Add(line.c_str());
		}
	} else {
		if (index != -1) {
			std::stringstream ss;
			ss << data_path << "/*_" << index << ".root";
			tree.Add(ss.str().c_str());
		} else {
			tree.Add(data_path + "/*.root");
		}
	}

	double theta_bound_low = 0.0005;
	double theta_bound_high = 0.015;

	PndLmdLumiHelper lumi_helper;
	double t_bound_low = lumi_helper.getMomentumTransferFromTheta(momentum,
			theta_bound_low);
	double t_bound_high = lumi_helper.getMomentumTransferFromTheta(momentum,
			theta_bound_high);

	TClonesArray* fEvt = new TClonesArray("TParticle", 100);
	tree.SetBranchAddress("Particles", &fEvt);

	std::cout << "tree has " << tree.GetEntries() << " entries" << std::endl;

	if (num_events < 0)
		num_events_to_process = tree.GetEntries();
	else
		num_events_to_process = num_events;

	if (step == 0) {

		TH1D *histtheta = new TH1D("hist_dpm_theta", "", 100, theta_bound_low,
				theta_bound_high);
		TH1D *histt = new TH1D("hist_dpm_t", "", 100, t_bound_low, t_bound_high);

		std::cout << "Processing " << num_events_to_process << " events!"
				<< std::endl;

		TDatabasePDG *pdg = TDatabasePDG::Instance();
		TLorentzVector ingoing(0, 0, momentum,
				TMath::Sqrt(
						TMath::Power(momentum, 2.0)
								+ TMath::Power(pdg->GetParticle(-2212)->Mass(), 2.0)));

		for (unsigned int i = 0; i < num_events_to_process; i++) {
			tree.GetEntry(i);
			for (unsigned int np = 0; np < fEvt->GetEntries(); np++) {
				TParticle *particle = (TParticle*) fEvt->At(np);
				if (particle->GetPdgCode() == -2212) {
					TLorentzVector outgoing;
					particle->Momentum(outgoing);
					histt->Fill(-1.0 * (outgoing - ingoing).M2());
					histtheta->Fill(outgoing.Theta());
				}
			}
		}
		std::stringstream ss;
		ss << data_path << "/data_" << index << ".root";
		TFile *output = new TFile(ss.str().c_str(), "RECREATE");
		histtheta->Write();
		histt->Write();
		output->Close();
	} else if (step == 1) {

		TFile *input;
		if (index != -1) {
			std::stringstream ss;
			ss << data_path << "/data_" << index << ".root";
			input = new TFile(ss.str().c_str(), "READ");
		} else {
			input = new TFile(data_path, "READ");
		}

		TH1D *histtheta;
		TH1D *histt;
		input->GetObject("hist_dpm_theta", histtheta);
		input->GetObject("hist_dpm_t", histt);

		ModelFitFacade model_fit_facade;
		PndLmdModelFactory model_factory;

		// create chi2 estimator
		shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());
		model_fit_facade.setEstimator(chi2_est);

		LumiFit::PndLmdFitModelOptions model_opt;

		// create a new model via the factory
		shared_ptr<Model1D> model1d = model_factory.generate1DModel(model_opt,
				momentum);
		// set model
		model_fit_facade.setModel1d(model1d);

		// create and set data
		ROOTDataHelper data_helper;
		shared_ptr<Data> data(new Data(1));
		data_helper.fillBinnedData(data, histtheta);
		model_fit_facade.setData(data);

		std::vector<TH1D*> histograms;

		double fit_range_low_begin = 0.002;
		double fit_range_low_end = 0.004;
		double fit_range_high = 0.01;

		int steps = 20;

		double reflumi = genlumi * num_events_to_process;

		EstimatorOptions est_opt;
		est_opt.setWithIntegralScaling(false);


		for (unsigned int step = 0; step < steps; step++) {
			DataStructs::DimensionRange fit_range;
			fit_range.range_low = fit_range_low_begin
					+ (fit_range_low_end - fit_range_low_begin) / steps * step;
			fit_range.range_high = fit_range_high;

			fit_range.is_active = true;

			est_opt.setFitRangeX(fit_range);

			model_fit_facade.setEstimatorOptions(est_opt);

			model1d->getModelParameterSet().setModelParameterValue("luminosity",
					reflumi);

			// create minimizer instance with control parameter
			shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

			model_fit_facade.setMinimizer(minuit_minimizer);

			ModelFitResult fit_result = model_fit_facade.Fit();
			double chi2 = fit_result.getFinalEstimatorValue() / fit_result.getNDF();
			std::cout << "chi2 of sample " << index << ": " << chi2 << std::endl;

			PndLmdLumiFitResult *temp_fit_result = new PndLmdLumiFitResult();
			temp_fit_result->setModelFitResult(fit_result);

			std::stringstream hs;
			hs << "reldiff_" << fit_range.range_low << "-" << fit_range.range_high;

			TH1D *reldiff = new TH1D(hs.str().c_str(), hs.str().c_str(), 100, -6, 6);
			reldiff->Fill(
					100.0 * (temp_fit_result->getLuminosity() - reflumi) / reflumi);

			histograms.push_back(reldiff);
		}

		model_opt.momentum_transfer_active = true;

		// create a new model via the factory
		model1d = model_factory.generate1DModel(model_opt, momentum);
		// set model
		model_fit_facade.setModel1d(model1d);

		// create and set data
		shared_ptr<Data> datat(new Data(1));
		data_helper.fillBinnedData(datat, histt);
		model_fit_facade.setData(datat);

		PndLmdLumiHelper lumi_helper;
		double t_fit_range_low_begin = lumi_helper.getMomentumTransferFromTheta(
				momentum, fit_range_low_begin);
		double t_fit_range_low_end = lumi_helper.getMomentumTransferFromTheta(
				momentum, fit_range_low_end);
		double t_fit_range_high = lumi_helper.getMomentumTransferFromTheta(momentum,
				fit_range_high);

		// momentum transfer fit
		for (unsigned int step = 0; step < steps; step++) {
			DataStructs::DimensionRange fit_range;
			fit_range.range_low = t_fit_range_low_begin
							+ (t_fit_range_low_end - t_fit_range_low_begin) / steps * step;
			fit_range.range_high = t_fit_range_high;

			fit_range.is_active = true;

			est_opt.setFitRangeX(fit_range);

			model1d->getModelParameterSet().setModelParameterValue("luminosity",
					reflumi);

			// create minimizer instance with control parameter
			shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

			model_fit_facade.setMinimizer(minuit_minimizer);

			ModelFitResult fit_result = model_fit_facade.Fit();
			double chi2 = fit_result.getFinalEstimatorValue() / fit_result.getNDF();
			std::cout << "reduced chi2 of sample " << index << ": " << chi2
					<< std::endl;

			PndLmdLumiFitResult *temp_fit_result = new PndLmdLumiFitResult();
			temp_fit_result->setModelFitResult(fit_result);

			std::stringstream hs;
			hs << "t_reldiff_" << fit_range.range_low << "-" << fit_range.range_high;

			TH1D *reldiff = new TH1D(hs.str().c_str(), hs.str().c_str(), 100, -6, 6);
			reldiff->Fill(
					100.0 * (temp_fit_result->getLuminosity() - reflumi) / reflumi);

			histograms.push_back(reldiff);
		}

		std::stringstream ss;
		ss << data_path << "/fitteddata_" << index << ".root";

		TFile *output = new TFile(ss.str().c_str(), "RECREATE");

		std::cout << "writing " << histograms.size() << " histograms to file! "
				<< std::endl;
		for (unsigned int i = 0; i < histograms.size(); i++) {
			histograms[i]->Write();
		}

		/*ROOTPlotter root_plotter;

		 ModelVisualizationProperties1D vis_prop;
		 DataStructs::dimension_range plot_range;
		 plot_range.range = std::make_pair(t_fit_range_low_end, t_fit_range_high);
		 vis_prop.setBinningFactor(datat->getBinningFactor());
		 vis_prop.setPlotRange(plot_range);

		 TGraphAsymmErrors* graph = root_plotter.createGraphFromModel1D(model1d,
		 vis_prop);
		 //temp_fit_result->Write("fit_result");

		 graph->Write("model");
		 histtheta->Write();
		 histt->Write();*/
		output->Close();
	}
}

void displayInfo() {
	// display info
	std::cout << "Required arguments are: " << std::endl;
	std::cout << "-m [pbar momentum]" << std::endl;
	std::cout << "-p [path to data]" << std::endl;
	std::cout
			<< "-l [computation step] (0 create histogram from data, 1 fit model)"
			<< std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-i [filename index]" << std::endl;
	std::cout << "-f is filelist directory" << std::endl;
	std::cout << "-g [generated luminosity per event]" << std::endl;
	std::cout << "-n [number of events to process] "
			"(if not specified all data found will be processed)" << std::endl;

}

int main(int argc, char* argv[]) {
	bool is_mom_set = false, is_data_path_set = false, is_gen_lumi_set = false;
	bool is_filelist_dir = false;
	double genlumi = -1.0;
	double momentum = -1.0;
	int num_events = -1;
	int index = -1;
	int step = -1;
	std::string data_path;
	int c;

	while ((c = getopt(argc, argv, "hm:i:l:n:p:g:f")) != -1) {
		switch (c) {
			case 'm':
				momentum = atof(optarg);
				is_mom_set = true;
				break;
			case 'g':
				genlumi = atof(optarg);
				is_gen_lumi_set = true;
				break;
			case 'l':
				step = atoi(optarg);
				break;
			case 'f':
				is_filelist_dir = true;
				break;
			case 'i':
				index = atoi(optarg);
				break;
			case 'n':
				num_events = atoi(optarg);
				break;
			case 'p':
				data_path = optarg;
				is_data_path_set = true;
				break;
			case '?':
				if (optopt == 'p' || optopt == 'm' || optopt == 'n')
					std::cerr << "Option -" << optopt << " requires an argument."
							<< std::endl;
				else if (isprint(optopt))
					std::cerr << "Unknown option -" << optopt << "." << std::endl;
				else
					std::cerr << "Unknown option character" << optopt << "." << std::endl;
				return 1;
			case 'h':
				displayInfo();
				return 1;
			default:
				return 1;
		}
	}

	if (is_mom_set && is_data_path_set) {
		fitRawDPMElasticData(data_path, is_filelist_dir, momentum, step, index,
				num_events, genlumi);
	} else
		displayInfo();

	return 0;
}
