/**
   @class QA_histos.C
   @brief Quality Assurance histogram macro
   @author Lia Lavezzi <lia.lavezzi@to.infn.it>, University of Torino
   @author Stefano Spataro <stefano.spataro@to.infn.it>, University of Torino
   @version 1.0
   @date Jun 15, 2015
**/

void QA_histos() {

  TFile fileqa("recoqa_complete.root");
  TTree *cbmsim = (TTree*) fileqa.Get("cbmsim");
  cbmsim->AddFriend("cbmsim", "sim_complete.root");

  TCut cut = "";

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
  
  cbmsim->Draw("RecoTrackInfo.GetMvdPixelEfficiency() >> eff_mvdpix", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetMvdStripEfficiency() >> eff_mvdstr", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetSttEfficiency() >> eff_stt", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetGemEfficiency() >> eff_gem", cut, "goff");

  TH1F *eff_glo = new TH1F("eff_glo", "global efficiency", 100, effmin, effmax);
  cbmsim->Draw("RecoTrackInfo.GetEfficiency() >> eff_glo", cut, "goff");

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
  
  cbmsim->Draw("RecoTrackInfo.GetMvdPixelPurity() >> pur_mvdpix", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetMvdStripPurity() >> pur_mvdstr", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetSttPurity() >> pur_stt", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetGemPurity() >> pur_gem", cut, "goff");

 
  TH1F *pur_glo = new TH1F("pur_glo", "global purity", 100, purmin, purmax);
  cbmsim->Draw("RecoTrackInfo.GetPurity() >> pur_glo", cut, "goff");

  /**
     2 histos: 
     Number of reco tracks correlated to MC track, for primaries (>1 -> clones) 
     Number of MC tracks correlated to reco tracks for primaries (>1 -> broken tracks
  **/
  TH1F *hnofrecotracks = new TH1F("hnofrecotracks", "# of reco tracks associated to the same MC track", 20, 0, 20);
  cbmsim->Draw("MCTrackInfo.GetNofRecoTracks() >> hnofrecotracks", cut, "goff");

  TH1F *hnofMCtracks = new TH1F("hnofMCtracks", "# of MC tracks associated to the same reco track", 20, 0, 20);
  cbmsim->Draw("RecoTrackInfo.GetNofMCTracks() >> hnofMCtracks", cut, "goff");

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

  cbmsim->Draw("RecoTrackInfo.GetMomentumFirst().Mag() - RecoTrackInfo.GetMCTrackInfo().GetMomentumFirst().Mag() >> hdelta_p_first", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetMomentumFirst().Perp() - RecoTrackInfo.GetMCTrackInfo().GetMomentumFirst().Perp() >> hdelta_pt_first", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetMomentumFirst().Z() - RecoTrackInfo.GetMCTrackInfo().GetMomentumFirst().Z() >> hdelta_pz_first", cut, "goff");
  
  double thetamin = -5;
  double thetamax = 5;
  int ntheta = (thetamax - thetamin)*10;
  TH1F *hdelta_theta_first = new TH1F("hdelta_theta_first", "#Delta #theta @ first hit", ntheta, thetamin, thetamax);
  cbmsim->Draw("(RecoTrackInfo.GetMomentumFirst().Theta() - RecoTrackInfo.GetMCTrackInfo().GetMomentumFirst().Theta()) * TMath::RadToDeg() >> hdelta_theta_first", cut, "goff");
  
  double phimin = -5;
  double phimax = 5;
  int nphi = (phimax - phimin)*10;
  TH1F *hdelta_phi_first = new TH1F("hdelta_phi_first", "#Delta #phi @ first hit", nphi, phimin, phimax);
  cbmsim->Draw("(RecoTrackInfo.GetMomentumFirst().Phi() - RecoTrackInfo.GetMCTrackInfo().GetMomentumFirst().Phi()) * TMath::RadToDeg() >> hdelta_phi_first", cut, "goff");


  double xmin = -0.25;
  double xmax = 0.25;
  TH1F *hdelta_x_first = new TH1F("hdelta_x_first", "#Delta x @ first hit", 100, xmin, xmax);
  TH1F *hdelta_y_first = new TH1F("hdelta_y_first", "#Delta y @ first hit", 100, xmin, xmax);
  cbmsim->Draw("RecoTrackInfo.GetPositionFirst().X() - RecoTrackInfo.GetMCTrackInfo().GetPositionFirst().X() >> hdelta_x_first", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetPositionFirst().Y() - RecoTrackInfo.GetMCTrackInfo().GetPositionFirst().Y() >> hdelta_y_first", cut, "goff");

  double zmin = -0.25;
  double zmax = 0.25;
  TH1F *hdelta_z_first = new TH1F("hdelta_z_first", "#Delta z @ first hit", 100, zmin, zmax);
  cbmsim->Draw("RecoTrackInfo.GetPositionFirst().Z() - RecoTrackInfo.GetMCTrackInfo().GetPositionFirst().Z() >> hdelta_z_first", cut, "goff");

  double rmax = xmax;
  TH1F *hdelta_r_first = new TH1F("hdelta_r_first", "#Delta r @ first hit", 100, -rmax, rmax);
  cbmsim->Draw("RecoTrackInfo.GetPositionFirst().Perp() - RecoTrackInfo.GetMCTrackInfo().GetPositionFirst().Perp() >> hdelta_r_first", cut, "goff");
  
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

  cbmsim->Draw("RecoTrackInfo.GetMomentumLast().Mag() - RecoTrackInfo.GetMCTrackInfo().GetMomentumLast().Mag() >> hdelta_p_last", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetMomentumLast().Perp() - RecoTrackInfo.GetMCTrackInfo().GetMomentumLast().Perp() >> hdelta_pt_last", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetMomentumLast().Z() - RecoTrackInfo.GetMCTrackInfo().GetMomentumLast().Z() >> hdelta_pz_last", cut, "goff");
  
  TH1F *hdelta_theta_last = new TH1F("hdelta_theta_last", "#Delta #theta @ last hit", ntheta, thetamin, thetamax);
  cbmsim->Draw("(RecoTrackInfo.GetMomentumLast().Theta() - RecoTrackInfo.GetMCTrackInfo().GetMomentumLast().Theta()) * TMath::RadToDeg() >> hdelta_theta_last", cut, "goff");
  
  TH1F *hdelta_phi_last = new TH1F("hdelta_phi_last", "#Delta #phi @ last hit", nphi, phimin, phimax);
  cbmsim->Draw("(RecoTrackInfo.GetMomentumLast().Phi() - RecoTrackInfo.GetMCTrackInfo().GetMomentumLast().Phi()) * TMath::RadToDeg() >> hdelta_phi_last", cut, "goff");

  TH1F *hdelta_x_last = new TH1F("hdelta_x_last", "#Delta x @ last hit", 100, xmin, xmax);
  TH1F *hdelta_y_last = new TH1F("hdelta_y_last", "#Delta y @ last hit", 100, xmin, xmax);
  cbmsim->Draw("RecoTrackInfo.GetPositionLast().X() - RecoTrackInfo.GetMCTrackInfo().GetPositionLast().X() >> hdelta_x_last", cut, "goff");
  cbmsim->Draw("RecoTrackInfo.GetPositionLast().Y() - RecoTrackInfo.GetMCTrackInfo().GetPositionLast().Y() >> hdelta_y_last", cut, "goff");

  TH1F *hdelta_z_last = new TH1F("hdelta_z_last", "#Delta z @ last hit", 100, zmin, zmax);
  cbmsim->Draw("RecoTrackInfo.GetPositionLast().Z() - RecoTrackInfo.GetMCTrackInfo().GetPositionLast().Z() >> hdelta_z_last", cut, "goff");

  TH1F *hdelta_r_last = new TH1F("hdelta_r_last", "#Delta r @ last hit", 100, -rmax, rmax);
  cbmsim->Draw("RecoTrackInfo.GetPositionLast().Perp() - RecoTrackInfo.GetMCTrackInfo().GetPositionLast().Perp() >> hdelta_r_last", cut, "goff");
  
  /**
     delta_charge
  **/
  double chargemin = -3;
  double chargemax = 3;
  int ncharge = chargemax - chargemin;
  TH1F *hdelta_charge = new TH1F("hdelta_charge", "#Delta #charge", ncharge, chargemin, chargemax);
  cbmsim->Draw("RecoTrackInfo.GetCharge() - RecoTrackInfo.GetMCTrackInfo().GetCharge() >> hdelta_charge", cut, "goff");

  /** 
      efficiency vs theta
  **/
  TH1F *hthetagen = new TH1F("hthetagen", "mc theta dist", 180, 0, 180);
  TH1F *heffintheta = new TH1F("heffintheta", "efficiency vs #theta", 180, 0, 180);

  TCut cut_mc = "";
  TCut cut_rec = cut_mc && "MCTrackInfo.GetRecoTrackID() != -1";
  cut_rec = cut_rec && "RecoTrackInfo[MCTrackInfo.GetRecoTrackID()].GetEfficiency() > 0.8";
  cut_rec = cut_rec && "RecoTrackInfo[MCTrackInfo.GetRecoTrackID()].IsClone() == 0";

  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> hthetagen", cut_mc, "goff");
  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Theta() * TMath::RadToDeg() >> heffintheta", cut_rec, "goff");
  
  hthetagen->Sumw2();
  heffintheta->Sumw2();
  heffintheta->Divide(hthetagen);


  /** 
      efficiency vs phi
  **/
  TH1F *hphigen = new TH1F("hphigen", "mc phi dist", 180, -180, 180);
  TH1F *heffinphi = new TH1F("heffinphi", "efficiency vs #phi", 180, -180, 180);
  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> hphigen", cut_mc, "goff");
  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Phi() * TMath::RadToDeg() >> heffinphi", cut_rec, "goff");
  
  hphigen->Sumw2();
  heffinphi->Sumw2();
  heffinphi->Divide(hphigen);

  /** 
      efficiency vs mom
  **/
  TH1F *hmomgen = new TH1F("hmomgen", "mc mom dist", 100, 0, 3);
  TH1F *heffinmom = new TH1F("heffinmom", "efficiency vs mom", 100, 0, 3);
  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Mag() >> hmomgen", cut_mc, "goff");
  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Mag() >> heffinmom", cut_rec, "goff");
  
  hmomgen->Sumw2();
  heffinmom->Sumw2();
  heffinmom->Divide(hmomgen);

  /** 
      efficiency vs pt
  **/
  TH1F *hptgen = new TH1F("hptgen", "mc pt dist", 100, 0, 3);
  TH1F *heffinpt = new TH1F("heffinpt", "efficiency vs pt", 100, 0, 3);
  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> hptgen", cut_mc, "goff");
  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Perp() >> heffinpt", cut_rec, "goff");
  
  hptgen->Sumw2();
  heffinpt->Sumw2();
  heffinpt->Divide(hptgen);

  /** 
      efficiency vs pl
  **/
  TH1F *hplgen = new TH1F("hplgen", "mc pl dist", 100, 0, 3);
  TH1F *heffinpl = new TH1F("heffinpl", "efficiency vs pl", 100, 0, 3);
  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> hplgen", cut_mc, "goff");
  cbmsim->Draw("MCTrack[MCTrackInfo.GetMCTrackID()].GetMomentum().Z() >> heffinpl", cut_rec, "goff");
  
  hplgen->Sumw2();
  heffinpl->Sumw2();
  heffinpl->Divide(hplgen);



  /**
     Write to output
  **/

  TFile outfile("QA_histograms.root", "RECREATE");
  eff_mvdpix->Write();
  eff_mvdstr->Write();
  eff_stt->Write();
  eff_gem->Write();
  eff_glo->Write();

  pur_mvdpix->Write();
  pur_mvdstr->Write();
  pur_stt->Write();
  pur_gem->Write();
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
  heffinpl->Write();

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}
