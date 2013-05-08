#include "PndLmdModelFactory.h"
#include "PndLmdLumiFitOptions.h"

#include <iostream>
#include <sstream>
#include <map>
#include <boost/filesystem.hpp>
#include <cmath>

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include "TF1.h"
#include "TH2I.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TRandom.h"
#include "TGaxis.h"
#include "TPaletteAxis.h"

#define C_PI 3.14159


/* ok this block here is the todo brain dump /
 * what still has to be done / how it should work at the end...
 *
 * - basically you define in the script which parametrization you want to calculate
 *   (via fit_options->smearing_model_type). Then you get that model1d from the
 *   modelfactory.
 * - ok now that we have the model we need to read in data via the helper class
 *   (ok at this point we need to read in the data sliced in theta and later also in phi...).
 *   then fit this data with this model, however we have to open all of the
 *   parameters of the model (not the ones from the parametrization models, but these
 *   parametrization models probably have to be released once a parameter is freed
 *   that that model is supposed to parametrize). Probably we have to adjust the pnddata
 *   class a bit to also allow difference graphs etc because now its fixed to normal
 *   theta graphs (mainly from labels...). ok then we can actually fit the model to the data.
 * - now each fit result includes a point for all of the parametrization models that appear
 *   in the model -> so fill a new tgraph for each of these objects and once all silces are
 *   fitted then go through each of these model parameters (tgraph + parametrization model and fit
 *   it to the data) then the parameters we get for that model should be save to some kind of db!
 *
 *   ok in case we have no parametrization models then we have a constant parametrization
 *   so just
 */


void determineResolution(std::map<double, std::string> &data_dirs) {
  boost::filesystem::path curpath = boost::filesystem::current_path(); // get current path
  curpath = boost::filesystem::path(curpath.string() + "/resolutions");
  boost::filesystem::create_directory(curpath); // make new directory

  //now run determine resolution function on all the folders and make a graph
  for(std::map<double, std::string>::iterator it = data_dirs.begin(); it != data_dirs.end(); it++) {
    boost::filesystem::current_path(curpath);
    determineSlicedResolution(it->first, it->second.c_str());
  }

  boost::filesystem::current_path(curpath);
}

void determineSlicedResolution(double mom, const char* dir) {
  PndLmdModelFactory model_factory; // construct model factory
  // specify which of type of smearing model we want to generate
  PndLmdLumiFitOptions *fit_options;
  fit_options->setSmearingMode(1);

  // generate the model
  std::shared_ptr<Model1D> resolution_model = model_factory.generate1DResolutionModel(fit_options);

  // free all parameters that appear in this model
  resolution_model->getModelParameterSet().freeAllModelParameter();

  // now read in data either sliced or not depending on the smearing type!

}


/*struct fit_pars {
  double theta;
  double theta_mean;
  double theta_mean_err;
  double sigma_gauss_wide;
  double sigma_gauss_wide_err;
  double sigma_gauss_narrow;
  double sigma_gauss_narrow_err;
  double ratio_gwgn;
  double ratio_gwgn_err;
  double red_chi2;
};

Double_t doublegauss(Double_t *x, Double_t *par)
{
  return par[0] * (par[3]/(par[3] + 1.0) * exp(-(pow(x[0] - par[1], 2.0) / (2.0 * par[2] * par[2]))) +
                   1.0/(par[3] + 1.0) * exp(-(pow(x[0] - par[5], 2.0) / (2.0 * par[4] * par[4]))));
}*/


/*fit_pars parametersVsTheta(std::vector<fit_pars> pars) {
  fit_pars mean;
  mean.theta = 0.0;

  double acc_range_low = 1.0, acc_range_high = -1.0;
  double x[pars.size()];
  double y[pars.size()];
  double xerr[pars.size()];
  double yerr[pars.size()];

  int counter = 0;
  int maxsize = 0;
  acc_range_low = 4.0;
  acc_range_high = 9.0;
  double red_chi2_bound = 1.3;

  for(int i = 0; i < pars.size(); i++) {
    if(pars[i].red_chi2 < red_chi2_bound)
      maxsize++;
    x[i] = pars[i].theta;
    y[i] = pars[i].red_chi2;
    xerr[i] = 0.0;
    yerr[i] = 0.0;

    if(x[i] < acc_range_low)
      acc_range_low = x[i];
    if(x[i] > acc_range_high)
      acc_range_high = x[i];
  }
  acc_range_low = 0.004;
  acc_range_high = 0.009;

  TGraphErrors* graph = new TGraphErrors(pars.size(), x, y, xerr, yerr);

  /*TF1* line = new TF1("line", "[0]", acc_range_low, acc_range_high);
  line->SetParameter(0, 1.0);
  graph->Fit(line, "R");*/

/*  TCanvas c("c", "c", 1000, 700);
  TLine lline(graph->GetXaxis()->GetXmin(), red_chi2_bound, graph->GetXaxis()->GetXmax(), red_chi2_bound);
  graph->SetTitle("");
  graph->GetXaxis()->SetTitle("#Theta [mrad]");
  graph->GetYaxis()->SetTitle("#Chi^{2}/NDF");
  graph->Draw("A*");
  lline.SetLineColor(2);
  lline.Draw("same");
  c.SaveAs("red_chi2_vs_theta.pdf");
  c.SaveAs("red_chi2_vs_theta.eps");

  
  counter =0;
  for(int i = 0; i < pars.size(); i++) {
    if(pars[i].red_chi2 > red_chi2_bound)
      continue;
    x[counter] = pars[i].theta;
    y[counter] = pars[i].theta_mean;
    xerr[counter] = 0.0;
    yerr[counter] = pars[i].theta_mean_err;
    counter++;
  }  
  
  graph = new TGraphErrors(maxsize-1, &x[1], &y[1], &xerr[1], &yerr[1]);

  TF1* line = new TF1("line", "[0]+[1]*exp(-[2]*x)", 2.5, 10.0);
  line->SetParameters(0.1, 1.0, 0.1);
  graph->Fit(line, "R");

  //gStyle->SetOptFit(1111);
  graph->SetTitle("");
  graph->GetXaxis()->SetTitle("#Theta [mrad]");
  graph->GetYaxis()->SetTitle("#theta_{mean} [mrad]");
  graph->Draw("A*");
  line->SetLineColor(2);
  line->Draw("same");
  c.SaveAs("theta_mean_vs_theta.pdf");
  c.SaveAs("theta_mean_vs_theta.eps");


  counter =0;
  for(int i = 0; i < pars.size(); i++) {
    if(pars[i].red_chi2 > red_chi2_bound)
      continue;
    x[counter] = pars[i].theta;
    y[counter] = pars[i].sigma_gauss_wide;
    xerr[counter] = 0.0;
    yerr[counter] = pars[i].sigma_gauss_wide_err;
    counter++;
  }  
  
  graph = new TGraphErrors(maxsize, x, y, xerr, yerr);

  line = new TF1("line", "[0]+[1]*x", 3.0, 9.0);
  line->SetParameters(1.0, 0.1);
  graph->Fit(line, "R");

  //gStyle->SetOptFit(1111);
  graph->SetTitle("");
  graph->GetXaxis()->SetTitle("#Theta [mrad]");
  graph->GetYaxis()->SetTitle("#sigma_{wide} [mrad]");
  graph->Draw("A*");
  line->SetLineColor(2);
  line->Draw("same");
  c.SaveAs("sigma_gauss_wide_vs_theta.pdf");
  c.SaveAs("sigma_gauss_wide_vs_theta.eps");

  mean.sigma_gauss_wide = line->GetParameter(0);


  counter =0;
  for(int i = 0; i < pars.size(); i++) {
    if(pars[i].red_chi2 > red_chi2_bound)
      continue;

    x[counter] = pars[i].theta;
    y[counter] = pars[i].sigma_gauss_narrow;
    xerr[counter] = 0.0;
    yerr[counter] = pars[i].sigma_gauss_narrow_err;
    counter++;
  }

  graph = new TGraphErrors(maxsize, x, y, xerr, yerr);

  line = new TF1("line", "[0]+[1]*x", 3.0, 9.0);
  line->SetParameters(1.0, 0.1);
  graph->Fit(line, "R");

  graph->SetTitle("");
  graph->GetXaxis()->SetTitle("#Theta [mrad]");
  graph->GetYaxis()->SetTitle("#sigma_{narrow} [mrad]");
  graph->Draw("A*");
  line->SetLineColor(2);
  line->Draw("same");
  c.SaveAs("sigma_gauss_narrow_vs_theta.pdf");
  c.SaveAs("sigma_gauss_narrow_vs_theta.eps");


  mean.sigma_gauss_narrow = line->GetParameter(0);


  counter = 0;
  for(int i = 0; i < pars.size(); i++) {
    if(pars[i].red_chi2 > red_chi2_bound)
      continue;

    x[counter] = pars[i].theta;
    y[counter] = pars[i].ratio_gwgn;
    xerr[counter] = 0.0;
    yerr[counter] = pars[i].ratio_gwgn_err;
    counter++;
  }

  graph = new TGraphErrors(maxsize, x, y, xerr, yerr);
 
  line = new TF1("line", "[0]", 3.0, 9.0);
  line->SetParameters(0, 1.0);
  graph->Fit(line, "R");

  graph->SetTitle("");
  graph->GetXaxis()->SetTitle("#Theta [mrad]");
  graph->GetYaxis()->SetTitle("ratio gaus_{wide}/gaus_{narrow}");
  graph->Draw("A*");
  line->SetLineColor(2);
  line->Draw("same");
  c.SaveAs("ratio_wide_narrow_vs_theta.pdf");
  c.SaveAs("ratio_wide_narrow_vs_theta.eps");

  mean.ratio_gwgn = line->GetParameter(0);


  return mean;
}


std::pair<double, double> determineSlicedResolution(double mom, const char* dir) {
  TFile *file = new TFile(TString(dir)+"/plots.root", "OPEN");

  boost::filesystem::path curpath = boost::filesystem::current_path(); // get current path
  std::stringstream ss;
  ss << mom;
  std::string s = curpath.string() + "/" + ss.str();
  curpath = boost::filesystem::path(s);
  boost::filesystem::create_directory(curpath); // make new directory
  boost::filesystem::current_path(curpath); // change to this directory

  int bins_in_slice = 2;

  std::vector<fit_pars> pars;

	gStyle->SetPadBorderMode(0);
	gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetPadLeftMargin(0.13);
  gStyle->SetPadRightMargin(0.18);
  gStyle->SetPadBottomMargin(0.13);

  double textsize = 0.06;

  TGaxis::SetMaxDigits(3);

  TH2I* res_vs_th;
  file->GetObject("theta_resolution_vs_theta", res_vs_th);

  TCanvas c2d("c", "c", 1000, 700);
  res_vs_th->SetTitle("");
  res_vs_th->GetXaxis()->SetTitle("#theta_{reco}-#theta_{mc} [mrad]");
  res_vs_th->GetXaxis()->SetTitleSize(textsize);
  res_vs_th->GetXaxis()->SetLabelSize(textsize);
  //res_vs_th->GetXaxis()->SetRangeUser();
  res_vs_th->GetYaxis()->SetTitle("#theta_{mc} [mrad]");
  res_vs_th->GetYaxis()->SetTitleSize(textsize);
  res_vs_th->GetYaxis()->SetLabelSize(textsize);
  res_vs_th->GetYaxis()->SetRangeUser(2.0, 12.0);
  res_vs_th->Draw("colz");
  c2d.Update();
  TPaletteAxis *palette = (TPaletteAxis*)res_vs_th->GetListOfFunctions()->FindObject("palette");
  palette->GetAxis()->SetTitle("# of entries");
  palette->GetAxis()->SetTitleSize(textsize);
  palette->GetAxis()->SetLabelSize(textsize);
  palette->SetX1NDC(0.83);
  palette->SetX2NDC(0.88);
  // draw in examplary slice cut lines
  TLine *line1 = new TLine(-3.0, 3.29, 3.0, 3.29);
  TLine *line2 = new TLine(-3.0, 3.47, 3.0, 3.47);
  line1->SetLineColor(2);
  line2->SetLineColor(2);
  line1->Draw();
  line2->Draw();
  c2d.SaveAs("res_vs_theta_2d.pdf");
  c2d.SaveAs("res_vs_theta_2d.eps");
  //gStyle->SetOptStat(111);
  gStyle->SetOptFit(1111);
  gStyle->SetPadRightMargin(0.06);

  if(res_vs_th) {
    //make slices and fit with gaussian...
    //
    TCanvas c("c", "c", 1000, 700);
    c.Print("theta_res_booky.pdf[");

    TCanvas co("c", "c", 1000, 700);
    co.Divide(4,3);

    TF1* bestfit;
    double best_rchi2 = 1000.0;
    double parameters[5];

    int sigma=(bins_in_slice)/2;
    int first = 1;
    int padcounter = 0;
    for(int i = sigma+1; i < res_vs_th->GetYaxis()->GetNbins()-sigma-1; i=i+2*sigma) { // sigma+1 because of under and overflow bin
      TH1D* projx = res_vs_th->ProjectionX("projx", i-sigma, i+sigma);
      char title[30];
      sprintf(title, "#theta: %.2f-%.2f mrad", res_vs_th->GetYaxis()->GetBinCenter(i-sigma), res_vs_th->GetYaxis()->GetBinCenter(i+sigma));
      projx = (TH1D*)projx->Rebin(2, title);
      std::cout<< "plotting slice for bin range: "<<i-sigma<<" - "<<i+sigma<<std::endl;
      std::cout<< "or :" << title << std::endl;
      if(projx->GetEntries() > 1000) {
        std::cout<<"ok now actually doing something because this hist contains more than 5000 entries..."<<std::endl;
        padcounter++;
        TF1* gaus = new TF1("gaus", &doublegauss, res_vs_th->GetXaxis()->GetXmin(), res_vs_th->GetXaxis()->GetXmax(), 5);
        best_rchi2 = 1000.0;


        // ok first fit will scan parameter space so fit few times with different starting parameters and pick best chi2
        // also save the parameters
       // if( first == 1) {
          // take rms as approximation for sigma for starting values of the fit
          double sigma_approx = projx->GetRMS();
          double mean_approx = projx->GetMean();

          first = 0;
          for(int j = 0; j < 10; j++) {
            gaus->SetParameters(500.0, mean_approx, gRandom->Uniform(0.8*1.0, 1.2*1.0), 0.25, gRandom->Uniform(0.8*0.6, 1.2*0.6), mean_approx);
            gaus->SetParNames("A", "#theta_{mean1}", "#sigma_{1}", "ratio_{1/2}", "#sigma_{2}", "#theta_{mean2}");
            gaus->SetParLimits(0, 0.0, 1000000.0);
            gaus->SetParLimits(1, -0.2, 0.2);
            gaus->SetParLimits(2, 0.0, 3.0);
            gaus->SetParLimits(3, 0.0, 10.0);
            gaus->SetParLimits(4, 0.0, 3.0);
            gaus->SetParLimits(5, -0.2, 0.2);

            projx->Fit(gaus, "MBIRNE");
            if(gaus->GetChisquare()/gaus->GetNDF() < best_rchi2) {
              bestfit = gaus;
              best_rchi2 = gaus->GetChisquare()/gaus->GetNDF();
            }
            if(best_rchi2 < 1.1)
              break;
          }
          for(int j = 0; j < 5; j++) {
            parameters[j] = gaus->GetParameters()[j];
          }
          gaus = bestfit;
          projx->GetListOfFunctions()->Add(bestfit);
       /* }
        else {
          gaus->SetParameters(parameters[0], parameters[1], parameters[2], parameters[3], parameters[4]);
          gaus->SetParNames("A_{1}", "#theta_{mean}", "#sigma_{1}", "A_{2}", "#sigma_{2}");
          projx->Fit(gaus, "RIEM");
        }*/
/*        co.cd(padcounter);
        projx->Draw("PE");
        //bestfit->Draw("SAME");
        c.cd();
        projx->Draw("PE");
        projx->SetTitle("");
        //projx->GetXaxis()->SetTitle("");
        projx->GetYaxis()->SetTitle("# of entries");
        projx->GetYaxis()->SetLabelSize(textsize);
        projx->GetYaxis()->SetTitleSize(textsize);

        //TPaveStats *s = (TPaveStats*) gPad->GetPrimitive("stats");

        c.Print("theta_res_booky.pdf");
        

        // if(projx->GetEntries() > 0 && res_vs_th->GetYaxis()->GetBinCenter(i) > acc_range_low && res_vs_th->GetYaxis()->GetBinCenter(i) < acc_range_high) {

        // ok first gaussian we define to be the broader one

        fit_pars temp;
        temp.red_chi2 = gaus->GetChisquare()/gaus->GetNDF();
        temp.theta = res_vs_th->GetYaxis()->GetBinCenter(i);
        temp.theta_mean = gaus->GetParameter(1);
        temp.theta_mean_err = gaus->GetParError(1);
        double sigma_gauss_wide;
        double sigma_gauss_wide_err;
        double sigma_gauss_narrow;
        double sigma_gauss_narrow_err;
        double ratio_gwgn;
        double ratio_gwgn_err;
        if(gaus->GetParameter(2) > gaus->GetParameter(4)) {
          temp.sigma_gauss_wide = gaus->GetParameter(2);
          temp.sigma_gauss_wide_err = gaus->GetParError(2);
          temp.sigma_gauss_narrow = gaus->GetParameter(4);
          temp.sigma_gauss_narrow_err = gaus->GetParError(4);
          temp.ratio_gwgn = gaus->GetParameter(3);
          temp.ratio_gwgn_err = gaus->GetParError(3);
        }
        else {
          temp.sigma_gauss_wide = gaus->GetParameter(4);
          temp.sigma_gauss_wide_err = gaus->GetParError(4);
          temp.sigma_gauss_narrow = gaus->GetParameter(2);
          temp.sigma_gauss_narrow_err = gaus->GetParError(2);
          temp.ratio_gwgn = 1/gaus->GetParameter(3);
          temp.ratio_gwgn_err = gaus->GetParError(3)/gaus->GetParameter(3);
        }

        pars.push_back(temp);
      }
      //}
    }
    co.SaveAs("theta_res_overview.pdf");
    c.Print("theta_res_booky.pdf]");

    fit_pars results = parametersVsTheta(pars);
    std::cout<< std::endl;
    std::cout<< "***************************************" << std::endl;
    std::cout<< "Fit results for beam energy: "<< mom << std::endl;
    std::cout<< "mean sigma of wide gauss: "<< results.sigma_gauss_wide << std::endl;
    std::cout<< "mean sigma of narrow gauss: "<< results.sigma_gauss_narrow << std::endl;
    std::cout<< "mean ratio of (wide gauss sigma)/(narrow gauss sigma): "<< results.ratio_gwgn << std::endl;
    std::cout<< "***************************************" << std::endl;

  }
  return std::make_pair(-1.0, -1.0);
}


std::pair<double, double> determineSingleResolution(double mom, const char* dir) {
  TFile *file = new TFile(TString(dir)+"/plots.root", "OPEN");

  boost::filesystem::path curpath = boost::filesystem::current_path(); // get current path
  std::stringstream ss;
  ss << mom;
  std::string s = curpath.string() + "/" + ss.str();
  curpath = boost::filesystem::path(s);
  boost::filesystem::create_directory(curpath); // make new directory
  boost::filesystem::current_path(curpath); // change to this directory

  int bins_in_slice = 10;

  double acc_range_low = 1.0, acc_range_high = -1.0;
  std::vector<std::pair<double, double> > resolutions;
  std::vector<std::pair<double, double> > resolution_errs;


  TH1I* res;
  file->GetObject("phi_resolution", res);

  if(res) {
    //make slices and fit with gaussian...
    //
    TCanvas *c = new TCanvas("c", "c", 1000, 700);
    c->Print("theta_res_overview.ps[");

    TF1* gaus = new TF1("gaus", "gaus", res->GetXaxis()->GetXmin(), res->GetXaxis()->GetXmax());
    gaus->SetParameters(10.0, 0.0, 0.1);
    res->Fit(gaus, "RI");

    res->Draw();
    c->SaveAs("phires.pdf");

    return std::make_pair(gaus->GetParameter(2), gaus->GetParError(2));
  }

  return std::make_pair(-1.0, -1.0);
}
*/

int main(int argc, char* argv[]) {
  if(argc == 1) {
    std::cout<<"./determineResolution dir1 plab1 dir2 plab2 dir3 plab3 ..."<<std::endl;
  }
  else if(argc%2 == 1) {
    std::map<double, std::string> args;
    for(int i = 1; i < argc; i=i+2) {
      double mom = atof(argv[i+1]);
      args[mom]=argv[i];
    }
    determineResolution(args);
  }
  else {
    std::cout<<"You did not follow input convention!!"<<std::endl;
    std::cout<<"./determineResolution dir1 plab1 dir2 plab2 dir3 plab3 ..."<<std::endl;
  }
  return 0;
}
