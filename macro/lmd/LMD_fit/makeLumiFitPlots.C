void makeLumiFitPlots(TString path) {
  std::cout << "Generating lumi plots for fit results....\n";

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdFit");

  PndLmdResultPlotter plotter;

  // PndLmdResultPlotter sets default pad margins etc that should be fine for most cases
  // you can fine tune it by changing the bottom stuff
  /*gStyle->SetPadTopMargin(0.06);
   gStyle->SetPadBottomMargin(0.12);
   gStyle->SetPadLeftMargin(0.14);
   gStyle->SetPadRightMargin(0.1);*/

  plotter.setThetaPlotRange(1.0, 18.0);
  //plotter.setTextLeftPos(0.20);
  //plotter.setTextTopPos(0.98);
  //plotter.setTextSpacing(0.08);
  //plotter.setTextSize(0.05);
  //plotter.setLabelSize(0.05);

  //plotter.setLabelOffsetX(0.007);
  //plotter.setLabelOffsetY(0.007);
  //plotter.setTitleOffsetX(1.0);
  //plotter.setTitleOffsetY(1.5);

  std::vector<PndLmdData*> data_vec = plotter.getDataFromPath(path);

  // if you only have a single data object (mostly the case)
  if (data_vec.size() > 0) {
    //std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> > &fit_map = data_vec[0]->getFitMap();
    std::vector<PndLmdAcceptance*> accs = data_vec[0]->getListOfAcceptances();
    if (accs.size() > 0) {
      std::vector<PndLmdResultPlotter::graph_bundle_1D> graph_bundles =
          plotter.makeGraphBundles1D(data_vec[0], accs[0]);

      PndLmdResultPlotter::acceptance_bundle_1D acceptance_bundle =
          plotter.makeAcceptanceBundle1D(accs[0]);

      TCanvas *c = plotter.makeOverviewCanvas(graph_bundles, acceptance_bundle);
      c->SaveAs("fitresults_overview.pdf");
    }
  }
}

/*
 for (int i = 0; i < fit_res.size(); i++) {
 if (fit_res[i]->getLumiFitOptions()->getBinaryOptions()
 == fitop_normal.getBinaryOptions()) {
 c->cd(k + 1);
 plotter.fillSinglePad(c, vec_lmd_data[0], iter->first, fit_res[i],
 "fit_reco", lumi_ref, rangel, rangeh, 0, 0, 1, 0.45, 0.98);
 double lumi = fit_res[i]->getLuminosity();
 double lumi_err = fit_res[i]->getLuminosityError();
 x[k] = k + 1;
 y[k] = plotter.calulateLumiRelDiff(lumi, lumi_err, lumi_ref).first;
 y_errs[k] =
 plotter.calulateLumiRelDiff(lumi, lumi_err, lumi_ref).second;
 proj->Fill(y[k]);
 }*/

/*int main(int argc, char *argv[]) {
 if (argc == 2) {
 makeComparisonPlots(argv[1]);
 } else if (argc > 2) {
 std::vector<std::string> paths;
 for (int i = 1; i < argc; i++)
 paths.push_back(std::string(argv[i]));
 makeComparisonCanvas(paths);
 }
 }*/
