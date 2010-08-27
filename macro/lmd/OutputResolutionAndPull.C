void OutputResolutionAndPull(const int nEvents=2, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=3, double dv=0.5, bool no4d=false)
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
  TString out=storePath+"/Lumi_out_resolutions_and_pulls_";
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
  //----------------------------------------------------------------------------------

  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array=new TClonesArray("PndSdsHit");
  tHits.SetBranchAddress("LMDHitsStrip",&rechit_array);  //Points for Tracks
  Double_t fPx1HitRec=0., fPy1HitRec=0., fPz1HitRec=0.;
  Double_t hitX=0., hitY=0., hitZ=0., hitR=0.;
  //----------------------------------------------------------------------------------

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
  Double_t fPrec=0.; Double_t ferrPrec=0.;

  //--- Output histogram -----------------------------------------------------
  TH1 *hchi2 = new TH1F("hchi2", "#chi^{2}", 3e2,0,10);

  TH2 *hchi2ang = new TH2D("chi2vsang", "hchi2 vs. angle between Pmc and Prec; #delta#alpha, rad;#chi^{2}", 
			   1e3,0.,0.001, 1e3,0,100);
  TH2 *hMom2D = new TH2D("hMom2D","P_{MC} vs. P_{rec}; P_{rec},GeV/c; P_{MC},GeV/c",
			 1e3,-1e-6,1e-6,1e3,-1e-6,1e-6);
  TH2 *hMomchi2 = new TH2D("hMomchi2","#delta P vs. #chi^{2};#chi^{2};#deltaP, GeV/c",1e3,0,100,1e4,-1e-2,1e-2);
  TH2 *hMomR = new TH2D("hMomR","#delta P vs. R_{PCA};R_{PCA}, cm;#deltaP, GeV/c",1e3,0,1,1e4,-1e-2,1e-2);
  TH2 *hMomXY = new TH2D("hMomXY","#delta P vs. XY_{PCA};XY_{PCA}, cm;#deltaP, GeV/c",1e3,0,1,1e4,-1e-2,1e-2);
  TH2 *hMomZ = new TH2D("hMomZ","#delta P vs. Z_{PCA};Z_{PCA}, cm;#deltaP, GeV/c",1e3,-0.05,0.05,1e4,-1e-2,1e-2);
  TH2 *hchi2R = new TH2D("hchi2R","#chi^{2} vs. R_{PCA};R_{PCA}, cm;#chi^{2}",1e4,0,1e0,1e3,0,1e1);
  TH2 *hangR = new TH2D("hangR","Angle between Pmc and Prec vs. R_{PCA};R_{PCA}, cm;#delta#alpha, rad",
			1e3,0,1,1e3,0.,0.001);
  TH1 *hangMCRec = new TH1F("hangMCRec", "Angle between Pmc and Prec;#delta#alpha, rad", 1e4,0.,1);
  TH1 *hResMom = new TH1F("hResMom","P_{MC}-P_{rec};#deltaP,GeV/c",1e3,-1e-5,1e-5);
  TH1 *hResMomIO = new TH1F("hResMomIO","P^{trk-fit}_{rec}-P^{geane}_{rec};#deltaP,GeV/c",1e3,-1e-5,1e-5);
  TH1 *hResMomIMC = new TH1F("hResMomIMC","P_{MC}-P^{trk-fit}_{rec};#deltaP,GeV/c",1e3,-1e-5,1e-5);
  TH1 *hErrMom = new TH1F("hErrMom","#sigma_{P};#sigmaP,GeV/c",1e3,0,1e-3);
  TH1 *hPullMom = new TH1F("hPullMom","(P_{MC}-P_{rec})/#sigma_{P};",1e2,-1e-2,1e-2);
  TH1 *hResTheta = new TH1F("hResTheta","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e3,-1e-2,1e-2);
  TH1 *hErrTheta = new TH1F("hErrTheta","#sigma(#theta_{rec});#sigma,rad",200,0,1e-3);
  TH1 *hPullTheta = new TH1F("hPullTheta","(#theta_{MC}-#theta_{rec})/#sigma_{#theta};",1e2,-20,20);
  TH1 *hResPhi = new TH1F("hResPhi","#phi_{MC}-#phi_{rec};#delta#phi,rad",1e3,-1,1);
  TH1 *hErrPhi = new TH1F("hErrPhi","#sigma(#phi_{rec});#sigma,rad",200,0,0.2);
  TH1 *hPullPhi = new TH1F("hPullPhi","(#phi_{MC}-#phi_{rec})/#sigma_{#phi};",1e2,-20,20);

  TH1 *hResMomX = new TH1F("hResMomX","(P_{MC}-P_{rec})_{X};#deltaP,GeV/c",2e2,-7e-3,7e-3);
  TH1 *hResMomY = new TH1F("hResMomY","(P_{MC}-P_{rec})_{Y};#deltaP,GeV/c",2e2,-7e-3,7e-3);
  TH1 *hResMomZ = new TH1F("hResMomZ","(P_{MC}-P_{rec})_{Z};#deltaP,GeV/c",2e2,-1e-4,1e-4);
  TH1 *hErrMomX = new TH1F("hErrMomX","#sigma_{Px};#sigmaPx,GeV/c",1e3,0,1e-3);
  TH1 *hErrMomY = new TH1F("hErrMomY","#sigma_{Py};#sigmaPy,GeV/c",1e3,0,1e-3);
  TH1 *hErrMomZ = new TH1F("hErrMomZ","#sigma_{Pz};#sigmaPz,GeV/c",1e4,0,1e-2);
  TH1 *hPullMomX = new TH1F("hPullMomX","(P_{MC}-P_{rec})_{X}/#sigma_{Px};",1e2,-10,10);
  TH1 *hPullMomY = new TH1F("hPullMomY","(P_{MC}-P_{rec})_{Y}/#sigma_{Py};",1e2,-10,10);
  TH1 *hPullMomZ = new TH1F("hPullMomZ","(P_{MC}-P_{rec})_{Z}/#sigma_{Pz};",4e2,-10,10);
  
  TH1 *hResPointX = new TH1F("hResPointX","X_{MC}-X_{rec};#deltaX,cm",2e2,-1.5,1.5);
  TH1 *hResPointY = new TH1F("hResPointY","Y_{MC}-Y_{rec};#deltaY,cm",2e2,-1.5,1.5);
  TH1 *hResPointZ = new TH1F("hResPointZ","Z_{MC}-Z_{rec};#deltaZ,cm",2e2,-3e-2,3e-2);
  TH1 *hErrPointX = new TH1F("hErrPointX","#sigma_{X};#sigmaX,cm",1e2,0,5e-1);
  TH1 *hErrPointY = new TH1F("hErrPointY","#sigma_{Y};#sigmaY,cm",1e2,0,5e-1);
  TH1 *hErrPointZ = new TH1F("hErrPointZ","#sigma_{Z};#sigmaZ,cm",1e2,0,5e-3);
  TH1 *hPullPointX = new TH1F("hPullPointX","(X_{MC}-X_{rec})/#sigma_{X};",1e2,-10,10);
  TH1 *hPullPointY = new TH1F("hPullPointY","(Y_{MC}-Y_{rec})/#sigma_{Y};",1e2,-10,10);
  TH1 *hPullPointZ = new TH1F("hPullPointZ","(Z_{MC}-Z_{rec})/#sigma_{Z};",1e2,-10,10);

  TH1 *hPointIN = new TH1F("hPointIN","Initial point ;xy,cm",4e2,10,50);
  TH1 *hPointOUT = new TH1F("hPointOUT","Final point ;xy,cm",1e3,0,1);

  TH1 *hThetaUneff = new TH1F("hThetaUneff","#theta for missed tracks;#theta, rad",1e2,1e-4,1.3e-2);
  TH2 *hThetaeffR = new TH2F("hThetaeffR","#theta for reconstructed tracks vs. R_{PCA};R_{PCA}, cm; #theta, rad",
			       1e3,0,2.,1e3,1e-4,1.3e-2);
  TH1 *hPhiUneff = new TH1F("hPhiUneff","#phi for missed tracks;#phi, rad", 1e2,-TMath::Pi(),TMath::Pi());
  TH2 *hnRecnMC = new TH2F("hnRecnMC","Number reconstracted tracks vs. Number simulated tracks; N_{MC}; N_{rec}",
			   20,0,20,20,0,20);
  TH2 *hPullThetaAng = new TH2F("hPullThetaAng","|(#theta_{MC}-#theta_{rec})|/#sigma_{#theta} vs. Angle between Pmc and Prec; #delta#alpha, rad; |(#theta_{MC}-#theta_{rec})|/#sigma_{#theta}", 1e2,0.,1e-3,1e2,0,10);
  TH2 *hPullThetaR = new TH2F("hPullThetaR","|(#theta_{MC}-#theta_{rec})|/#sigma_{#theta} vs. R_{PCA};R_{PCA}, cm;  |(#theta_{MC}-#theta_{rec})|/#sigma_{#theta}", 1e2,0.,1,1e2,0,10);
  //---------------------------------------------------------------------------

  Double_t trkpar[4];
  int tot = 0;
  int all=0, uneff=0, mistrk=0, ghosttrk=0;

  for (Int_t j=0; j<nEvents; j++)
  {
    // Read GEANE & MC info -----------------------------------------------------------------
    tgeane.GetEntry(j);
    tMC.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);

    const int nGeaneTrks = geaneArray->GetEntriesFast();
    const int nParticles = true_tracks->GetEntriesFast();
    const int nRecHits = rechit_array->GetEntriesFast();
    const int nRecTrks = rec_trk->GetEntriesFast();
    hnRecnMC->Fill(nParticles,nGeaneTrks);
    //  cout<<"#"<<j<<" nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles
    //	<<" nRecTrks"<<nRecTrks<<" nRecHits = "<<nRecHits<<endl;
  
    if(nGeaneTrks<nParticles)
         mistrk += nParticles-nGeaneTrks;
    
    if(nGeaneTrks>nParticles)
      ghosttrk += nGeaneTrks-nParticles;

    //if(nParticles!=1 || nRecTrks<1) continue;
    if(nParticles!=1) continue;
    //cout<<"#"<<j<<" nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles<<" nRecTrks="<<nRecTrks<<endl;
    vector<int> missedTrk;
    missedTrk.resize(nParticles);
    vector<int> ghostTrk;
    ghostTrk.resize(nGeaneTrks);
    for(int jmc=0;jmc<nParticles;jmc++)
      missedTrk[jmc] = 1;
    for (Int_t i=0; i<nGeaneTrks; i++){
      PndLinTrack *trk = (PndLinTrack*)rec_trk->At(i);
      Int_t rectrkID = trk->GetTCandID();
      ///Particle momentum from reconstruction (track fit)
      Double_t parrecTrk[4];
      trk->GetPar(parrecTrk);
      Double_t p0 = parrecTrk[0];
      Double_t p1 = parrecTrk[1];
      Double_t p2 = parrecTrk[2];
      Double_t p3 = parrecTrk[3];
  
      Double_t parrecTrkErr[4];
      trk->GetParErr(parrecTrkErr);
      
      //Vector of particle momentum and starting point
      TVector3  DirVec =  trk->GetDirectionVec();
      Double_t fPbeam = 4.06;
      Double_t Pnorm = fPbeam/DirVec.Mag();
      TVector3 StartMom = TVector3(p1*Pnorm,p3*Pnorm,Pnorm); 
      TVector3 StartPos = trk->GetStartVec();
      // hPointIN->Fill(TMath::Hypot(StartPos.X(),StartPos.Y()));
      ///------------------------------------------------


      double chi2 = trk->GetChiSquare();
   
      //  if(chi2>0.1) continue;
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(i);
      Double_t lyambda = fRes->GetLambda();
      Double_t thetaRec = TMath::Pi()/2. - lyambda;
      Double_t err_lyambda = fRes->GetDLambda();
      //     cout<<"err_lyambda = "<<err_lyambda<<" sqrt(err_lyambda)"<<TMath::Sqrt(err_lyambda)<<endl;
      Double_t phi = fRes->GetPhi();
      Double_t err_phi = fRes->GetDPhi();
      TVector3 MomRec = fRes->GetMomentum();
      Double_t CovRec[15];
      fRes->GetCov(CovRec);
      TVector3 PosRec = fRes->GetPosition();
      hThetaeffR->Fill(PosRec.Mag(),thetaRec);
     
      // if(PosRec.Mag()>1 || chi2>10.){
      // 	if(angMCRec<1e-3) missedTrk[jk] = 0;
      // 	continue; //cut on 1 cm
      // }

      //  if(PosRec.Mag()<0.1) continue;
      //      if(PosRec.Mag()<0.5 || chi2>10.) continue; //cut on 1 cm
      hchi2->Fill(chi2);
      hchi2R->Fill(PosRec.Mag(),chi2);
      hPointOUT->Fill(PosRec.Mag());
      hPointIN->Fill(TMath::Hypot(StartPos.X(),StartPos.Y()));
      //Double_t errMomRec = TMath::Sqrt(CovRec[0])*MomRec.Mag2();
      //      Double_t ErrPcalc = TMath::Sqrt(errPx*errPx+errPy*errPy+errPz*errPz);
      Double_t errPx = fRes->GetDPx();
      Double_t errPy = fRes->GetDPy();
      Double_t errPz = fRes->GetDPz();
      Double_t errMomRec = TMath::Sqrt(errPx*errPx+errPy*errPy+errPz*errPz);
      hErrMomX->Fill(errPx);
      hErrMomY->Fill(errPy);
      hErrMomZ->Fill(errPz);
      //  cout<<"errPx = "<<errPx<<" errPy = "<<errPy<<" errPz = "<<errPz<<endl;
      Double_t errX = fRes->GetDX();
      Double_t errY = fRes->GetDY();
      Double_t errZ = fRes->GetDZ();
      hErrPointX->Fill(errX);
      hErrPointY->Fill(errY);
      hErrPointZ->Fill(errZ);
     
      for(Int_t jk=0; jk< nParticles;jk++){
	//	missedTrk[jk] = false;
      	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(jk);
	Int_t mcID = mctrk->GetPdgCode();
	if(verboseLevel>0 && mcID!=-2212){
	  cout << "Event: "<<j<<" MC trk#"<<jk
	       <<" mcID "<<mcID
	       <<" rectrkID "<<rectrkID<<" [nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles<<"]"<<endl;
	}
	//	if(mcID!=-2212) continue;
	TVector3 MomMC = mctrk->GetMomentum();
	Double_t angMCRec = MomMC.Angle(MomRec); // Angle between two vectors

	//	if(fabs(MomMC.Mag()-MomRec.Mag())<5e-7) continue;
	//	if((MomMC.Mag()-MomRec.Mag())>-6e-7) continue;
	//	cout<<"#"<<j<<" nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles<<" nRecTrks"<<nRecTrks<<endl;
	//	if(chi2>0.5) continue;
	hangMCRec->Fill(angMCRec);
	// if(PosRec.Mag()>1 || chi2>10.){
	//	if(angMCRec<1e-3) missedTrk[jk] = 0;
	//   continue;
	// }

	if(PosRec.Mag()>0.8 || chi2>10.) continue;
	// if(angMCRec>1e-3 && mcID==-2212) missedTrk.push_back(0);
	// else missedTrk.push_back(1);
	//	if(angMCRec>1e-3) missedTrk[jk] = 1;
	//	if(angMCRec>1e-3) continue;
	//	if(angMCRec>1e-3) continue;
	 if(angMCRec>1e-3) continue;
	// else missedTrk[jk] = 0;
	
	hchi2ang->Fill(angMCRec,chi2);
	hMomchi2->Fill(chi2,(MomMC.Mag()-MomRec.Mag()));

	hMom2D->Fill(MomRec.Mag()-4.06,MomMC.Mag()-4.06);
	hResMom->Fill(MomMC.Mag()-MomRec.Mag());
	hResMomIO->Fill(StartMom.Mag()-MomRec.Mag());
	hResMomIMC->Fill(MomMC.Mag()-StartMom.Mag());
	hPullMom->Fill((MomMC.Mag()-MomRec.Mag())/errMomRec);
	hErrMom->Fill(errMomRec);

	hResMomX->Fill(MomMC.X()-MomRec.X());
	hResMomY->Fill(MomMC.Y()-MomRec.Y());
	hResMomZ->Fill(MomMC.Z()-MomRec.Z());
	hPullMomX->Fill((MomMC.X()-MomRec.X())/errPx);
	hPullMomY->Fill((MomMC.Y()-MomRec.Y())/errPy);
	hPullMomZ->Fill((MomMC.Z()-MomRec.Z())/errPz);

	// if(fabs(MomMC.Mag()-4.06)>1e-8){
	//   cout<<"MomMC.Mag()-4.06 = "<<MomMC.Mag()-4.06<<endl;	
	//   cout<<"MomRec.Mag()-4.06 = "<<MomRec.Mag()-4.06<<endl;	
	//   cout<<"Delta P = "<<MomMC.Mag()-MomRec.Mag()<<" errMomRec = "<<errMomRec
	//       <<" Pull = "<<(MomMC.Mag()-MomRec.Mag())/errMomRec<<endl;
	// }
	hResTheta->Fill(MomMC.Theta()-thetaRec);
	hErrTheta->Fill(err_lyambda);
	hPullTheta->Fill((MomMC.Theta()-thetaRec)/err_lyambda);
	hPullThetaAng->Fill(angMCRec,fabs(MomMC.Theta()-thetaRec)/err_lyambda);
	hPullThetaR->Fill(PosRec.Mag(),fabs(MomMC.Theta()-thetaRec)/err_lyambda);
	hResPhi->Fill(MomMC.Phi()-phi);
	hErrPhi->Fill(err_phi);
	hPullPhi->Fill((MomMC.Phi()-phi)/err_phi);
	TVector3 MCvertex = mctrk->GetStartVertex();
	hResPointX->Fill(PosRec.X()-MCvertex.X());
	hResPointY->Fill(PosRec.Y()-MCvertex.Y());
	hResPointZ->Fill(PosRec.Z()-MCvertex.Z());
	// cout<<"diff for coord: dx="<<PosRec.X()-MCvertex.X()
	//     <<" dy="<<PosRec.Y()-MCvertex.Y()<<" dz="<<PosRec.Z()-MCvertex.Z()
	//     <<". And PosRec.Mag() = "<<PosRec.Mag()<<endl;
	//	cout<<"MCvertex.Mag() = "<<MCvertex.Mag()<<endl;
	//	MCvertex.Print();
	
	hMomR->Fill(PosRec.Mag(),(MomMC.Mag()-MomRec.Mag()));
	hMomZ->Fill(PosRec.Z(),(MomMC.Mag()-MomRec.Mag()));
	//	cout<<"Hypot = "<<TMath::Hypot(PosRec.X(),PosRec.Y())<<" Perp = "<<PosRec.Perp()<<endl;
	hMomXY->Fill(PosRec.Perp(),(MomMC.Mag()-MomRec.Mag()));

	hangR->Fill(PosRec.Mag(),angMCRec);

	hPullPointX->Fill((PosRec.X()-MCvertex.X())/errX);
	hPullPointY->Fill((PosRec.Y()-MCvertex.Y())/errY);
	hPullPointZ->Fill((PosRec.Z()-MCvertex.Z())/errZ);
      }

      


    }
    //----------------------------------------------------------------------------------

    ///Unefficiency --------------------------------------------------------------
    for(Int_t jk=0; jk< nParticles;jk++){
      if(missedTrk[jk]!=0) continue;
	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(jk);
	TVector3 MomMC = mctrk->GetMomentum();
	hThetaUneff->Fill(MomMC.Theta());
	hPhiUneff->Fill(MomMC.Phi());
    }
    ///---------------------------------------------------------------------------
  }
  //----------------------------------------------------------------------------------  
  TCanvas *cmom = new TCanvas("cmom");
  cmom->Divide(2,2);
  cmom->cd(1);
  hMom2D->Draw();
  //  hMomXY->Draw();
  cmom->cd(2);
  //  hMomchi2->Draw();
  hMomZ->Draw();
  cmom->cd(3);
  hMomR->Draw();
  cmom->cd(4);
  // hchi2R->Draw("SURF2");
  // hchi2R->Draw("CONT0");
  hchi2R->Draw();
  cmom->Write();

  TCanvas *c1 = new TCanvas();
  c1->Divide(3,2);
  c1->cd(1);
  hchi2->Draw();
  c1->cd(2);
  hangMCRec->Draw();
  c1->cd(3);
  //  hchi2ang->Draw();
  // hPointOUT->Draw();
  hThetaeffR->Draw();
  c1->cd(4);
  //  hangR->Draw();
  //  hPointIN->Draw();
  hPhiUneff->Draw();
  c1->cd(5);
  hThetaUneff->Draw();
  c1->cd(6);
  // hnRecnMC->Draw("LEGO2");
  hnRecnMC->Draw();
  c1->Write();


   TF1 *funrp = new TF1("fitrp","gaus",-1e-5,1e-5);
   funrp->SetParameters(100,0,3e-3);
   funrp->SetParNames("Constant","Mean","Sigma");

   TF1 *funrth = new TF1("fitrth","gaus",-0.01,0.01);
   funrth->SetParameters(100,0,3e-3);
   funrth->SetParNames("Constant","Mean","Sigma");

   TF1 *funrphi = new TF1("fitrth","gaus",-1,1);
   funrphi->SetParameters(100,0,3e-3);
   funrphi->SetParNames("Constant","Mean","Sigma");

   TF1 *funp = new TF1("fitp","gaus",-20,20);
   funp->SetParameters(100,0,3e-3);
   funp->SetParNames("Constant","Mean","Sigma");

   TCanvas *cdiffmom = new TCanvas("diffmom");
   hResMom->Draw();
   hResMomIO->SetLineColor(kBlue);
   hResMomIO->Draw("same");
   hResMomIMC->SetLineColor(kGreen);
   hResMomIMC->Draw("same");
   cdiffmom->Write();

   TCanvas *c2 = new TCanvas();
   //  c2->Divide(3,3);
   c2->Divide(3,2);
   c2->cd(1);
   hResMom->Fit(funrp,"r");
   hResMom->Draw();
   // hResMomIO->SetLineColor(kBlue);
   // hResMomIO->Draw("same");
   // hResMomIMC->SetLineColor(kGreen);
   // hResMomIMC->Draw("same");
   c2->cd(2);
   hResTheta->Fit(funrth,"r");
   hResTheta->Draw();
   c2->cd(3);
   hResPhi->Fit(funrphi,"r");
   hResPhi->Draw();
   c2->cd(4);
   hPullMom->Fit(funp,"r");
   hPullMom->Draw();
   c2->cd(5);
   hPullTheta->Fit(funp,"r");
   hPullTheta->Draw();
   c2->cd(6);
   hPullPhi->Fit(funp,"r");
   hPullPhi->Draw();
   // c2->cd(7);
   // hErrMom->Draw();
   // // hPullThetaAng->Draw();
   // c2->cd(8);
   // hErrTheta->Draw();
   // c2->cd(9);
   // hErrPhi->Draw();
   c2->Write();

   TCanvas *ctheta = new TCanvas("thetapulls");
   hPullThetaR->FitSlicesX();
   TH1 *fpullThetaRMean = (TH1F*)gDirectory->Get("hPullThetaR_1");
   TH1 *fpullThetaRSigma = (TH1F*)gDirectory->Get("hPullThetaR_2");
   ctheta->Divide(3,2);
   ctheta->cd(1);
   hPullThetaR->Draw();
   ctheta->cd(2);
   fpullThetaRMean->Draw();
   ctheta->cd(3);
   fpullThetaRSigma->Draw();

   hPullThetaAng->FitSlicesX();
   TH1 *fpullThetaAngMean = (TH1F*)gDirectory->Get("hPullThetaAng_1");
   TH1 *fpullThetaAngSigma = (TH1F*)gDirectory->Get("hPullThetaAng_2");
   ctheta->cd(4);
   hPullThetaAng->Draw();
   ctheta->cd(5);
   fpullThetaAngMean->Draw();
   ctheta->cd(6);
   fpullThetaAngSigma->Draw();
   ctheta->Write();

   TCanvas *ctheta2 = new TCanvas("thetapulls2");
   hPullThetaR->FitSlicesY();
   TH1 *fpullThetaRMeanY = (TH1F*)gDirectory->Get("hPullThetaR_1");
   TH1 *fpullThetaRSigmaY = (TH1F*)gDirectory->Get("hPullThetaR_2");
   ctheta2->Divide(3,2);
   ctheta2->cd(1);
   hPullThetaR->Draw();
   ctheta2->cd(2);
   fpullThetaRMeanY->Draw();
   ctheta2->cd(3);
   fpullThetaRSigmaY->Draw();

   hPullThetaAng->FitSlicesY();
   TH1 *fpullThetaAngMeanY = (TH1F*)gDirectory->Get("hPullThetaAng_1");
   TH1 *fpullThetaAngSigmaY = (TH1F*)gDirectory->Get("hPullThetaAng_2");
   ctheta2->cd(4);
   hPullThetaAng->Draw();
   ctheta2->cd(5);
   fpullThetaAngMeanY->Draw();
   ctheta2->cd(6);
   fpullThetaAngSigmaY->Draw();
   ctheta2->Write();
   // hPullThetaR->Write();
   //  hPullThetaAng->Write();

   TCanvas *c3 = new TCanvas();
   c3->Divide(3,3);
   c3->cd(1);
   hResMomX->Draw();
   c3->cd(2);
   hResMomY->Draw();
   c3->cd(3);
   hResMomZ->Draw();
   c3->cd(4);
   hErrMomX->Draw();
   c3->cd(5);
   hErrMomY->Draw();
   c3->cd(6);
   hErrMomZ->Draw();
   c3->cd(7);
   hPullMomX->Fit(funp,"r");
   hPullMomX->Draw();
   c3->cd(8);
   hPullMomY->Fit(funp,"r");
   hPullMomY->Draw();
   c3->cd(9);
   hPullMomZ->Fit(funp,"r");
   hPullMomZ->Draw();
   c3->Write();

   
   TCanvas *c4 = new TCanvas();
   c4->Divide(3,3);
   c4->cd(1);
   hResPointX->Draw();
   c4->cd(2);
   hResPointY->Draw();
   c4->cd(3);
   hResPointZ->Draw();
   c4->cd(4);
   hErrPointX->Draw();
   c4->cd(5);
   hErrPointY->Draw();
   c4->cd(6);
   hErrPointZ->Draw();
   c4->cd(7);
   hPullPointX->Fit(funp,"r");
   hPullPointX->Draw();
   c4->cd(8);
   hPullPointY->Fit(funp,"r");
   hPullPointY->Draw();
   c4->cd(9);
   hPullPointZ->Fit(funp,"r");
   hPullPointZ->Draw();
   c4->Write();

   f->Close();
   cout<<"Number of missed tracks is "<<mistrk<<" and number of ghost tracks is "<<ghosttrk<<endl;
}
