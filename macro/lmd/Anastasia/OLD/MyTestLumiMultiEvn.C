void MyTestLumiMultiEvn(const int nEvents=2, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=3, double dv=0.5, bool no4d=false)
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

  TString recHit=storePath+"/Lumi_reco_";
  recHit += startEvent;
  recHit += ".root";
  TChain tHits("cbmsim");
  tHits.Add(recHit);

  TString recTrack = storePath+"/Lumi_Track_";
  recTrack += startEvent;
  recTrack += ".root";
  TChain tTrkRec("cbmsim");
  tTrkRec.Add(recTrack);

  TString geaneFile = storePath+"/Lumi_Geane_";
  geaneFile += startEvent;
  geaneFile += ".root";
  TChain tgeane("cbmsim");
  tgeane.Add(geaneFile);

  // ---------------------------------------------------------------------------------
  
  // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/Lumi_out_tracks_";
  out += startEvent;
  out += ".root";
  TFile *f = new TFile(out,"RECREATE");
  // ---------------------------------------------------------------------------------

  //--- MC info -----------------------------------------------------------------
  TClonesArray* true_tracks=new TClonesArray("PndMCTrack");
  tMC.SetBranchAddress("MCTrack",&true_tracks);  //True Track to compare

  Double_t fPxMC=0., fPyMC=0., fPzMC=0.;


  TClonesArray* true_points=new TClonesArray("PndSdsMCPoint");
  tMC.SetBranchAddress("LMDPoint",&true_points);  //True Points to compare

  TH1 *hdiffmom = new TH1F("hdiffmom", "P^{trk}_{MC}-P^{first hit}_{MC}", 1e2,-2e-6,2e-6);
  TH1 *hdiffmomMCtrk = new TH1F("hdiffmomMCtrk", "P^{trk}_{MC}-8.9", 1e2,-2e-6,2e-6);
  TH1 *hdiffmomMChit = new TH1F("hdiffmomMChit", "P^{first hit}_{MC}-8.9", 1e2,-2e-6,2e-6);
  TH1 *hdiffmomRectrk = new TH1F("hdiffmomRectrk", "P^{trk}_{REC}-8.9", 1e2,-2e-6,2e-6);
  //----------------------------------------------------------------------------------

  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array=new TClonesArray("PndSdsHit");
  tHits.SetBranchAddress("LMDHitsStrip",&rechit_array);  //Points for Tracks
  Double_t fPx1HitRec=0., fPy1HitRec=0., fPz1HitRec=0.;
  Double_t hitX=0., hitY=0., hitZ=0., hitR=0.;
  // Double_t hiterrX,hiterrY,hiterrZ;
  // //----------------------------------------------------------------------------------

  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* rec_trk=new TClonesArray("PndLinTrack");
  tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  bool isTrk = false;
  //----------------------------------------------------------------------------------

  //--- Geane info ------------------------------------------------------------------
  // TClonesArray* geaneArray =new TClonesArray("FairTrackParP");
  TClonesArray* geaneArray =new TClonesArray("FairTrackParH");
  tgeane.SetBranchAddress("GeaneTrackFinal",&geaneArray);  //Tracks with parabolic parametrisation

  TVector3 MomIniRec(0,0,0), PointIniRec(0,0,0);
  Double_t fPx=0., fPy=0., fPz=0.;
  Double_t fXv=0., fYv=0., fZv=0.;
  Double_t ferrPx=0., ferrPy=0., ferrPz=0.;
  Double_t ferrXv=0., ferrYv=0., ferrZv=0.;

  TTree *tree = new TTree("Tracks","tracks information");
  tree->Branch("isTrk",&isTrk);
  tree->Branch("Xv",&fXv,"Xv/D");
  tree->Branch("Yv",&fYv,"Yv/D");
  tree->Branch("Zv",&fZv,"Zv/D");
  tree->Branch("errXv",&ferrXv,"errXv/D");
  tree->Branch("errYv",&ferrYv,"errYv/D");
  tree->Branch("errZv",&ferrZv,"errZv/D");
  tree->Branch("Px",&fPx,"Px/D");
  tree->Branch("Py",&fPy,"Py/D");
  tree->Branch("Pz",&fPz,"Pz/D");
  tree->Branch("PxMC",&fPxMC,"PxMC/D");
  tree->Branch("PyMC",&fPyMC,"PyMC/D");
  tree->Branch("PzMC",&fPzMC,"PzMC/D");
  tree->Branch("errPx",&ferrPx,"errPx/D");
  tree->Branch("errPy",&ferrPy,"errPy/D");
  tree->Branch("errPz",&ferrPz,"errPz/D");
  // tree->Branch("Px1HitRec",&fPx1HitRec,"Px1HitRec/D");
  // tree->Branch("Py1HitRec",&fPy1HitRec,"Py1HitRec/D");
  // tree->Branch("Pz1HitRec",&fPz1HitRec,"Pz1HitRec/D");
  
  TH1 *hchi2 = new TH1F("hchi2", "#chi^2", 1e2,0,8);
  TH1 *hangMCRec = new TH1F("hangMCRec", "Angle between Pmc and Prec;#delta#alpha, rad", 200,-1e-3,1e-3);
  TH1 *hResX = new TH1F("hResX", "x^{rec}_{trk} - x_{MC};#deltax, cm", 2e2,-1e-2,1e-2);
  TH1 *hResY = new TH1F("hResY", "y^{rec}_{trk} - y_{MC};#deltay, cm", 2e2,-1e-2,1e-2);
  TH1 *hResRecX = new TH1F("hResRecX", "x^{rec}_{trk} - x^{rec}_{hit};#deltax, cm", 2e2,-1e-2,1e-2);
  TH1 *hResRecY = new TH1F("hResRecY", "y^{rec}_{trk} - y^{rec}_{hit};#deltay, cm", 2e2,-1e-2,1e-2);
  TH1 *hPullX = new TH1F("hPullX", "x^{rec}_{trk}-x^{rec}_{hit}/#sigma_{x}", 2e2,-1e1,1e1);
  TH1 *hPullY = new TH1F("hPullY", "y^{rec}_{trk}-y^{rec}_{hit}/#sigma_{y}", 2e2,-1e1,1e1);
 
 
  // TH1 *hhitzMC0good = new TH1F("hhitzMC0GOOD", "Hit_{z}^{MC}0", 1e2,1098,1101);
  // TH1 *hhitzMC0bad = new TH1F("hhitzMC0BAD", "Hit_{z}^{MC}0", 1e2,1098,1101);
  // TH1 *hhitxMC0good = new TH1F("hhitxMC0GOOD", "Hit_{x}^{MC}0", 1e2,12,42);
  // TH1 *hhityMC0good = new TH1F("hhityMC0GOOD", "Hit_{y}^{MC}0", 1e2,-20,20);
  // TH1 *hhitxMC0bad = new TH1F("hhitxMC0BAD", "Hit_{x}^{MC}0", 1e2,12,42);
  // TH1 *hhityMC0bad = new TH1F("hhityMC0BAD", "Hit_{y}^{MC}0", 1e2,-20,20);
  // TH1 *hhitRMC0good = new TH1F("hhitRMC0GOOD", "Hit_{R}^{MC}0", 1e2,15,35);
  // TH1 *hhitRMC0bad = new TH1F("hhitRMC0BAD", "Hit_{R}^{MC}0", 1e2,15,35);

  Double_t trkpar[4];
  int tot = 0;
  int all=0, uneff=0, mistrk=0, ghosttrk=0;

  // Double_t MCzHit0=0.;//temp
  // Double_t MCxHit0=0.;//temp
  // Double_t MCyHit0=0.;//temp
  for (Int_t j=0; j<nEvents; j++)
  {
    // Read GEANE & MC info -----------------------------------------------------------------
    tgeane.GetEntry(j);
    tMC.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);

    int nGeaneTrks = geaneArray->GetEntriesFast();
    int nParticles = true_tracks->GetEntriesFast();
    int nRecHits = rechit_array->GetEntriesFast();
    int nRecTrks = rec_trk->GetEntriesFast();
    cout<<"nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles<<" nRecTrks"<<nRecTrks<<endl;
  
    if(nGeaneTrks<nParticles)
         mistrk += nParticles-nGeaneTrks;
    
    if(nGeaneTrks>nParticles)
      ghosttrk += nGeaneTrks-nParticles;
    if(nParticles!=1) continue;
    //  if(nGeaneTrks!=1) continue;

   
    // TVector3 MCmomHit;
    // mchit->Momentum(MCmomHit);
    // TVector3 MCposHit = mchit->GetPosition();
    // MCzHit0 = MCposHit.Z();
    // MCxHit0 = MCposHit.X();
    // MCyHit0 = MCposHit.Y();
   
    
    // //For checking diff
    // PndMvdHit *rechit = (PndMvdHit*) rechit_array->At(0);

    // TVector3 RecHit;
    // rechit->Position(RecHit);
    // if(rechit>);
    // TVector3 RecmomHit;
    // rechit->Momentum(RecmomHit);
    // Px1HitRec = RecmomHit.X();
    // Py1HitRec = RecmomHit.Y();
    // Pz1HitRec = RecmomHit.Z();
    //---------------------------------------

    for (Int_t i=0; i<nGeaneTrks; i++){
      //   FairTrackParP *fRes = (FairTrackParP*)geaneArray->At(i);
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(i);
      fPx = fRes->GetPx();     fPy = fRes->GetPy();     fPz = fRes->GetPz();
      fXv = fRes->GetX();      fYv = fRes->GetY();      fZv = fRes->GetZ();
      ferrPx = fRes->GetDPx(); ferrPy = fRes->GetDPy(); ferrPz = fRes->GetDPz();
      //  if(ferrPx<1e-04 && ferrPy<1e-04 && ferrPz<1e-5) break;
      ferrXv = fRes->GetDX();  ferrYv = fRes->GetDY();  ferrZv = fRes->GetDZ();
      TVector3 MomRec(fPx,fPy,fPz);
      
      for(Int_t jk=0; jk< nParticles;jk++){
      	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(jk);
	TVector3 P = mctrk->GetMomentum();

	/// Obtain resolution (begin) -------------------------------------------
	Double_t angMCRec = P.Angle(MomRec); // Angle between two vectors
	hangMCRec->Fill(angMCRec);
	//	if(angMCRec>1e-3) continue;
	PndLinTrack *trk = (PndLinTrack*)rec_trk->At(i);
	double chi2 = trk->GetChiSquare();
	hchi2->Fill(chi2);
	trk->GetPar(trkpar);
	Double_t P0 =trkpar[0]; 
	Double_t P1 =trkpar[1]; 
	Double_t P2 =trkpar[2]; 
	Double_t P3 =trkpar[3];  

	int nMChits = true_points->GetEntriesFast();
	//    cout<<"Number of hits from MC is "<<nMChits<<endl;	

	//	for(int ir=0;ir<nMChits;ir++){
	for(int ir=0;ir<1;ir++){
	  PndSdsMCPoint *mchit = (PndSdsMCPoint*) true_points->At(ir);

	  TVector3 MCposHit = mchit->GetPosition();
	  Double_t t = MCposHit.Z()-1099.;
	  Double_t xRec = P0 + P1*t;
	  Double_t yRec = P2 + P3*t;
	  //  cout<<"dX="<<xRec-MCposHit.X()<<" = "<<xRec<<" - "<<MCposHit.X()<<endl;
	  //  cout<<"dY="<<yRec-MCposHit.Y()<<" = "<<yRec<<" - "<<MCposHit.Y()<<endl;
	  hResX->Fill(xRec-MCposHit.X());
	  hResY->Fill(yRec-MCposHit.Y());

	  for(int irr=0;irr<1;irr++){
	  PndSdsHit *hit=(PndSdsHit*) rechit_array->At(irr);
	  TVector3 rechit = hit->GetPosition();
	  if(fabs(MCposHit.Z()-rechit.Z())>8e-3) continue;
	  double coordX = rechit.X();
	  double coordY = rechit.Y();
	  hResRecX->Fill(xRec-coordX);
	  hResRecY->Fill(yRec-coordY);
	  TMatrixD CovRec = hit->GetCov();
	  errX = sqrt(CovRec[0][0]);
	  errY = sqrt(CovRec[1][1]);
	  // hPullX->Fill((xRec-MCposHit.X())/errX);
	  // hPullY->Fill((yRec-MCposHit.Y())/errY);
	  hPullX->Fill((xRec-coordX)/errX);
	  hPullY->Fill((yRec-coordY)/errY);
	  }
	}
	/// Obtain resolution (end) -------------------------------------------

	Int_t mcID = mctrk->GetPdgCode();
	if(verboseLevel>0 && mcID!=-2212){
	  cout << "Event: "<< j <<endl;
	  cout<<"mcID "<<mcID<<endl;
	  cout<<"nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles<<endl;
	}
      
      
	fPxMC = P.X(); fPyMC = P.Y(); fPzMC = P.Z();
	tree->Fill(); //save results
	//	hdiffmom->Fill(P.Mag()-MCmomHit.Mag());
	hdiffmomMCtrk->Fill(P.Mag()-8.9);
	//	hdiffmomMChit->Fill(MCmomHit.Mag()-8.9);
	hdiffmomRectrk->Fill(MomRec.Mag()-8.9);
	// if(fabs(MomRec.Mag()-8.9)>5e-6){
      // 	hhitzMC0bad->Fill(MCzHit0);
      // 	hhitxMC0bad->Fill(MCxHit0);
      // 	hhityMC0bad->Fill(MCyHit0);
      // 	hhitRMC0bad->Fill(TMath::Hypot(MCxHit0,MCyHit0));
      // }
      // else{
      // 	hhitzMC0good->Fill(MCzHit0);
      // 	hhitxMC0good->Fill(MCxHit0);
      // 	hhityMC0good->Fill(MCyHit0);
      // 	hhitRMC0good->Fill(TMath::Hypot(MCxHit0,MCyHit0));
      // }
      }
    }
    //----------------------------------------------------------------------------------
  }
  //----------------------------------------------------------------------------------
   
  tree->Write(tree->GetName(),TObject::kOverwrite);
  hchi2->Write();
  TCanvas *c1 = new TCanvas();
  c1->Divide(2,2);
  c1->cd(1);
  hdiffmomMCtrk->Draw();
  c1->cd(2);
  hdiffmomMChit->Draw();
  c1->cd(3);
  hdiffmom->Draw();
  c1->cd(4);
  hdiffmomRectrk->Draw();
  // hdiffmom->Write();
  // hdiffmomMCtrk->Write();
  // hdiffmomMChit->Write();
  // hdiffmomRectrk->Write();
  c1->Write();
  // hhitzMC0bad->Write();
  // hhitzMC0good->Write();
  // hhitxMC0bad->Write();
  // hhitxMC0good->Write();
  // hhityMC0bad->Write();
  // hhityMC0good->Write();
  // hhitRMC0good->Write();
  // hhitRMC0bad->Write();
  TCanvas *c2 = new TCanvas();
  c2->Divide(2,3);
  c2->cd(1);
  hResX->Draw();
  c2->cd(2);
  hResY->Draw();
  c2->cd(3);
  hResRecX->Draw();
  
  c2->cd(4);
  hResRecY->Draw();
 
  // hangMCRec->Draw();
  // hangMCRec->Write();
  // hResX->Write();
  // hResY->Write();
  c2->cd(5);
  hPullX->Draw();
  c2->cd(6);
  hPullY->Draw();
  c2->Write();
  f->Close();
  cout<<"Number of missed tracks is "<<mistrk<<" and number of ghost tracks is "<<ghosttrk<<endl;
}
