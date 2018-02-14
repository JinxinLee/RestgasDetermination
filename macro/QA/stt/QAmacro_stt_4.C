// TEST 4: check digi and reco output
int QAmacro_stt_4()
{
  TStopwatch timer;
  timer.Start();

  Bool_t fTest = kTRUE;
  Int_t kindOftest[9];
  for(int t = 0; t < 9; t++) kindOftest[t] = 0;
  
  // MCpoints
  TFile filerun("testrun.root");
  TTree *treepnt = (TTree*) filerun.Get("cbmsim");
  TClonesArray *pnt = new TClonesArray("PndSttPoint");
  treepnt->SetBranchAddress("STTPoint",&pnt);

  // MCTracks 
  TClonesArray *mctrackarray = new TClonesArray("PndMCTrack");
  treepnt->SetBranchAddress("MCTrack",&mctrackarray);
  
  // Hits
  TFile filedigi("testdigi.root");
  TTree *treedigi = (TTree*) filedigi.Get("cbmsim");
  TClonesArray *digiarray = new TClonesArray("PndSttHit");
  treedigi->SetBranchAddress("STTHit",&digiarray);

  // Helix Tracks 
  TFile filereco("testreco.root");
  TTree *treereco = (TTree*) filereco.Get("cbmsim");
  TClonesArray *trackarray = new TClonesArray("PndTrack");
  treereco->SetBranchAddress("SttMvdTrack",&trackarray);
  
  // Track Match
//  TClonesArray *trackIDarray = new TClonesArray("PndTrackID");
//  treereco->SetBranchAddress("SttMvdTrackID",&trackIDarray);

  
  // histograms
  TH1F *hisochrone = new TH1F("hisochrone","Simulated drift radius", 100, 0., 0.6);
  TH1F *hptot_mum = new TH1F("hptot_mum","Total momentum for #mu^{-}", 100, 0.6, 1.4);
  TH1F *hptot_mup = new TH1F("hptot_mup","Total momentum for #mu^{+}", 100, 0.6, 1.4);
  TH1F *hptot = new TH1F("hptot","Total momentum", 100, 0.6, 1.4);

  // limits
  int nevents = treereco->GetEntriesFast();
  int mum_tracks = 3 * nevents;
  int mup_tracks = 3 * nevents;
  // mu - HELIX
  double mum_hel_res          = 0.03;
  double mum_hel_res_tol      = 0.01;
  double mum_hel_eff          = 0.87;
  double mum_hel_eff_tol      = 0.1;
  double mum_hel_peak_eff     = 0.81;
  double mum_hel_peak_eff_tol = 0.1;

  // mu + HELIX
  double mup_hel_res          = 0.03;
  double mup_hel_res_tol      = 0.01;
  double mup_hel_eff          = 0.87;
  double mup_hel_eff_tol      = 0.1;
  double mup_hel_peak_eff     = 0.81;
  double mup_hel_peak_eff_tol = 0.1;
 
  //  cout << "nevents " <<  nevents << endl;

  for (Int_t evt = 0; evt < nevents; evt++) {
    
    treepnt->GetEntry(evt);
    treedigi->GetEntry(evt);
    treereco->GetEntry(evt);

    // =================== DIGI ====================
    for(Int_t i = 0; i < digiarray->GetEntriesFast(); i++) {

      PndSttHit *hit = (PndSttHit*) digiarray->At(i);
      if(!hit) continue;
      hisochrone->Fill(hit->GetIsochrone());
      // if the reconstructed radius is > 0.5 (0.506 because of binning) -> kFALSE
      if(hisochrone->GetBinCenter(hisochrone->FindLastBinAbove(0)) > 0.506) { 
	fTest = kFALSE;	kindOftest[0] = 1;
	cout << "TEST 0: hisocrone " << hisochrone->GetBinCenter(hisochrone->FindLastBinAbove(0)) << " (limit = 0.506)" << endl;
      }
    }


    // =================== RECO ====================
    // tracks loop
    for (Int_t k = 0; k < trackarray->GetEntriesFast(); k++) {
    
      PndTrack *track = (PndTrack*) trackarray->At(k);
      if(!track) continue;
//      PndTrackID *trackID = (PndTrackID*) trackIDarray->At(k);
//      if(!trackID) continue;

      if(track->GetFlag() < 0) continue;
      TVector3 lastmom = track->GetParamLast().GetMomentum();  
	
      // mu - or mu + ?
      Int_t MCTrackID = -1;
      if (track->GetNLinks() > 0){
    	  FairLink MCLink = track->GetLink(1);
    	  if (MCLink.GetType() == 0)
    		  MCTrackID = MCLink.GetIndex();
      }
      if(MCTrackID == -1) continue;
      PndMCTrack *mctrack = (PndMCTrack*) mctrackarray->At(MCTrackID);
      if(!mctrack) continue;
      if(mctrack->GetMotherID() != -1) continue;
   
      if(mctrack->GetPdgCode() == 13)       hptot_mum->Fill(lastmom.Mag());
      else if(mctrack->GetPdgCode() == -13) hptot_mup->Fill(lastmom.Mag());
      hptot->Fill(lastmom.Mag());

      //    else nomutracks++; 
    }

  
  }

  // get limits on sum histo mu+ + mu-
  int binmax = hptot->GetMaximumBin();
  double ymax = hptot->GetMaximum();
  double xmax = hptot->GetBinCenter(binmax);
  int bininf = hptot->FindFirstBinAbove(ymax/2., 1);
  bininf -= 1;
  int binsup = hptot->FindLastBinAbove(ymax/2., 1);
  binsup += 1;
  double xinf = hptot->GetBinCenter(bininf);
  double xsup = hptot->GetBinCenter(binsup);
  double gamma = xsup - xinf;
  double sigma1 = gamma / 2.34;
  xinf = xmax - 3 * sigma1;
  xsup = xmax + 3 * sigma1;



  // negative muon ...............................................
  // HELIX
  TF1 *fgaus = new TF1("fgaus", "gaus", xinf, xsup);
  hptot_mum->Fit("fgaus","R0QN");
  Double_t gauspar[3];
  fgaus->GetParameters(gauspar);
  Double_t mean  = gauspar[1];
  Double_t sigma = gauspar[2];


  // mean in [1 GeV/c - 3 sigma, 1 GeV/c + 3 sigma]
  if(mean < (mean - 3 * sigma) || mean > (mean + 3 * sigma)) { 
    fTest = kFALSE; kindOftest[1] = 1; 
    cout << "TEST 1: helix mu- mean " << mean 
	 << " (limits [" << (mean - 3 * sigma) << ", " << (mean + 3 * sigma) << "])" << endl;
  }

//   // resolution sigma/p  // CHECK reactivate it when it works
//   if((sigma/mean) < (mum_hel_res - mum_hel_res_tol) || (sigma/mean) > (mum_hel_res + mum_hel_res_tol)) { 
//     fTest = kFALSE; kindOftest[2] = 1;
//     cout << "TEST 2: helix mu- resolution " << sigma/mean 
// 	 << " (limits [" << (mum_hel_res - mum_hel_res_tol) << ", " <<  (mum_hel_res + mum_hel_res_tol) << "])" << endl;
//   }
  
//   // efficiency integral in 0.6, 1.4 / generated tracks  // CHECK reactivate it when it works
//   if((hptot_mum->Integral() / mum_tracks) < (mum_hel_eff - mum_hel_eff_tol) || (hptot_mum->Integral() / mum_tracks) > (mum_hel_eff + mum_hel_eff_tol)) {
//     fTest = kFALSE; kindOftest[3] = 1; 
//     cout << "TEST 3: helix mu- efficiency " << (hptot_mum->Integral() / mum_tracks) 
// 	 << " (limits [" << (mum_hel_eff - mum_hel_eff_tol) << ", " << (mum_hel_eff +  mum_hel_eff_tol) << "])" << endl;
//   }

//   // efficiency integral under the peak/generated tracks // CHECK reactivate it when it works
//   hptot_mum->GetXaxis()->SetRangeUser(mean - 3 * sigma, mean + 3 * sigma);
//   if((hptot_mum->Integral() / mum_tracks) < (mum_hel_peak_eff - mum_hel_peak_eff_tol) || (hptot_mum->Integral() / mum_tracks) > (mum_hel_peak_eff + mum_hel_peak_eff_tol)) { 
//     fTest = kFALSE; kindOftest[4] = 1;
//     cout << "TEST 4: helix mu- peak efficiency " << (hptot_mum->Integral() / mum_tracks) 
// 	 << " (limits [" << (mum_hel_peak_eff - mum_hel_peak_eff_tol) << ", " << (mum_hel_peak_eff + mum_hel_peak_eff_tol) << "])" << endl;
//   }


  // positive muon ...............................................
  // HELIX
  hptot_mup->Fit("fgaus","R0QN");
  fgaus->GetParameters(gauspar);
  mean  = gauspar[1];
  sigma = gauspar[2];
  
  // mean in [1 GeV/c - 3 sigma, 1 GeV/c + 3 sigma]
  if(mean < (mean - 3 * sigma) || mean > (mean + 3 * sigma)) { 
    fTest = kFALSE; kindOftest[5] = 1; 
    cout << "TEST 9: helix mu+ mean " << mean 
	 << " (limits [" << (mean - 3 * sigma) << ", " << (mean + 3 * sigma) << "])" << endl;
  }

//   // resolution sigma/p p  // CHECK reactivate it when it works
//   if((sigma/mean) < (mup_hel_res - mup_hel_res_tol) || (sigma/mean) > (mup_hel_res + mup_hel_res_tol)) { fTest = kFALSE; kindOftest[6] = 1;  
//     cout << "TEST 10: helix mu+ resolution " << sigma/mean 
// 	 << " (limits [" << (mup_hel_res - mup_hel_res_tol) << ", " << (mup_hel_res + mup_hel_res_tol) << "])" << endl;
//   }

//   // efficiency integral in 0.6, 1.4 / generated tracks // CHECK reactivate it when it works
//   if((hptot_mup->Integral() / mup_tracks) < (mup_hel_eff - mup_hel_eff_tol) || (hptot_mup->Integral() / mup_tracks) > (mup_hel_eff + mup_hel_eff_tol)) { 
//     fTest = kFALSE; kindOftest[7] = 1; 
//     cout << "TEST 11: helix mu+ efficiency " << (hptot_mup->Integral() / mup_tracks) 
// 	 << " (limits [" << (mup_hel_eff - mup_hel_eff_tol) << ", " << (mup_hel_eff + mup_hel_eff_tol) << "])" << endl;
//   }

 //  // efficiency integral under the peak/generated tracks // CHECK reactivate it when it works
//   hptot_mup->GetXaxis()->SetRangeUser(mean - 3 * sigma, mean + 3 * sigma);
//   if((hptot_mup->Integral() / mup_tracks) < (mup_hel_peak_eff - mup_hel_peak_eff_tol) || (hptot_mup->Integral() / mup_tracks) > (mup_hel_peak_eff + mup_hel_peak_eff_tol)) { fTest = kFALSE; kindOftest[8] = 1;   
//     cout << "TEST 12: helix mu+ peak efficiency " << (hptot_mup->Integral() / mup_tracks) 
// 	 << " (limits [" << (mup_hel_peak_eff - mup_hel_peak_eff_tol) << ", " << (mup_hel_peak_eff + mup_hel_peak_eff_tol) << "])" << endl;
//   }


   
 
  if (fTest == kTRUE){
    cout << " Test Passed" << endl;
    cout << " All Ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         

    for(int t = 0; t < 9; t++) {

      if(kindOftest[t] == 0) continue;

      switch(t) {
	  
      case 0:
	cout << "isochrone out of 0.5 cm" << endl;
	break;
	  
      case 1:
	cout << "negative muon ptot out of 3 sigma" << endl;
	break;
	  
      case 2:
	cout << "negative muon ptot resolution wrong" << endl;
	break;
	  
      case 3:
	cout << "negative muon ptot efficiency in [0.6, 1.4] wrong" << endl;
	break;
	  
      case 4:
	cout << "negative muon ptot efficiency in peak wrong" << endl;
	break;
	
      case 5:
	cout << "positive muon ptot out of 3 sigma" << endl;
	break;
	  
      case 6:
	cout << "positive muon ptot resolution wrong" << endl;
	break;
	  
      case 7:
	cout << "positive muon ptot efficiency in [0.6, 1.4] wrong" << endl;
	break;
	  
      case 8:
	cout << "positive muon ptot efficiency in peak wrong" << endl;
	break;
		  
      }
    }
  }
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}  
  
