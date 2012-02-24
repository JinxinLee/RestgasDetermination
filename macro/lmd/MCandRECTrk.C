void MCandRECTrk(const int nEvents=2, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=3, double dv=0.5, bool no4d=false)
{
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdTrk");
  // gROOT->LoadMacro("line3Dfit.C");
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // ---- Input files --------------------------------------------------------
  TString simMC=storePath+"/Lumi_MC_";
  simMC += startEvent;
  simMC += ".root";
  TChain tMC("cbmsim");
  tMC.Add(simMC);
  
  TString geaneFile = storePath+"/Lumi_Geane_";
  geaneFile += startEvent;
  geaneFile += ".root";
  TChain tgeane("cbmsim");
  tgeane.Add(geaneFile);
  
  // ---------------------------------------------------------------------------------
  
  // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/Lumi_out_MC_and_REC_trks";
  out += startEvent;
  out += ".root";
  TFile *f = new TFile(out,"RECREATE");
  // ---------------------------------------------------------------------------------
  
  //--- MC info -----------------------------------------------------------------
  TClonesArray* true_tracks=new TClonesArray("PndMCTrack");
  tMC.SetBranchAddress("MCTrack",&true_tracks);  //True Track to compare
  
  TClonesArray* true_points=new TClonesArray("PndSdsMCPoint");
  tMC.SetBranchAddress("LMDPoint",&true_points);  //True Points to compare
  //----------------------------------------------------------------------------------
  
  
  
  //--- Geane info ------------------------------------------------------------------
  // TClonesArray* geaneArray =new TClonesArray("FairTrackParP");
  TClonesArray* geaneArray =new TClonesArray("FairTrackParH");
  tgeane.SetBranchAddress("GeaneTrackFinal",&geaneArray);  //Tracks with parabolic parametrisation
  
  //--- Output histogram -----------------------------------------------------
  // TH1 *hchi2 = new TH1F("hchi2","#chi^2 for reconstructed tracks;#chi^2;",5e2,0,50);
  TH2 *hnRecnMC = new TH2F("hnRecnMC","Number reconstracted tracks vs. Number simulated tracks; N_{MC}; N_{rec}",
			   100,0,100,100,0,100);
  TH1 *hVxRec = new TH1F("hVxRec","Vx^{Rec};Vx, cm;",1e3,-2e0,2e0);
  TH1 *hVyRec = new TH1F("hVyRec","Vy^{Rec};Vy, cm;",1e3,-2e0,2e0);
  TH1 *hVzRec = new TH1F("hVzRec","Vz^{Rec};Vz, cm;",1e3,-2e0,2e0);
  TH1 *hVxMC = new TH1F("hVxMC","Vx^{MC};Vx, cm;",1e3,-2e0,2e0);
  TH1 *hVyMC = new TH1F("hVyMC","Vy^{MC};Vy, cm;",1e3,-2e0,2e0);
  TH1 *hVzMC = new TH1F("hVzMC","Vz^{MC};Vz, cm;",1e3,-2e0,2e0);

  // TH1 *hPxRec = new TH1F("hPxRec","Px^{Rec};Px, GeV/c;",1e3,-5e1,5e1);
  // TH1 *hPyRec = new TH1F("hPyRec","Py^{Rec};Py, GeV/c;",1e3,-5e1,5e1);
  // TH1 *hPzRec = new TH1F("hPzRec","Pz^{Rec};Pz, GeV/c;",1e3,-5e1,5e1);
  // TH1 *hPxMC = new TH1F("hPxMC","Px^{MC};Px, GeV/c;",1e3,-5e1,5e1);
  // TH1 *hPyMC = new TH1F("hPyMC","Py^{MC};Py, GeV/c;",1e3,-5e1,5e1);
  // TH1 *hPzMC = new TH1F("hPzMC","Pz^{MC};Pz, GeV/c;",1e3,-5e1,5e1);

  TH1 *hPRec = new TH1F("hPRec","|P|^{Rec};|P|, GeV/c;",1e3,0,5e1);
  TH1 *hPhiRec = new TH1F("hPhiRec","#phi^{Rec};#phi, rad;",1e2,-3.15,3.15);
  TH1 *hThetaRec = new TH1F("hThetaRec","#theta^{Rec};#theta, rad;",1e3,0,5*0.0175);
  TH1 *hPMC = new TH1F("hPMC","|P|^{MC};|P|, GeV/c;",1e3,0,5e1);
  TH1 *hThetaMC = new TH1F("hThetaMC","#theta^{MC};#theta, rad;",1e3,0,5*0.0175);
  TH1 *hPhiMC = new TH1F("hPhiMC","#phi^{MC};#phi, rad;",1e2,-3.15,3.15);
  
  for (Int_t j=0; j<nEvents; j++){
    //  cout<<"Event #"<<j<<endl;
    // Read GEANE & MC info -----------------------------------------------------------------
    tgeane.GetEntry(j);
    tMC.GetEntry(j);
  
    
    const int nGeaneTrks = geaneArray->GetEntriesFast();
    const int nParticles = true_tracks->GetEntriesFast();
    hnRecnMC->Fill(nGeaneTrks,nParticles);

    ///-- Read info about MC tracks -------------------------------------------
    for(Int_t jk=0; jk< nParticles;jk++){
      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(jk);
      Int_t mcID = mctrk->GetPdgCode();
      TVector3 MomMC_all = mctrk->GetMomentum();
      hPMC->Fill(MomMC_all.Mag());
      Double_t thetaMC_all = MomMC_all.Theta();
      hThetaMC->Fill(thetaMC_all);
      Double_t phiMC_all = MomMC_all.Phi();
      hPhiMC->Fill(phiMC_all);
      TVector3 StartMC_all = mctrk->GetStartVertex();
      hVxMC->Fill(StartMC_all.X());
      hVyMC->Fill(StartMC_all.Y());
      hVzMC->Fill(StartMC_all.Z());
    }
    ///------------------------------------------------------------------------   

    ///-- Read info about GEANE(resonctructed) tracks--------------------------
    for (Int_t iN=0; iN<nGeaneTrks; iN++){
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(iN);
      Double_t lyambda = fRes->GetLambda();
      TVector3 posRec = fRes->GetPosition();
      hVxRec->Fill(posRec.X());
      hVyRec->Fill(posRec.Y());
      hVzRec->Fill(posRec.Z());
      Double_t thetaRec = TMath::Pi()/2. - lyambda;
      hThetaRec->Fill(thetaRec);
      Double_t phiRec = fRes->GetPhi();
      hPhiRec->Fill(phiRec);
      TVector3 MomRec = fRes->GetMomentum();
      hPRec->Fill(MomRec.Mag());
    }
    ///------------------------------------------------------------------------
  }

  hVxRec->Write();
  hVyRec->Write();
  hVzRec->Write();
  hVxMC->Write();
  hVyMC->Write();
  hVzMC->Write();
  hPRec->Write();
  hPhiRec->Write(); 
  hThetaRec->Write();
  hPMC->Write();
  hThetaMC->Write(); 
  hPhiMC->Write(); 
  f->Close();
}
