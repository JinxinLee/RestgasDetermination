double DrawProgressBar(int len, double percent, double last_percent) {
  if ((int)(last_percent*100) == (int)(percent*100)) return last_percent;
  //cout << " drawing " << endl;
  cout << "\x1B[2K"; // Erase the entire current line.
  cout << "\x1B[0E"; // Move to the beginning of the current line.
  string progress;
  for (int i = 0; i < len; ++i) {
    if (i < static_cast<int>(len * percent)) {
      progress += "=";
    } else {
      progress += " ";
    }
  }
  cout << "[" << progress << "] " << (static_cast<int>(100 * percent))
<< "%";
  flush(cout); // Required.
  last_percent = percent;
  return last_percent;
}


void runLumi6Fit(const int nEvents=100000, const double mom=15, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0)
{
  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"/Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  // Digi file
  TString RecoFile = storePath+"/Lumi_reco_";
  RecoFile += startEvent;
  RecoFile += ".root";
  // TCand file
  TString CandFile = storePath+"/Lumi_TCand_";
  CandFile += startEvent;
  CandFile += ".root";
  // Parameter file
  TString parFile = storePath+"/Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";
  // Track file
  TString TrkFile = storePath+"/Lumi_Track_";
  TrkFile += startEvent;
  TrkFile += ".root";
  // Output file
  TString GeaFile = storePath+"/Lumi_Geane_";
  GeaFile += startEvent;
  GeaFile += ".root";

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
//-lRooFit –lRooFitCore -lMinuit
  gSystem->Load("libRooFit");
  gSystem->Load("libRooFitCore");
  gSystem->Load("libMinuit");
  gSystem->Load("libSds");
  gSystem->Load("libSdsReco");
  gSystem->Load("libLmd");
  gSystem->Load("libLmdReco");
  gSystem->Load("libLmdTrk");
  gSystem->Load("libLmdFit");
  // ------------------------------------------------------------------------
    
  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
  std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
  std::cout << "TCandFile: " << CandFile.Data()<< std::endl;
  std::cout << "TrackFile: " << TrkFile.Data()<< std::endl;
  std::cout << "GeaneFile: " << GeaFile.Data()<< std::endl;


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  TH1D *hTheta = new TH1D("hTheta","#theta_{rec} rad",1e2,0,10);

  TChain tgeane("cbmsim");
  tgeane.Add(GeaFile);

  TClonesArray* geaneArray =new TClonesArray("FairTrackParH");
  tgeane.SetBranchAddress("GeaneTrackFinal",&geaneArray);  //Tracks with parabolic parametrisation

  TVector3 MomIniRec(0,0,0), PointIniRec(0,0,0);
  Double_t fPrec=0.; Double_t ferrPrec=0.;

 Double_t trkpar[4];
  int tot = 0;
  int all=0, uneff=0, mistrk=0, ghosttrk=0;
  double last=0;

  for (Int_t j=0; j<nEvents; j++)
  {
    // Read GEANE -----------------------------------------------------------------
    tgeane.GetEntry(j);
    last = DrawProgressBar(50, (j+1)/((double)nEvents),last);
    const int nGeaneTrks = geaneArray->GetEntriesFast();

    for (Int_t i=0; i<nGeaneTrks; i++){
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(i);
        if(!fRes) { cout << "no fRes!" << endl; continue;}
      Double_t lyambda = fRes->GetLambda();
      Double_t thetaRec = TMath::Pi()/2. - lyambda;
      //Double_t err_lyambda = fRes->GetDLambda();
           //cout<<"err_lyambda = "<<err_lyambda<<" sqrt(err_lyambda)"<<TMath::Sqrt(err_lyambda)<<endl;
      //Double_t phi = fRes->GetPhi();
      //Double_t err_phi = fRes->GetDPhi();
      //TVector3 MomRec = fRes->GetMomentum();
      //Double_t CovRec[15];
      //fRes->GetCov(CovRec);
      //TVector3 PosRec = fRes->GetPosition();
      //hThetaeffR->Fill(PosRec.Mag(),thetaRec);
     
      //hchi2->Fill(chi2);
      //hchi2R->Fill(PosRec.Mag(),chi2);
      //hPointOUT->Fill(PosRec.Mag());
      //hPointIN->Fill(TMath::Hypot(StartPos.X(),StartPos.Y()));
      //Double_t errMomRec = TMath::Sqrt(CovRec[0])*MomRec.Mag2();
      //      Double_t ErrPcalc = TMath::Sqrt(errPx*errPx+errPy*errPy+errPz*errPz);
      //Double_t errPx = fRes->GetDPx();
     // Double_t errPy = fRes->GetDPy();
      //Double_t errPz = fRes->GetDPz();
      //Double_t errMomRec = TMath::Sqrt(errPx*errPx+errPy*errPy+errPz*errPz);
      //hErrMomX->Fill(errPx);
      //hErrMomY->Fill(errPy);
      //hErrMomZ->Fill(errPz);
      //  cout<<"errPx = "<<errPx<<" errPy = "<<errPy<<" errPz = "<<errPz<<endl;
     // Double_t errX = fRes->GetDX();
     // Double_t errY = fRes->GetDY();
     // Double_t errZ = fRes->GetDZ();
      //hErrPointX->Fill(errX);
      //hErrPointY->Fill(errY);
      //hErrPointZ->Fill(errZ);
     
      hTheta->Fill(thetaRec*1000.);


    }//geane tracks
    //----------------------------------------------------------------------------------

  }//events

  gStyle->SetOptFit(111);

  TCanvas *cmom = new TCanvas("cmom");
  cmom->Divide(1,1);
  cmom->cd(1);
  hTheta->Draw();

  // -----   DaFit   -------------------------------------------------------

  PndLmdLumiFit dafit(mom, 47.2178116, -0.0857419446, 11.6186924, 0.3);
  dafit->LumiFit(hTheta);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
