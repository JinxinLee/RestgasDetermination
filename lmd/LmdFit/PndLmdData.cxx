/*
 * PndLmdData.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"
#include "PndROOTModelFitter.h"
#include "PndLmdLumiFitResult.h"
#include "PndLmdLumiFitOptions.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "Math/WrappedTF1.h"
#include "Math/GSLIntegrator.h"
#include "TCanvas.h"

ClassImp(PndLmdData)

PndLmdData::PndLmdData(TFile *f_, int num_events_, double plab_,
		int elastic_th_bins_, int elastic_phi_bins_, double elastic_th_range_low_,
		double elastic_th_range_high_, double elastic_phi_range_low_,
		double elastic_phi_range_high_, double generated_luminosity_per_event_) :
		PndLmdDataBase(f_, num_events_, plab_, elastic_th_bins_, elastic_phi_bins_,
				elastic_th_range_low_, elastic_th_range_high_, elastic_phi_range_low_,
				elastic_phi_range_high_) {
	//fit_map =
	//		new std::map<PndLmdLumiModel*, std::vector<PndLmdLumiFitResult*> >();

	fitter = new PndROOTModelFitter();
	luminosity_ref = generated_luminosity_per_event_ * num_events_;
	makeDir();
}
PndLmdData::PndLmdData() {
}

PndLmdData::~PndLmdData() {
}

void PndLmdData::saveToRootFile() {
	std::cout << "Saving " << getName() << " to file..." << std::endl;

	makeDir(); // this will just change current directory to the one created by this data set

	this->Write("lmddata");
}

double PndLmdData::getReferenceLuminosity() const {
	return luminosity_ref;
}

void PndLmdData::setReferenceLuminosity(double luminosity_ref_) {
	luminosity_ref = luminosity_ref_;
}

double PndLmdData::getBinningFactor(PndLmdLumiFitOptions *fit_opt) const {
	if (fit_opt->isFitRaw())
		return getTBinSize();
	double bin_factor = getThBinSize();
	if (fit_opt->getFitDimension())
		bin_factor *= getPhiBinSize();
	return bin_factor;
}

/*void PndLmdData::makeCorrectedGraph(TF1 *func, TH1D* hist) {
 ROOT::Math::WrappedTF1 wf1(*func);

 // Create the Integrator
 ROOT::Math::GSLIntegrator ig(ROOT::Math::IntegrationOneDim::kADAPTIVE);

 // Set parameters of the integration
 ig.SetFunction(wf1);
 ig.SetRelTolerance(0.001);

 TH1D* temp = new TH1D("corrected", "corrected", getThBins(), getThRangeLow(), getThRangeHigh());
 for(int i = 0; i < hist->GetXaxis()->GetNbins(); i++) {
 double xlow = hist->GetBinLowEdge(i);
 double xhigh = xlow+hist->GetBinWidth(i);

 double int_func_real = ig.Integral(xlow, xhigh);
 double int_func_approx = func->Eval(hist->GetBinCenter(i))*(xhigh-xlow);

 double scale = 0.0;
 if(int_func_approx > 0.0) {
 scale = int_func_approx/int_func_real;
 std::cout<<int_func_approx<<" "<<int_func_real<<std::endl;
 }
 temp->Fill(hist->GetBinCenter(i), hist->GetBinContent(i)*scale);
 }

 TCanvas *asdf = new TCanvas("asdf", "asdf", 1000, 700);
 asdf->Divide(2,2);
 asdf->cd(1);
 temp->Draw();
 asdf->cd(2);
 hist->Draw();
 asdf->cd(3);
 TH1D* diff = new TH1D(*temp);
 diff->Add(hist, -1.0);
 diff->Draw();
 asdf->Update();
 asdf->SaveAs("testdiff.pdf");
 hist = temp;
 }*/

std::pair<double, double> PndLmdData::calcRange(
		PndLmdLumiFitOptions *fit_options) {
	double range_low, range_high;
	if (fit_options->isFitRaw()) {
		range_low = getTRangeLow();
		range_high = getTRangeHigh();
		if (fit_options->getTFitRangeLow() > getTRangeLow())
			range_low = fit_options->getTFitRangeLow();
		if (fit_options->getTFitRangeHigh() < getTRangeHigh())
			range_high = fit_options->getTFitRangeHigh();
	} else {
		range_low = getThRangeLow();
		range_high = getThRangeHigh();
		if (fit_options->getThetaFitRangeLow() > getThRangeLow())
			range_low = fit_options->getThetaFitRangeLow();
		if (fit_options->getThetaFitRangeHigh() < getThRangeHigh())
			range_high = fit_options->getThetaFitRangeHigh();
	}
	return std::make_pair(range_low, range_high);
}

void PndLmdData::fillFitData(shared_ptr<Model1D> model1d,
		PndLmdLumiFitOptions *fit_options) {
	fitter->clearData();
	TH1D* hist = getMeasuredHist1D(fit_options);

	std::pair<double, double> range = calcRange(fit_options);
	for (int i = 1; i <= getThBins(); i++) {
		ModelFitInterface::data_point datapoint;

		datapoint.x[0] = hist->GetBinCenter(i);
		datapoint.z = hist->GetBinContent(i);
		datapoint.z_error = hist->GetBinError(i);

		if (datapoint.x[0] < range.first || datapoint.x[0] > range.second)
			continue;
		if (datapoint.z == 0.0)
			continue;

		double xlow = hist->GetBinLowEdge(i);
		double xhigh = xlow + hist->GetBinWidth(i);

		double int_func_real = model1d->Integral(xlow, xhigh, 0.001);
		double int_func_approx = model1d->evaluate(datapoint.x) * (xhigh - xlow);

		double scale = 1.0;
		if (int_func_approx > 0.0) {
			scale = int_func_approx / int_func_real;
		}
		datapoint.scale = scale;
		datapoint.z = datapoint.z * scale;

		fitter->insertData(datapoint);
	}
}

PndLmdLumiFitResult* PndLmdData::Fit(PndLmdAcceptance *lmd_acc,
		PndLmdLumiFitOptions *fit_options) {

	std::cout << "Attempting to perform fit with following fit options:"
			<< std::endl;
	std::cout << *fit_options << std::endl;

	//first check if this model with the fit options have already been fitted
	std::vector<PndLmdLumiFitResult*> &model_fit_res = fit_map[lmd_acc];
	for (unsigned int i = 0; i < model_fit_res.size(); i++) {
		if (model_fit_res[i]->checkFitOptions(fit_options)) {
			std::cout << "Fit was already performed! Skipping..." << std::endl;
			return model_fit_res[i];
		}
	}

	std::cout << "Initiating Fitter..." << std::endl;

	PndLmdLumiFitResult *fit_result;

	if (fit_options->getFitterType() == 0) { // if user wants to use ROOT/Minuit
		// first clear current data set of the fitter
		fitter->clearData();
		// create a new model via the factory
		shared_ptr<Model1D> model1d = signal_model_fac.generate1DModel(fit_options,
				getLabMomentum(), lmd_acc);

		// insert data to the fitter
		fillFitData(model1d, fit_options);

		// set start value for overall normalization constant
		double binning_factor = 1.0;
		if (fit_options->isFitRaw())
			binning_factor = getTBinSize();
		else {
			binning_factor = getThBinSize();
			if (fit_options->getFitDimension())
				binning_factor *= getPhiBinSize();
		}
		fitter->setBinningFactor(binning_factor);

		// now set better starting lumi value
		std::pair<double, double> range = calcRange(fit_options);
		double integral_data = getMeasuredHist1D(fit_options)->Integral("width");
		double integral_func = model1d->Integral(range.first, range.second, 10e-3);
		double lumi_start = integral_data / integral_func
				/ getBinningFactor(fit_options);
		std::cout << integral_data << " / "
				<< integral_func * getBinningFactor(fit_options) << std::endl;
		std::cout << "(1D) Using start luminosity: " << lumi_start << std::endl;
		model1d->getModelParameterSet().setModelParameterValue("luminosity",
				lumi_start);

		// set free parameters
		model1d->getModelParameterSet().freeModelParameter("luminosity");

		// set model
		fitter->setModel(model1d);

		// call minimization procedure
		if (fitter->doFit()) {
			std::cout
					<< "ERROR: Problem while performing fit. Returning NULL pointer!"
					<< std::endl;
			return fit_result;
		}

		// store fit results
		std::cout << "Saving results..." << std::endl;

		fit_result = new PndLmdLumiFitResult(fit_options);

		fit_result->setChiSquare(fitter->chi2(fitter->getROOTMinimizer()->X()));

		for (unsigned int i = 0; i < fitter->getROOTMinimizer()->NDim(); i++) {
			if (0
					== fitter->getROOTMinimizer()->VariableName(i).compare("luminosity")) {
				fit_result->setLuminosity(fitter->getROOTMinimizer()->X()[i]);
				fit_result->setLuminosityStatError(
						fitter->getROOTMinimizer()->Errors()[i]);
			} else {
				fit_result->addParameterValue(
						fitter->getROOTMinimizer()->VariableName(i),
						fitter->getROOTMinimizer()->X()[i],
						fitter->getROOTMinimizer()->Errors()[i]);
			}
		}

		fit_result->setNDF(
				fitter->getNumberOfDataPoints() - fitter->getROOTMinimizer()->NFree());
	}
	else { // user wants to use ROOFIT
		// ok do roofit stuff here
	}

	model_fit_res.push_back(fit_result);
	return fit_result;
}

TH1D* PndLmdData::getMeasuredHist1D(PndLmdLumiFitOptions *fit_options) const {
	if (fit_options->isFitRaw()) {
		if (fit_options->isSmearingOn()) {
			if (fit_options->isAcceptanceCorrOn()) {
				return t_reco_1d;
			} else {
				std::cout
						<< "WARNING: Requesting data which has flat acceptance and is resolution smeared."
						<< "This cannot exist... the standard reconstruction histogram will be returned."
						<< "Make sure that the fit range is chosen appropriately (in which acceptance is actually flat)."
						<< std::endl;
				return t_reco_1d;
			}
		} else {
			if (fit_options->isAcceptanceCorrOn()) {
				return t_mc_acc_1d;
			} else {
				return t_mc_1d;
			}
		}
	} else {
		if (fit_options->isSmearingOn()) {
			if (fit_options->isAcceptanceCorrOn()) {
				return reco_1d;
			} else {
				std::cout
						<< "WARNING: Requesting data which has flat acceptance and is resolution smeared."
						<< "This cannot exist... the standard reconstruction histogram will be returned."
						<< "Make sure that the fit range is chosen appropriately (in which acceptance is actually flat)."
						<< std::endl;
				return reco_1d;
			}
		} else {
			if (fit_options->isAcceptanceCorrOn()) {
				return mc_acc_1d;
			} else {
				return mc_1d;
			}
		}
	}
}

TH2D* PndLmdData::getMeasuredHist2D(PndLmdLumiFitOptions *fit_options) const {
	if (fit_options->isSmearingOn()) {
		if (fit_options->isAcceptanceCorrOn()) {
			return reco_2d;
		} else {
			std::cout
					<< "WARNING: Requesting data which has flat acceptance and is resolution smeared."
					<< "This cannot exist... the standard reconstruction histogram will be returned."
					<< "Make sure that the fit range is chosen appropriately (in which acceptance is actually flat)."
					<< std::endl;
			return reco_2d;
		}
	} else {
		if (fit_options->isAcceptanceCorrOn()) {
			return mc_acc_2d;
		} else {
			return mc_2d;
		}
	}
}

std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >& PndLmdData::getFitMap() {
	return fit_map;
}

std::vector<PndLmdAcceptance*> PndLmdData::getListOfAcceptances() {
	std::vector<PndLmdAcceptance*> return_vector;
	for (std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >::iterator i =
			fit_map.begin(); i != fit_map.end(); i++) {
		return_vector.push_back(i->first);
	}
	return return_vector;
}

PndLmdLumiFitResult* PndLmdData::getFitResult(PndLmdAcceptance* lmd_acc,
		PndLmdLumiFitOptions* fit_options) {
	std::vector<PndLmdLumiFitResult*> &fit_res_vec = fit_map[lmd_acc];
	for (unsigned int i = 0; i < fit_res_vec.size(); i++) {
		if (fit_res_vec[i]->checkFitOptions(fit_options))
			return fit_res_vec[i];
	}
	return NULL; // found nothing
}

std::vector<PndLmdLumiFitResult*> PndLmdData::getFitResults(
		PndLmdAcceptance* lmd_acc) {
	return fit_map[lmd_acc];
}

std::vector<std::pair<PndLmdAcceptance*, PndLmdLumiFitResult*> > PndLmdData::getFitResults(
		PndLmdLumiFitOptions* fit_options) {
	std::vector<std::pair<PndLmdAcceptance*, PndLmdLumiFitResult*> > new_fit_res_vec;
	for (std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >::iterator i =
			fit_map.begin(); i != fit_map.end(); i++) {
		for (unsigned int j = 0; j < i->second.size(); j++) {
			if (i->second[j]->checkFitOptions(fit_options))
				new_fit_res_vec.push_back(std::make_pair(i->first, i->second[j]));
		}
	}
	return new_fit_res_vec;
}
