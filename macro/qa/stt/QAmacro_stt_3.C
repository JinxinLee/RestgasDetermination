// TEST 3: check digi and reco output
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  Bool_t fTest = kTRUE;
  Int_t kindOftest[17];
  for(int t = 0; t < 17; t++) kindOftest[t] = 0;
  
  // MCpoints
  TFile filerun("testrun.root");
  TTree *treepnt = (TTree*) filerun.Get("cbmsim");
  TClonesArray *pnt = new TClonesArray("PndSttPoint");
  treepnt->SetBranchAddress("STTPoint",&pnt);

  // MCTracks 
  TClonesArray *mctrackarray = new TClonesArray("PndMCTrack");
  treepnt->SetBranchAddress("MCTrack",&mctrackarray);
  
  // Hits
  TFile filereco("testcomplete.root");
  TTree *treereco = (TTree*) filereco.Get("cbmsim");
  TClonesArray *digiarray = new TClonesArray("PndSttHit");
  treereco->SetBranchAddress("STTHit",&digiarray);

  // HelixHits
  TClonesArray *hharray = new TClonesArray("PndSttHelixHit");
  treereco->SetBranchAddress("SttHelixHit",&hharray);
  
  // Helix Tracks
  TClonesArray *trackarray = new TClonesArray("PndSttTrack");
  treereco->SetBranchAddress("STTTrack",&trackarray);
  
  // Track Match
  TClonesArray *trackmatcharray = new TClonesArray("PndSttTrackMatch");
  treereco->SetBranchAddress("STTTrackMatch",&trackmatcharray);

  // Kalman Track 
  TClonesArray *kalmanarray = new TClonesArray("GFTrack");
  treereco->SetBranchAddress("Track",&kalmanarray);
  
  // histograms
  TH1F *hisochrone = new TH1F("hisochrone","Simulated drift radius", 100, 0., 0.6);
  TH1F *hptot_mum = new TH1F("hptot_mum","Total momentum for #mu^{-}", 100, 0.5, 1.5);
  TH1F *hptot_mup = new TH1F("hptot_mup","Total momentum for #mu^{+}", 100, 0.5, 1.5);
  TH1F *hptot_mum_kal = new TH1F("hptot_mum_kal","Kalman total momentum for #mu^{-}", 100, 0.5, 1.5);
  TH1F *hptot_mup_kal = new TH1F("hptot_mup_kal","Kalman total momentum for #mu^{+}", 100, 0.5, 1.5);

  for (Int_t evt = 0; evt < treereco->GetEntriesFast(); evt++) {
    
    treepnt->GetEntry(evt);
    treereco->GetEntry(evt);

    // =================== DIGI ====================
    for(Int_t i = 0; i < digiarray->GetEntriesFast(); i++) {

      PndSttHit *hit = (PndSttHit*) digiarray->At(i);
      if(!hit) continue;
      hisochrone->Fill(hit->GetIsochrone());
      // if the reconstructed radius is > 0.5 (0.506 because of binning) -> kFALSE
      if(hisochrone->GetBinCenter(hisochrone->FindLastBinAbove(0)) > 0.506) { fTest = kFALSE; kindOftest[1] = 1; }
    }


    // =================== RECO ====================
    // tracks loop
    for (Int_t k = 0; k < trackarray->GetEntriesFast(); k++) {
    
      PndSttTrack *stttrack = (PndSttTrack*) trackarray->At(k);
      if(!stttrack) continue;
      PndSttTrackMatch *sttmatch = (PndSttTrackMatch*) trackmatcharray->At(k);
      if(!sttmatch) continue; 

      Double_t d0   = stttrack->GetDist();
      Double_t phi0 = stttrack->GetPhi();
      Double_t R    = stttrack->GetRad();
      Double_t z0   = stttrack->GetZ();
      Double_t tanl = stttrack->GetTanL();
      Double_t h    = -(Int_t) stttrack->GetCharge();
      Double_t ptran = 0.003 * 2 * R;
      Double_t plong = ptran * tanl;
      Double_t ptot = sqrt(plong*plong + ptran*ptran);

      // mu - or mu + ?
      Int_t MCTrackID = sttmatch->GetMCTrackID();
      if(MCTrackID == -1) continue;
      PndMCTrack *mctrack = (PndMCTrack*) mctrackarray->At(MCTrackID);
      if(!mctrack) continue;

      if(mctrack->GetPdgCode() == 13)       hptot_mum->Fill(ptot);
      else if(mctrack->GetPdgCode() == -13) hptot_mup->Fill(ptot);
      //    else nomutracks++; 
 
    }

    // =================== KALMAN ====================
    // tracks loop
    for (Int_t j = 0; j < kalmanarray->GetEntriesFast(); j++) {
      
      GFTrack *kaltrack = (GFTrack*) kalmanarray->At(j);
      if(!kaltrack) continue;
	
      ptot = kaltrack->getMom().Mag();
	
      // mu - or mu + ?
      MCTrackID = ((GFTrackCand*) kaltrack->getCand())->getMcTrackId();
      if(MCTrackID == -1) continue;
	
      PndMCTrack *mctrack = (PndMCTrack*) mctrackarray->At(MCTrackID);
      if(!mctrack) continue;
	
      if(mctrack->GetPdgCode() == 13)       hptot_mum_kal->Fill(ptot);
      else if(mctrack->GetPdgCode() == -13) hptot_mup_kal->Fill(ptot);
      //    else nomutracks++; 
     
    }
  }

  // negative muon ...............................................
  // HELIX
  TF1 *fgaus = new TF1("fgaus", "gaus",0.9, 1.1);
  hptot_mum.Fit("fgaus","R0QN");
  Double_t gauspar[3];
  fgaus->GetParameters(gauspar);
  Double_t mean  = gauspar[1];
  Double_t sigma = gauspar[2];

  // mean in [1 GeV/c - 3 sigma, 1 GeV/c + 3 sigma]
  if(mean < (mean - 3 * sigma) || mean > (mean + 3 * sigma)) { fTest = kFALSE; kindOftest[1] = 1; }

  // resolution sigma/p in [3.%  - 0.5%, 3.% + 0.5%]
  if((sigma/mean) < (0.03 - 0.005) || (sigma/mean) > (0.03 + 0.005)) { fTest = kFALSE; kindOftest[2] = 1; }

  // efficiency integral in 0.5, 1.5 / generated tracks in [83% - 5%, 83% + 5%] 
  if((hptot_mum->Integral() / 300.) < (0.83 - 0.5) || (hptot_mum->Integral() / 300.) > (0.83 + 0.5)) { fTest = kFALSE; kindOftest[3] = 1; }

  // efficiency integral under the peak/generated tracks in [78% - 5%, 78% + 5%]
  hptot_mum->GetXaxis()->SetRangeUser(0.9, 1.1);
  if((hptot_mum->Integral() / 300.) < (0.78 - 0.5) || (hptot_mum->Integral() / 300.) > (0.78 + 0.5)) { fTest = kFALSE; kindOftest[4] = 1;}


  // KALMAN
  hptot_mum_kal.Fit("fgaus","R0QN");
  fgaus->GetParameters(gauspar);
  mean  = gauspar[1];
  sigma = gauspar[2];
      
  // mean in [1 GeV/c - 3 sigma, 1 GeV/c + 3 sigma]
  if(mean < (mean - 3 * sigma) || mean > (mean + 3 * sigma)) { fTest = kFALSE; kindOftest[5] = 1; }
      
  // resolution sigma/p in [2.3%  - 0.5%, 2.3% + 0.5%]
  if((sigma/mean) < (0.023 - 0.005) || (sigma/mean) > (0.023 + 0.005)) { fTest = kFALSE; kindOftest[6] = 1; }
      
  // efficiency integral in 0.5, 1.5 / generated tracks in [83% - 5%, 83% + 5%] 
  if((hptot_mum_kal->Integral() / 300.) < (0.83 - 0.5) || (hptot_mum_kal->Integral() / 300.) > (0.83 + 0.5)) { fTest = kFALSE; kindOftest[7] = 1; }
      
  // efficiency integral under the peak/generated tracks in [81% - 5%, 81% + 5%]
  hptot_mum_kal->GetXaxis()->SetRangeUser(0.9, 1.1);
  if((hptot_mum_kal->Integral() / 300.) < (0.81 - 0.5) || (hptot_mum_kal->Integral() / 300.) > (0.81 + 0.5)) { fTest = kFALSE; kindOftest[8] = 1; }
      
    
  // positive muon ...............................................
  // HELIX
  hptot_mup.Fit("fgaus","R0QN");
  fgaus->GetParameters(gauspar);
  mean  = gauspar[1];
  sigma = gauspar[2];
  
  // mean in [1 GeV/c - 3 sigma, 1 GeV/c + 3 sigma]
  if(mean < (mean - 3 * sigma) || mean > (mean + 3 * sigma)) { fTest = kFALSE; kindOftest[9] = 1; }

  // resolution sigma/p in [2.8%  - 0.5%, 2.8% + 0.5%]
  if((sigma/mean) < (0.028 - 0.005) || (sigma/mean) > (0.028 + 0.005)) { fTest = kFALSE; kindOftest[10] = 1; }

  // efficiency integral in 0.5, 1.5 / generated tracks in [83% - 5%, 83% + 5%] 
  if((hptot_mup->Integral() / 300.) < (0.83 - 0.5) || (hptot_mup->Integral() / 300.) > (0.83 + 0.5)) { fTest = kFALSE; kindOftest[11] = 1; }

  // efficiency integral under the peak/generated tracks in [78% - 5%, 78% + 5%]
  hptot_mup->GetXaxis()->SetRangeUser(0.9, 1.1);
  if((hptot_mup->Integral() / 300.) < (0.78 - 0.5) || (hptot_mup->Integral() / 300.) > (0.78 + 0.5)) { fTest = kFALSE; kindOftest[12] = 1; }


  // KALMAN
  hptot_mup_kal.Fit("fgaus","R0QN");
  fgaus->GetParameters(gauspar);
  mean  = gauspar[1];
  sigma = gauspar[2];
  
  // mean in [1 GeV/c - 3 sigma, 1 GeV/c + 3 sigma]
  if(mean < (mean - 3 * sigma) || mean > (mean + 3 * sigma)) { fTest = kFALSE; kindOftest[13] = 1; }

  // resolution sigma/p in [2.3%  - 0.5%, 2.3% + 0.5%]
  if((sigma/mean) < (0.023 - 0.005) || (sigma/mean) > (0.023 + 0.005)) { fTest = kFALSE; kindOftest[14] = 1; }

  // efficiency integral in 0.5, 1.5 / generated tracks in [85% - 5%, 85% + 5%] 
  if((hptot_mup_kal->Integral() / 300.) < (0.85 - 0.5) || (hptot_mup_kal->Integral() / 300.) > (0.85 + 0.5)) { fTest = kFALSE; kindOftest[15] = 1; }

  // efficiency integral under the peak/generated tracks in [80% - 5%, 80% + 5%]
  hptot_mup_kal->GetXaxis()->SetRangeUser(0.9, 1.1);
  if((hptot_mup_kal->Integral() / 300.) < (0.80 - 0.5) || (hptot_mup_kal->Integral() / 300.) > (0.80 + 0.5)) { fTest = kFALSE; kindOftest[16] = 1; }
   


 
  if (fTest == kTRUE){
    cout << " Test Passed" << endl;
    cout << " All Ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         

    for(int t = 0; t < 17; t++) {

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
	cout << "negative muon ptot efficiency in [0.5, 1.5] wrong" << endl;
	break;
	  
      case 4:
	cout << "negative muon ptot efficiency in [0.9, 1.1] wrong" << endl;
	break;
	  
      case 5:
	cout << "negative muon kalman ptot out of 3 sigma" << endl;
	break;
	  
      case 6:
	cout << "negative muon kalman ptot resolution wrong" << endl;
	break;
	  
      case 7:
	cout << "negative muon kalman ptot efficiency in [0.5, 1.5] wrong" << endl;
	break;
	  
      case 8:
	cout << "negative muon kalman ptot efficiency in [0.9, 1.1] wrong" << endl;
	break;
	  
      case 9:
	cout << "positive muon ptot out of 3 sigma" << endl;
	break;
	  
      case 10:
	cout << "positive muon ptot resolution wrong" << endl;
	break;
	  
      case 11:
	cout << "positive muon ptot efficiency in [0.5, 1.5] wrong" << endl;
	break;
	  
      case 12:
	cout << "positive muon ptot efficiency in [0.9, 1.1] wrong" << endl;
	break;
	  
      case 13:
	cout << "positive muon kalman ptot out of 3 sigma" << endl;
	break;
	  
      case 14:
	cout << "positive muon kalman ptot resolution wrong" << endl;
	break;
	  
      case 15:
	cout << "positive muon kalman ptot efficiency in [0.5, 1.5] wrong" << endl;
	break;
	  
      case 16:
	cout << "positive muon kalman ptot efficiency in [0.9, 1.1] wrong" << endl;
	break;
	  
      }
    }
  }
  
  exit(0); 
}  
  
