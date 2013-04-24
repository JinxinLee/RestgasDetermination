/*
 * PndLmdLumiModelFitter.cxx
 *
 *  Created on: Jan 21, 2013
 *      Author: steve
 */

#include "PndLmdLumiModelFitter.h"
#include "PndLmdModelFactory.h"
#include "PndLmdData.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdLumiFitResult.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

PndLmdLumiModelFitter::PndLmdLumiModelFitter() {
  signal_model_fac = new PndLmdModelFactory();
}

PndLmdLumiModelFitter::~PndLmdLumiModelFitter() {
  // TODO Auto-generated destructor stub
}

void PndLmdLumiModelFitter::fillVectors(PndLmdData *lmd_data, Model1D &model1d,
    PndLmdLumiFitOptions *fit_options) {
  x.clear();
  z.clear();
  errorz.clear();
  TH1D* hist = lmd_data->getMeasuredHist1D(fit_options);

  double range_low, range_high;
  if (fit_options->isFitRaw()) {
    range_low = lmd_data->getTRangeLow();
    range_high = lmd_data->getTRangeHigh();
    if (fit_options->getTFitRangeLow() > lmd_data->getTRangeLow())
      range_low = fit_options->getTFitRangeLow();
    if (fit_options->getTFitRangeHigh() < lmd_data->getTRangeHigh())
      range_high = fit_options->getTFitRangeHigh();
  } else {
    range_low = lmd_data->getThRangeLow();
    range_high = lmd_data->getThRangeHigh();
    if (fit_options->getThetaFitRangeLow() > lmd_data->getThRangeLow())
      range_low = fit_options->getThetaFitRangeLow();
    if (fit_options->getThetaFitRangeHigh() < lmd_data->getThRangeHigh())
      range_high = fit_options->getThetaFitRangeHigh();
  }

  for (int i = 1; i <= lmd_data->getThBins(); i++) {
    double *xi = new double[1];
    xi[0] = hist->GetBinCenter(i);
    double zi = hist->GetBinContent(i);
    double errzi = hist->GetBinError(i);

    if (xi[0] < range_low || xi[0] > range_high)
      continue;
    if (zi == 0.0)
      continue;

    double xlow = hist->GetBinLowEdge(i);
    double xhigh = xlow + hist->GetBinWidth(i);

    double int_func_real = model1d.GSL_Integral(xlow, xhigh, 0.001);
    double int_func_approx = model1d.evaluate(xi) * (xhigh - xlow);

    double scale = 1.0;
    if (int_func_approx > 0.0) {
      scale = int_func_approx / int_func_real;
    }
    zi = zi * scale;

    x.push_back(xi);
    z.push_back(zi);
    errorz.push_back(errzi);
  }
}

/*void PndLmdLumiModelFitter::probeChi2(Double_t *par) {
 TCanvas c("c", "c", 1000, 700);
 double reflumi = par[0];
 int n = 200;
 double xv[n];
 double yv[n];

 // probe chi2 +-5% around the minimum value that was determined by fit
 double minlumi = 0.95 * par[0];
 double maxlumi = 1.05 * par[0];
 double stepsize = (maxlumi - minlumi) / n;
 for (int i = 0; i < n; i++) {
 xv[i] = minlumi + stepsize * i;
 par[0] = xv[i];
 yv[i] = chi2(par);
 }

 TGraph *g = new TGraph(n, xv, yv);
 g->Draw("AP");
 TLine l(reflumi, g->GetYaxis()->GetXmin(), reflumi,
 g->GetYaxis()->GetXmax());
 l.Draw();
 c.SaveAs("chi2probe.pdf");
 }*/

double PndLmdLumiModelFitter::chi2(const double *par) {
  /* this point is crucial: because this method is called for every iteration of
   * the fitter, the "newly changed" parameters have to be updated in the model so that the
   * changes are actually registered.
   */
  model->updateModelParameters(par);

  //calculate chisquare
  Double_t chisq = 0.0;
  Double_t delta;

  // loop over data
  for (int i = 0; i < x.size(); i++) {
    delta = (z[i] - par[0] * par[1] * model->evaluate(x[i])) / errorz[i];
    chisq += delta * delta;
  }
  //std::cout << "chi2: " << chisq << std::endl;
  return chisq;
}

void PndLmdLumiModelFitter::doFit(PndLmdData *lmd_data,
    PndLmdLumiFitOptions *fit_options, PndLmdLumiFitResult* fit_result) {

  std::cout << "Setting up fit..." << std::endl;

  //Minimize = Migrad+Simplex
  ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2",
      "Minimize");
  // set tolerance , etc...
  min->SetMaxFunctionCalls(100000); // for Minuit/Minuit2
  min->SetMaxIterations(1000);
  min->SetTolerance(0.1);
  //min->SetPrecision(0.001);
  min->SetPrintLevel(5);

  // number of parameters is equals that of the model + 1 for overall scale (luminosity)
  // and +1 for the correction factor of the overall scale due to binning effects
  int numpars = model->getModelParameterSet().getNumberOfFreeParameters() + 2;

  // create function wrapper for minmizer  a IMultiGenFunction type
  ROOT::Math::Functor fc(this, &PndLmdLumiModelFitter::chi2, numpars);
  min->SetFunction(fc);

  double variable[numpars];

  // parameter initialization
  // first are the global parameters
  variable[0] = 1.0; // set luminosity to 1.0
  if (fit_options->isFitRaw())
    variable[1] = lmd_data->getTBinSize();
  else {
    variable[1] = lmd_data->getThBinSize();
    if (fit_options->getFitDimension())
      variable[1] *= lmd_data->getPhiBinSize();
  }

  // now fill the values of all model parameters into the array variable
  model->getModelParameterSet().getModelParameters(variable);

  // now set better starting lumi value
  double range_low, range_high;
  if (fit_options->isFitRaw()) {
    range_low = lmd_data->getTRangeLow();
    range_high = lmd_data->getTRangeHigh();
    if (fit_options->getTFitRangeLow() > lmd_data->getTRangeLow())
      range_low = fit_options->getTFitRangeLow();
    if (fit_options->getTFitRangeHigh() < lmd_data->getTRangeHigh())
      range_high = fit_options->getTFitRangeHigh();
  } else {
    range_low = lmd_data->getThRangeLow();
    range_high = lmd_data->getThRangeHigh();
    if (fit_options->getThetaFitRangeLow() > lmd_data->getThRangeLow())
      range_low = fit_options->getThetaFitRangeLow();
    if (fit_options->getThetaFitRangeHigh() < lmd_data->getThRangeHigh())
      range_high = fit_options->getThetaFitRangeHigh();
  }

  double integral_data = lmd_data->getMeasuredHist1D(fit_options)->Integral(
      "width");
  double integral_func = model->GSL_Integral(range_low, range_high, 10e-3);
  double lumi_start = integral_data / integral_func
      / lmd_data->getBinningFactor(fit_options);
  std::cout << integral_data << " / "
      << integral_func * lmd_data->getBinningFactor(fit_options)
      << std::endl;
  std::cout << "(1D) Using start luminosity: " << lumi_start << std::endl;
  variable[0] = lumi_start;

  // Set the free variables to be minimized!
  min->SetVariable(0, "luminosity", variable[0], 0.001 * variable[0]);
  min->SetFixedVariable(1, "bin_factor", variable[1]);
  for (int i = 0; i < model->getModelParameterSet().getNumberOfFreeParameters();
      i++) {
    min->SetVariable(
        2 + i,
        model->getModelParameterSet().getFreeModelParameter(i)->getName(),
        model->getModelParameterSet().getFreeModelParameter(i)->getValue(),
        0.01
            * model->getModelParameterSet().getFreeModelParameter(i)->getValue());
  }

  std::cout << "Finished setting up fit!" << std::endl;

  std::cout << "Performing fit..." << std::endl;
  min->Minimize();
  std::cout << "Fit done!" << std::endl;

  // save results to fit result object

  std::cout << "Saving results..." << std::endl;
  fit_result->setLuminosity(min->X()[0]);
  fit_result->setLuminosityStatError(min->Errors()[0]);
  for (unsigned int i = 2; i < numpars; i++) {
    fit_result->addParameterValue(min->VariableName(i), min->X()[i],
        min->Errors()[i]);
    variable[i] = min->X()[i];
  }

  fit_result->setChiSquare(chi2(variable));
  fit_result->setNDF(x.size() - min->NFree());
  //probeChi2(variable);
}

PndLmdLumiFitResult* PndLmdLumiModelFitter::fitTo(PndLmdData *lmd_data,
    PndLmdAcceptance *lmd_acc, PndLmdLumiFitOptions *fit_options) {
  PndLmdLumiFitResult* fit_result = new PndLmdLumiFitResult(fit_options);

  if (fit_options->getFitDimension() == 0) {
//1d fit
    std::shared_ptr<Model1D> model1d = signal_model_fac->generate1DModel(fit_options,
        lmd_data->getLabMomentum(), lmd_acc);
    model = model1d.get();

    // do a check that everything is set
    if (model->init()) {
      std::cout << "Error: not all parameters have been set!" << std::endl;
    }
    model->getModelParameterSet().printInfo();

    fillVectors(lmd_data, *model1d.get(), fit_options);
    doFit(lmd_data, fit_options, fit_result);

  } else {
//2d fit goes here
  }

  return fit_result;
}
