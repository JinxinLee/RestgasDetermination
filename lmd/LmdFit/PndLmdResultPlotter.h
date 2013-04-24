/*
 * PndLmdResultPlotter.h
 *
 *  Created on: Mar 25, 2013
 *      Author: steve
 */

#ifndef PNDLMDRESULTPLOTTER_H_
#define PNDLMDRESULTPLOTTER_H_

#include <map>

#include "TString.h"

class PndLmdData;
class PndLmdAcceptance;
class PndLmdLumiFitResult;
class PndLmdLumiFitOptions;

class TH1D;
class TGraph;
class TGraphErrors;
class TGraphAsymmErrors;
class TCanvas;

class PndLmdResultPlotter {
public:
  struct combined_values {
    double xval;
    double lumival;
    double lumierr;
    double lumiref;
  };

  struct graph_bundle_1D {
    TGraphErrors *model;
    TH1D* data_hist;
    double plab;
    double ref_lumi;
    combined_values lumi_values;
    double chi2;
    PndLmdLumiFitOptions* fit_options;
  };

  struct acceptance_bundle_1D {
    TGraphAsymmErrors *acceptance;
    double plab;
    bool is_angular;
  };

private:
  double theta_plot_range_low;
  double theta_plot_range_high;
  double text_leftpos;
  double text_toppos;
  double text_spacing;
  double text_size;
  double text_size2;

  double label_offset_x;
  double label_offset_y;
  double title_offset_x;
  double title_offset_y;

  std::vector<PndLmdResultPlotter::combined_values> calcDiff(
      std::vector<combined_values> &vals);

  void unifyPlotRange(TH1D* hist, TGraph *graph);

  std::pair<double, double> determinePlotRange(
      std::map<TString, std::vector<combined_values> > &result_map);
  std::pair<double, double> determineDiffPlotRange(
      std::map<TString, std::vector<combined_values> > &result_map);

  TGraphErrors* makeGraph(std::vector<combined_values> data, int color);

  double calculateYPos(double line, double text_toppos, double text_spacing,
      bool log);

  TGraphErrors* createGraphFromFitResult(PndLmdLumiFitResult *fit_res,
      PndLmdAcceptance *acc, PndLmdData *data);

  std::pair<double, double> calculatePlotRange(PndLmdData *data,
      PndLmdLumiFitOptions *fit_options);

public:
  PndLmdResultPlotter();
  virtual ~PndLmdResultPlotter();

  void setThetaPlotRange(double theta_plot_range_low_,
      double theta_plot_range_high_);
  void setTextLeftPos(double text_leftpos_);
  void setTextTopPos(double text_toppos_);
  void setTextSpacing(double text_spacing_);
  void setTextSize(double text_size_);
  void setLabelSize(double text_size2_);

  void setLabelOffsetX(double label_offset_x_);
  void setLabelOffsetY(double label_offset_y_);
  void setTitleOffsetX(double title_offset_x_);
  void setTitleOffsetY(double title_offset_y_);

  std::vector<PndLmdData*> getDataFromPath(TString path);

  std::vector<PndLmdResultPlotter::graph_bundle_1D> makeGraphBundles1D(
      PndLmdData *data, PndLmdAcceptance* acc);

  acceptance_bundle_1D makeAcceptanceBundle1D(PndLmdAcceptance* acc,
      bool is_momentum_transfer = false);

  std::pair<double, double> calulateLumiRelDiff(double lumi, double lumi_err,
      double lumi_ref);

  void fillAcceptanceInPad(TCanvas *c, acceptance_bundle_1D acceptance_bundle);

  void fillSinglePad(TCanvas *c, graph_bundle_1D graph_bundle,
      const TString &label, bool log_scale, bool labels_on = true);

  combined_values createCombinedValue(double x, double lumi, double err,
      double ref);

  TCanvas* makeOverviewCanvas(
      std::vector<PndLmdResultPlotter::graph_bundle_1D> &graph_bundles,
      acceptance_bundle_1D &acceptance_bundle);

  void makeComparisonCanvas(
      TString name,
      std::map<TString, std::vector<PndLmdResultPlotter::combined_values> >& result_map);
};

#endif /* PNDLMDRESULTPLOTTER_H_ */
