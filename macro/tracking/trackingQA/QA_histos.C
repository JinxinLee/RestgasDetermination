/**
   @class QA_histos.C
   @brief Quality Assurance histogram macro
   @author Lia Lavezzi <lia.lavezzi@to.infn.it>, University of Torino
   @author Stefano Spataro <stefano.spataro@to.infn.it>, University of Torino
   @version 1.0
   @date Jun 15, 2015
**/

void LabelQualyHistogram(TH1 * hist) {
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(1), "Spurious found");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(2), "Partially found");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(3), "Fully found");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(-1), "Not found secondary");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(-2), "Not found primary");
	hist->GetXaxis()->SetBinLabel(hist->FindFixBin(-3), "Found");
}

int QA_histos(TString prefix, TString trackBranch, Bool_t forward = kFALSE) {

	PndFileNameCreator creator(prefix.Data());
	TString extension;
	if (forward == kFALSE)
		extension = "trackingQA_";
	else extension = "trackingQAfwd_";
	extension.Append(trackBranch);
	TString simFile = creator.GetSimFileName();
	TString trackQAFile = creator.GetCustomFileName(extension.Data());
	extension.Append("_histos");
	TString outputFile = creator.GetCustomFileName(extension.Data());

	std::cout << "InFileName: " << simFile.Data() << std::endl;
	std::cout << "TrackQAFile: " << trackQAFile.Data() << std::endl;
	std::cout << "OutputFileName: " << outputFile.Data() << std::endl;

  TFile fileqa(trackQAFile.Data());
  TTree *simtree = (TTree*) fileqa.Get("pndsim");
  std::cout << "SimTree: " << simtree << std::endl;
  std::cout << "SimTree.GetEntries() " << simtree->GetEntries() << std::endl;
  simtree->AddFriend("pndsim", simFile.Data());

  TCut cut = "RecoTrackInfo.GetQuality() > 0";

  /**
     7 histos: Global efficiency (#true hits/#totaltruehits), for all the primary tracks, for all the detectors:
     Efficiency for MVD pixel
     Eff for MVD strips
     Eff for Stt parallel
     Eff for Stt skewed
     Eff for gem
     Eff for FTS (skewed? parallel?)
  **/

  double effmin = -0.1;
  double effmax = 1.1;

  TH1F *eff_mvdpix = new TH1F("eff_mvdpix", "MVD pixel efficiency", 100, effmin, effmax);
  TH1F *eff_mvdstr = new TH1F("eff_mvdstr", "MVD strip efficiency", 100, effmin, effmax);
  TH1F *eff_stt = new TH1F("eff_stt", "STT efficiency", 100, effmin, effmax);
  TH1F *eff_gem = new TH1F("eff_gem", "GEM efficiency", 100, effmin, effmax);
  TH1F *eff_fts = new TH1F("eff_fts", "FTS efficiency", 100, effmin, effmax);


  std::cout << "Before first Draw!" << std::endl;

  simtree->Draw("RecoTrackInfo.GetMvdPixelEfficiency() >> eff_mvdpix", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetMvdStripEfficiency() >> eff_mvdstr", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetSttEfficiency() >> eff_stt", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetGemEfficiency() >> eff_gem", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetFtsEfficiency() >> eff_fts", cut, "goff");
//
  TH1F *eff_glo = new TH1F("eff_glo", "global efficiency", 100, effmin, effmax);
  simtree->Draw("RecoTrackInfo.GetEfficiency() >> eff_glo", cut, "goff");

  std::cout << "Efficiencies filled!" << std::endl;
  /**
     7 histos: Global Purity (#true hits/#hits), for all the primary tracks, for all the detectors:
     Purity for MVD pixel
     Pur for MVD strips
     Pur for Stt parallel
     Pur for Stt skewed
     Pur for gem
     Pur for FTS (skewed? parallel?)
  **/
  double purmin = -0.1;
  double purmax = 1.1;

  TH1F *pur_mvdpix = new TH1F("pur_mvdpix", "MVD pixel purity", 100, purmin, purmax);
  TH1F *pur_mvdstr = new TH1F("pur_mvdstr", "MVD strip purity", 100, purmin, purmax);
  TH1F *pur_stt = new TH1F("pur_stt", "STT purity", 100, purmin, purmax);
  TH1F *pur_gem = new TH1F("pur_gem", "GEM purity", 100, purmin, purmax);
  TH1F *pur_fts = new TH1F("pur_fts", "FTS purity", 100, purmin, purmax);

  simtree->Draw("RecoTrackInfo.GetMvdPixelPurity() >> pur_mvdpix", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetMvdStripPurity() >> pur_mvdstr", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetSttPurity() >> pur_stt", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetGemPurity() >> pur_gem", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetFtsPurity() >> pur_fts", cut, "goff");


  TH1F *pur_glo = new TH1F("pur_glo", "global purity", 100, purmin, purmax);
  simtree->Draw("RecoTrackInfo.GetPurity() >> pur_glo", cut, "goff");

  std::cout << "Purities filled!" << std::endl;
  /**
     2 histos:
     Number of reco tracks correlated to MC track, for primaries (>1 -> clones)
     Number of MC tracks correlated to reco tracks for primaries (>1 -> broken tracks
  **/
  TH1F *hnofrecotracks = new TH1F("hnofrecotracks", "# of reco tracks associated to the same MC track", 20, 0, 20);
  simtree->Draw("MCTrackInfo.GetNofRecoTracks() >> hnofrecotracks", cut, "goff");

  TH1F *hnofMCtracks = new TH1F("hnofMCtracks", "# of MC tracks associated to the same reco track", 20, 0, 20);
  simtree->Draw("RecoTrackInfo.GetNofMCTracks() >> hnofMCtracks", cut, "goff");

  std::cout << "RecoTracks filled!" << std::endl;

  /**
     7 histos: Resolution at the first point:
     delta_p
     delta_pz
     delta_pperp
     delta_theta
     delta_phi
     delta_position
  **/
  double pmin = -1;
  double pmax = 1;
  TH1F *hdelta_p_first = new TH1F("hdelta_p_first", "#Delta p @ first hit", 100, pmin, pmax);
  TH1F *hdelta_pz_first = new TH1F("hdelta_pz_first", "#Delta pz @ first hit", 100, pmin, pmax);
  TH1F *hdelta_pt_first = new TH1F("hdelta_pt_first", "#Delta pt @ first hit", 100, pmin, pmax);

  simtree->Draw("RecoTrackInfo.GetMomentumFirst().Mag() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumFirst().Mag() >> hdelta_p_first", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetMomentumFirst().Perp() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumFirst().Perp() >> hdelta_pt_first", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetMomentumFirst().Z() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumFirst().Z() >> hdelta_pz_first", cut, "goff");

  double thetamin = -5;
  double thetamax = 5;
  int ntheta = (thetamax - thetamin)*10;
  TH1F *hdelta_theta_first = new TH1F("hdelta_theta_first", "#Delta #theta @ first hit", ntheta, thetamin, thetamax);
  simtree->Draw("(RecoTrackInfo.GetMomentumFirst().Theta() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumFirst().Theta()) * TMath::RadToDeg() >> hdelta_theta_first", cut, "goff");

  double phimin = -5;
  double phimax = 5;
  int nphi = (phimax - phimin)*10;
  TH1F *hdelta_phi_first = new TH1F("hdelta_phi_first", "#Delta #phi @ first hit", nphi, phimin, phimax);
  simtree->Draw("(RecoTrackInfo.GetMomentumFirst().Phi() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumFirst().Phi()) * TMath::RadToDeg() >> hdelta_phi_first", cut, "goff");


  double xmin = -0.2;
  double xmax = 0.2;
  TH1F *hdelta_x_first = new TH1F("hdelta_x_first", "#Delta x @ first hit", 100, xmin, xmax);
  TH1F *hdelta_y_first = new TH1F("hdelta_y_first", "#Delta y @ first hit", 100, xmin, xmax);
  simtree->Draw("RecoTrackInfo.GetPositionFirst().X() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetPositionFirst().X() >> hdelta_x_first", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetPositionFirst().Y() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetPositionFirst().Y() >> hdelta_y_first", cut, "goff");

  double zmin = -0.2;
  double zmax = 0.2;
  TH1F *hdelta_z_first = new TH1F("hdelta_z_first", "#Delta z @ first hit", 100, zmin, zmax);
  simtree->Draw("RecoTrackInfo.GetPositionFirst().Z() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetPositionFirst().Z() >> hdelta_z_first", cut, "goff");

  double rmax = xmax;
  TH1F *hdelta_r_first = new TH1F("hdelta_r_first", "#Delta r @ first hit", 100, -rmax, rmax);
  simtree->Draw("RecoTrackInfo.GetPositionFirst().Perp() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetPositionFirst().Perp() >> hdelta_r_first", cut, "goff");

  std::cout << "DeltaPFirst filled!" << std::endl;

  /**
     7 histos: Resolution at the last point:
     delta_p
     delta_pz
     delta_pperp
     delta_theta
     delta_phi
     delta_position
  **/
  TH1F *hdelta_p_last = new TH1F("hdelta_p_last", "#Delta p @ last hit", 100, pmin, pmax);
  TH1F *hdelta_pz_last = new TH1F("hdelta_pz_last", "#Delta pz @ last hit", 100, pmin, pmax);
  TH1F *hdelta_pt_last = new TH1F("hdelta_pt_last", "#Delta pt @ last hit", 100, pmin, pmax);

  simtree->Draw("RecoTrackInfo.GetMomentumLast().Mag() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumLast().Mag() >> hdelta_p_last", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetMomentumLast().Perp() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumLast().Perp() >> hdelta_pt_last", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetMomentumLast().Z() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumLast().Z() >> hdelta_pz_last", cut, "goff");

  TH1F *hdelta_theta_last = new TH1F("hdelta_theta_last", "#Delta #theta @ last hit", ntheta, thetamin, thetamax);
  simtree->Draw("(RecoTrackInfo.GetMomentumLast().Theta() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumLast().Theta()) * TMath::RadToDeg() >> hdelta_theta_last", cut, "goff");

  TH1F *hdelta_phi_last = new TH1F("hdelta_phi_last", "#Delta #phi @ last hit", nphi, phimin, phimax);
  simtree->Draw("(RecoTrackInfo.GetMomentumLast().Phi() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetMomentumLast().Phi()) * TMath::RadToDeg() >> hdelta_phi_last", cut, "goff");

  TH1F *hdelta_x_last = new TH1F("hdelta_x_last", "#Delta x @ last hit", 100, xmin, xmax);
  TH1F *hdelta_y_last = new TH1F("hdelta_y_last", "#Delta y @ last hit", 100, xmin, xmax);
  simtree->Draw("RecoTrackInfo.GetPositionLast().X() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetPositionLast().X() >> hdelta_x_last", cut, "goff");
  simtree->Draw("RecoTrackInfo.GetPositionLast().Y() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetPositionLast().Y() >> hdelta_y_last", cut, "goff");

  double zminlast = -0.6;
  double zmaxlast = 0.6;
  TH1F *hdelta_z_last = new TH1F("hdelta_z_last", "#Delta z @ last hit", 100, zminlast, zmaxlast);
  simtree->Draw("RecoTrackInfo.GetPositionLast().Z() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetPositionLast().Z() >> hdelta_z_last", cut, "goff");

  TH1F *hdelta_r_last = new TH1F("hdelta_r_last", "#Delta r @ last hit", 100, -rmax, rmax);
  simtree->Draw("RecoTrackInfo.GetPositionLast().Perp() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetPositionLast().Perp() >> hdelta_r_last", cut, "goff");

  std::cout << "DeltaPLast filled!" << std::endl;
  /**
     delta_charge
  **/
  double chargemin = -3;
  double chargemax = 3;
  int ncharge = chargemax - chargemin;
  TH1F *hdelta_charge = new TH1F("hdelta_charge", "#Delta #charge", ncharge, chargemin, chargemax);
  simtree->Draw("RecoTrackInfo.GetCharge() - MCTrackInfo[RecoTrackInfo.GetIdealTrackId()].GetCharge() >> hdelta_charge", cut, "goff");

  /**
      hit efficiency vs theta
  **/
  TH1F *hthetagen = new TH1F("hthetagen", "mc theta dist", 180, 0, 180);
  TH1F *heffintheta = new TH1F("heffintheta", "hit efficiency vs #theta", 180, 0, 180);

  TCut cut_mc = "";
  TCut cut_rec = cut_mc && "MCTrackInfo.GetRecoTrackID() != -1";
  cut_rec = cut_rec && "RecoTrackInfo[MCTrackInfo.GetRecoTrackID()].GetEfficiency() > 0.8";
  cut_rec = cut_rec && "RecoTrackInfo[MCTrackInfo.GetRecoTrackID()].IsClone() == 0";

  TCut cut_mc_prim = cut_mc && "MCTrackInfo.GetMCQuality() == -2";
  TCut cut_mc_sec = cut_mc && "MCTrackInfo.GetMCQuality() == -1";
  TCut cut_rec_prim = cut_rec && cut_mc_prim;
  TCut cut_rec_sec = cut_rec && cut_mc_sec;

  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> hthetagen", cut_mc, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> heffintheta", cut_rec, "goff");

  hthetagen->Sumw2();
  heffintheta->Sumw2();
  heffintheta->Divide(hthetagen);


  /**
      hit efficiency vs phi
  **/
  TH1F *hphigen = new TH1F("hphigen", "mc phi dist", 180, -180, 180);
  TH1F *heffinphi = new TH1F("heffinphi", "hit efficiency vs #phi", 180, -180, 180);
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> hphigen", cut_mc, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> heffinphi", cut_rec, "goff");

  hphigen->Sumw2();
  heffinphi->Sumw2();
  heffinphi->Divide(hphigen);

  /**
   * GetMaximum/mimimum Pt, Pl, P as range for histograms
   */

  TH1F *hMcPDist = new TH1F("hMcPDist","Momentum Distribution", 30,0,15);
  TH1F *hMcPtDist = new TH1F("hMcPtDist","Transv. Momentum Distribution", 30,0,15);
  TH1F *hMcPlDist = new TH1F("hMcPlDist","Long. Momentum Distribution", 30,0,15);

  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Mag()>>hMcPDist","","goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp()>>hMcPtDist","","goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z()>>hMcPlDist","","goff");

  Float_t maxP = hMcPDist->GetBinLowEdge(hMcPDist->FindLastBinAbove(0)) + hMcPDist->GetBinWidth(hMcPDist->FindLastBinAbove(0));
  Float_t maxPt = hMcPtDist->GetBinLowEdge(hMcPtDist->FindLastBinAbove(0)) + hMcPtDist->GetBinWidth(hMcPtDist->FindLastBinAbove(0));
  Float_t maxPl = hMcPlDist->GetBinLowEdge(hMcPlDist->FindLastBinAbove(0)) + hMcPlDist->GetBinWidth(hMcPlDist->FindLastBinAbove(0));
  Float_t minPl = hMcPlDist->GetBinLowEdge(hMcPlDist->FindFirstBinAbove(0)) + hMcPlDist->GetBinWidth(hMcPlDist->FindFirstBinAbove(0));

  if (minPl > 0) minPl = 0;
  Int_t Pres = 20;

  std::cout << "MaxP = " << maxP << " : MaxPt = " << maxPt << " : minPl = " << minPl << " : maxPl = " << maxPl << " : P res = " << Pres << std::endl;


  /**
      hit efficiency vs mom
  **/
  TH1F *hmomgen = new TH1F("hmomgen", "mc mom dist", maxP*Pres, 0, maxP);
  TH1F *heffinmom = new TH1F("heffinmom", "hit efficiency vs mom", maxP*Pres, 0, maxP);
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Mag() >> hmomgen", cut_mc, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Mag() >> heffinmom", cut_rec, "goff");

  hmomgen->Sumw2();
  heffinmom->Sumw2();
  heffinmom->Divide(hmomgen);

  /**
      hit efficiency vs pt
  **/
  TH1F *hptgen = new TH1F("hptgen", "mc pt dist", maxPt * Pres, 0, maxPt);
  TH1F *heffinpt = new TH1F("heffinpt", "hit efficiency vs pt", maxPt * Pres, 0, maxPt);

  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> hptgen", cut_mc, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> heffinpt", cut_rec, "goff");

  hptgen->Sumw2();
  heffinpt->Sumw2();
  heffinpt->Divide(hptgen);

  TH1F *hptgenprim = new TH1F("hptgenprim", "mc pt dist prim", maxPt * Pres, 0, maxPt);
  TH1F *heffinptprim = new TH1F("heffinptprim", "hit efficiency vs pt prim", maxPt * Pres, 0, maxPt);

  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> hptgenprim", cut_mc_prim, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> heffinptprim", cut_rec_prim, "goff");

  hptgenprim->Sumw2();
  heffinptprim->Sumw2();
  heffinptprim->Divide(hptgenprim);

  TH1F *hptgensec = new TH1F("hptgensec", "mc pt dist sec", maxPt * Pres, 0, maxPt);
  TH1F *heffinptsec = new TH1F("heffinptsec", "hit efficiency vs pt sec", maxPt * Pres, 0, maxPt);

  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> hptgensec", cut_mc_sec, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> heffinptsec", cut_rec_sec, "goff");

  hptgensec->Sumw2();
  heffinptsec->Sumw2();
  heffinptsec->Divide(hptgensec);

  /**
      hit efficiency vs pl
  **/
  TH1F *hplgen = new TH1F("hplgen", "mc pl dist", (maxPl - minPl)*Pres, minPl, maxPl);
  TH1F *heffinpl = new TH1F("heffinpl", "hit efficiency vs pl", (maxPl - minPl)*Pres, minPl, maxPl);
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> hplgen", cut_mc, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> heffinpl", cut_rec, "goff");

  hplgen->Sumw2();
  heffinpl->Sumw2();
  heffinpl->Divide(hplgen);

  /**
   *    Quality plots for primary and secondary particles
   */

  TH1F* hQualityPrim = new TH1F("hQualityPrim", "Quality for possible primary tracks", 8, -3.5, 4.5);
  TH1F* hQualitySec = new TH1F("hQualitySec", "Quality for possible secondary tracks", 8, -3.5, 4.5);
  hQualityPrim->SetStats(kFALSE);
  hQualitySec->SetStats(kFALSE);


  LabelQualyHistogram(hQualityPrim);
  LabelQualyHistogram(hQualitySec);

  simtree->Draw("MCTrackInfo.GetQuality()>>hQualityPrim", cut_mc_prim,"goff");
  simtree->Draw("MCTrackInfo.GetQuality()>>hQualitySec", cut_mc_sec,"goff");

 // std::cout << "Primary Tracks: " << nPrim << " Secondary Tracks: " << nSec << std::endl;
  hQualityPrim->Scale(1.0/hQualityPrim->Integral());
  hQualitySec->Scale(1.0/hQualitySec->Integral());
//  NormalizeHistogram(hQualityPrim);
//  NormalizeHistogram(hQualitySec);
  hQualityPrim->SetBinContent(1, 1.0 - hQualityPrim->GetBinContent(2));
  hQualitySec->SetBinContent(1, 1.0 - hQualitySec->GetBinContent(3));




 // constant->SetParameter(0, nPrim);
 // hQualityPrim->Divide(constant, nPrim);
 // constant->SetParameter(0, nSec);
 // hQualitySec->Divide(constant, nSec);

  /**
   * track finding efficiency vs. theta
   */
    cut_rec = "MCTrackInfo.GetQuality() > 0";

    cut_mc_prim = cut_mc && "MCTrackInfo.GetMCQuality() == -2";
    cut_mc_sec = cut_mc && "MCTrackInfo.GetMCQuality() == -1";
    cut_rec_prim = cut_rec && cut_mc_prim;
    cut_rec_sec = cut_rec && cut_mc_sec;

  TH1F* htfeffintheta = new TH1F("htfeffintheta", "Track finding efficiency vs theta", 180, 0, 180);
  TH1F* hmctfeffintheta = new TH1F("hmctfeffintheta", "Track finding efficiency vs theta", 180, 0, 180);
  htfeffintheta->Sumw2();
  hmctfeffintheta->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> htfeffintheta", cut_rec, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> hmctfeffintheta", cut_mc, "goff");
  htfeffintheta->Divide(hmctfeffintheta);

  TH1F *htfeffinthetaprim = new TH1F("htfeffinthetaprim", "Track finding efficiency vs theta prim", 180, 0, 180);
  TH1F *hmctfeffinthetaprim = new TH1F("hmctfeffinthetaprim", "Track finding efficiency vs theta prim", 180, 0, 180);
  htfeffinthetaprim->Sumw2();
  hmctfeffinthetaprim->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> htfeffinthetaprim", cut_rec_prim, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> hmctfeffinthetaprim", cut_mc_prim, "goff");
  htfeffinthetaprim->Divide(hmctfeffinthetaprim);

  TH1F *htfeffinthetasec = new TH1F("htfeffinthetasec", "Track finding efficiency vs theta sec", 180, 0, 180);
  TH1F *hmctfeffinthetasec = new TH1F("hmctfeffinthetasec", "Track finding efficiency vs theta sec", 180, 0, 180);
  htfeffinthetasec->Sumw2();
  hmctfeffinthetasec->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> htfeffinthetasec", cut_rec_sec, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> hmctfeffinthetasec", cut_mc_sec, "goff");
  htfeffinthetasec->Divide(hmctfeffinthetasec);

  /**
     * track finding efficiency vs. phi
     */
      cut_rec = "MCTrackInfo.GetQuality() > 0";

      cut_mc_prim = cut_mc && "MCTrackInfo.GetMCQuality() == -2";
      cut_mc_sec = cut_mc && "MCTrackInfo.GetMCQuality() == -1";
      cut_rec_prim = cut_rec && cut_mc_prim;
      cut_rec_sec = cut_rec && cut_mc_sec;

    TH1F* htfeffinphi = new TH1F("htfeffinphi", "Track finding efficiency vs phi", 180, -180, 180);
    TH1F* hmctfeffinphi = new TH1F("hmctfeffinphi", "Track finding efficiency vs phi", 180, -180, 180);
    htfeffinphi->Sumw2();
    hmctfeffinphi->Sumw2();
    simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> htfeffinphi", cut_rec, "goff");
    simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> hmctfeffinphi", cut_mc, "goff");
    htfeffinphi->Divide(hmctfeffinphi);

    TH1F *htfeffinphiprim = new TH1F("htfeffinphiprim", "Track finding efficiency vs phi prim", 180, -180, 180);
    TH1F *hmctfeffinphiprim = new TH1F("hmctfeffinphiprim", "Track finding efficiency vs phi prim", 180, -180, 180);
    htfeffinphiprim->Sumw2();
    hmctfeffinphiprim->Sumw2();
    simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> htfeffinphiprim", cut_rec_prim, "goff");
    simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> hmctfeffinphiprim", cut_mc_prim, "goff");
    htfeffinphiprim->Divide(hmctfeffinphiprim);

    TH1F *htfeffinphisec = new TH1F("htfeffinphisec", "Track finding efficiency vs phi sec", 180, -180, 180);
    TH1F *hmctfeffinphisec = new TH1F("hmctfeffinphisec", "Track finding efficiency vs phi sec", 180, -180, 180);
    htfeffinphisec->Sumw2();
    hmctfeffinphisec->Sumw2();
    simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> htfeffinphisec", cut_rec_sec, "goff");
    simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> hmctfeffinphisec", cut_mc_sec, "goff");
    htfeffinphisec->Divide(hmctfeffinphisec);

  /**
   * track finding efficiency vs. pt
   */
    cut_rec = "MCTrackInfo.GetQuality() > 0";

    cut_mc_prim = cut_mc && "MCTrackInfo.GetMCQuality() == -2";
    cut_mc_sec = cut_mc && "MCTrackInfo.GetMCQuality() == -1";
    cut_rec_prim = cut_rec && cut_mc_prim;
    cut_rec_sec = cut_rec && cut_mc_sec;

  TH1F* htfeffinpt = new TH1F("htfeffinpt", "Track finding efficiency vs pt", maxPt * Pres, 0, maxPt);
  TH1F* hmctfeffinpt = new TH1F("hmctfeffinpt", "Track finding efficiency vs pt", maxPt * Pres, 0, maxPt);
  htfeffinpt->Sumw2();
  hmctfeffinpt->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> htfeffinpt", cut_rec, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> hmctfeffinpt", cut_mc, "goff");
  htfeffinpt->Divide(hmctfeffinpt);

  TH1F *htfeffinptprim = new TH1F("htfeffinptprim", "Track finding efficiency vs pt prim", maxPt * Pres, 0, maxPt);
  TH1F *hmctfeffinptprim = new TH1F("hmctfeffinptprim", "Track finding efficiency vs pt prim", maxPt * Pres, 0, maxPt);
  htfeffinptprim->Sumw2();
  hmctfeffinptprim->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> htfeffinptprim", cut_rec_prim, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> hmctfeffinptprim", cut_mc_prim, "goff");
  htfeffinptprim->Divide(hmctfeffinptprim);

  TH1F *htfeffinptsec = new TH1F("htfeffinptsec", "Track finding efficiency vs pt sec", maxPt * Pres, 0, maxPt);
  TH1F *hmctfeffinptsec = new TH1F("hmctfeffinptsec", "Track finding efficiency vs pt sec", maxPt * Pres, 0, maxPt);
  htfeffinptsec->Sumw2();
  hmctfeffinptsec->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> htfeffinptsec", cut_rec_sec, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> hmctfeffinptsec", cut_mc_sec, "goff");
  htfeffinptsec->Divide(hmctfeffinptsec);

  /**
   * track finding efficiency vs. pl
   */
    cut_rec = "MCTrackInfo.GetQuality() > 0";

    cut_mc_prim = cut_mc && "MCTrackInfo.GetMCQuality() == -2";
    cut_mc_sec = cut_mc && "MCTrackInfo.GetMCQuality() == -1";
    cut_rec_prim = cut_rec && cut_mc_prim;
    cut_rec_sec = cut_rec && cut_mc_sec;

  TH1F* htfeffinpl = new TH1F("htfeffinpl", "Track finding efficiency vs pl", (maxPl - minPl)*Pres, minPl, maxPl);
  TH1F* hmctfeffinpl = new TH1F("hmctfeffinpl", "Track finding efficiency vs pl", (maxPl - minPl)*Pres, minPl, maxPl);
  htfeffinpl->Sumw2();
  hmctfeffinpl->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> htfeffinpl", cut_rec, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> hmctfeffinpl", cut_mc, "goff");
  htfeffinpl->Divide(hmctfeffinpl);

  TH1F *htfeffinplprim = new TH1F("htfeffinplprim", "Track finding efficiency vs pl prim", (maxPl - minPl)*Pres, minPl, maxPl);
  TH1F *hmctfeffinplprim = new TH1F("hmctfeffinplprim", "Track finding efficiency vs pl prim", (maxPl - minPl)*Pres, minPl, maxPl);
  htfeffinplprim->Sumw2();
  hmctfeffinplprim->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> htfeffinplprim", cut_rec_prim, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> hmctfeffinplprim", cut_mc_prim, "goff");
  htfeffinplprim->Divide(hmctfeffinplprim);

  TH1F *htfeffinplsec = new TH1F("htfeffinplsec", "Track finding efficiency vs pl sec", (maxPl - minPl)*Pres, minPl, maxPl);
  TH1F *hmctfeffinplsec = new TH1F("hmctfeffinplsec", "Track finding efficiency vs pl sec", (maxPl - minPl)*Pres, minPl, maxPl);
  htfeffinplsec->Sumw2();
  hmctfeffinplsec->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> htfeffinplsec", cut_rec_sec, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> hmctfeffinplsec", cut_mc_sec, "goff");
  htfeffinplsec->Divide(hmctfeffinplsec);

  /**
   * track finding efficiency vs. startZ
   */
    cut_rec = "MCTrackInfo.GetQuality() > 0";

    cut_mc_prim = cut_mc && "MCTrackInfo.GetMCQuality() == -2";
    cut_mc_sec = cut_mc && "MCTrackInfo.GetMCQuality() == -1";
    cut_rec_prim = cut_rec && cut_mc_prim;
    cut_rec_sec = cut_rec && cut_mc_sec;

    TH1F* hstartZRange = new TH1F("hstartZRange","Range of StartZ", 2000, -1000, 1000);
    simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].fStartZ >> hstartZRange", "", "goff");
    Float_t maxStartZ = hstartZRange->GetBinLowEdge(hstartZRange->FindLastBinAbove(0)) + hstartZRange->GetBinWidth(hstartZRange->FindLastBinAbove(0));
    Float_t minStartZ = hstartZRange->GetBinLowEdge(hstartZRange->FindFirstBinAbove(0)) + hstartZRange->GetBinWidth(hstartZRange->FindFirstBinAbove(0));
    Float_t startZRes = 2;

    std::cout << "MinStartZ = " << minStartZ << " : MaxStartZ " << maxStartZ << " : StartZRes = " << startZRes << std::endl;

  TH1F* htfeffinstartZ = new TH1F("htfeffinstartZ", "Track finding efficiency vs startZ", (maxStartZ - minStartZ)*startZRes, minStartZ, maxStartZ);
  TH1F* hmctfeffinstartZ = new TH1F("hmctfeffinstartZ", "Track finding efficiency vs startZ", (maxStartZ - minStartZ)*startZRes, minStartZ, maxStartZ);
  htfeffinstartZ->Sumw2();
  hmctfeffinstartZ->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].fStartZ >> htfeffinstartZ", cut_rec, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].fStartZ >> hmctfeffinstartZ", cut_mc, "goff");
  htfeffinstartZ->Divide(hmctfeffinstartZ);

  TH1F *htfeffinstartZprim = new TH1F("htfeffinstartZprim", "Track finding efficiency vs startZ prim", (maxStartZ - minStartZ)*startZRes, minStartZ, maxStartZ);
  TH1F *hmctfeffinstartZprim = new TH1F("hmctfeffinstartZprim", "Track finding efficiency vs startZ prim", (maxStartZ - minStartZ)*startZRes, minStartZ, maxStartZ);
  htfeffinstartZprim->Sumw2();
  hmctfeffinstartZprim->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].fStartZ >> htfeffinstartZprim", cut_rec_prim, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].fStartZ >> hmctfeffinstartZprim", cut_mc_prim, "goff");
  htfeffinstartZprim->Divide(hmctfeffinstartZprim);

  TH1F *htfeffinstartZsec = new TH1F("htfeffinstartZsec", "Track finding efficiency vs startZ sec", (maxStartZ - minStartZ)*startZRes, minStartZ, maxStartZ);
  TH1F *hmctfeffinstartZsec = new TH1F("hmctfeffinstartZsec", "Track finding efficiency vs startZ sec", (maxStartZ - minStartZ)*startZRes, minStartZ, maxStartZ);
  htfeffinstartZsec->Sumw2();
  hmctfeffinstartZsec->Sumw2();
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].fStartZ >> htfeffinstartZsec", cut_rec_sec, "goff");
  simtree->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].fStartZ >> hmctfeffinstartZsec", cut_mc_sec, "goff");
  htfeffinstartZsec->Divide(hmctfeffinstartZsec);

  /**
     Write to output
  **/

  TFile outfile(outputFile.Data(), "RECREATE");
  eff_mvdpix->Write();
  eff_mvdstr->Write();
  eff_stt->Write();
  eff_gem->Write();
  eff_fts->Write();
  eff_glo->Write();

  pur_mvdpix->Write();
  pur_mvdstr->Write();
  pur_stt->Write();
  pur_gem->Write();
  pur_fts->Write();
  pur_glo->Write();

  hnofrecotracks->Write();
  hnofMCtracks->Write();

  hdelta_p_first->Write();
  hdelta_pz_first->Write();
  hdelta_pt_first->Write();
  hdelta_theta_first->Write();
  hdelta_phi_first->Write();
  hdelta_x_first->Write();
  hdelta_y_first->Write();
  hdelta_z_first->Write();
  hdelta_r_first->Write();


  hdelta_p_last->Write();
  hdelta_pz_last->Write();
  hdelta_pt_last->Write();
  hdelta_theta_last->Write();
  hdelta_phi_last->Write();
  hdelta_x_last->Write();
  hdelta_y_last->Write();
  hdelta_z_last->Write();
  hdelta_r_last->Write();

  hdelta_charge->Write();

  heffintheta->Write();
  heffinphi->Write();
  heffinmom->Write();
  heffinpt->Write();
  heffinptprim->Write();
  heffinptsec->Write();
  heffinpl->Write();

  htfeffintheta->Write();
  htfeffinthetaprim->Write();
  htfeffinthetasec->Write();

  htfeffinphi->Write();
   htfeffinphiprim->Write();
   htfeffinphisec->Write();

  htfeffinpt->Write();
  htfeffinptprim->Write();
  htfeffinptsec->Write();

  htfeffinpl->Write();
  htfeffinplprim->Write();
  htfeffinplsec->Write();

  htfeffinstartZ->Write();
  htfeffinstartZprim->Write();
  htfeffinstartZsec->Write();

  hQualityPrim->Write();
  hQualitySec->Write();


  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  return 0;
}
