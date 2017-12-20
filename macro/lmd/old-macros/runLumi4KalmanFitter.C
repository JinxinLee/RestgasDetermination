int runLumi4KalmanFitter(const int nEvents=100000, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, const int senstype=0, const bool mergedHits=false, TString trkRep="GEANE", double scaleP=1, double scaleM=1)
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
  TString RecoFile = storePath;
  if(senstype>0 && mergedHits) RecoFile +="/Lumi_recoMerged_";
  else RecoFile +="/Lumi_reco_";
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

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libSds");
  gSystem->Load("libSdsReco");
  gSystem->Load("libLmd");
  gSystem->Load("libLmdReco");
  gSystem->Load("libLmdTrk");
  // ------------------------------------------------------------------------
  // Output file
  TString outFile = storePath+"/Lumi_Track_";
  outFile += startEvent;
  outFile += ".root";

  // TString outFile = storePath+"/Lumi_Geane_";
  //   outFile += startEvent;
  //   outFile += ".root";
    
  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
  std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
  std::cout << "TCandFile: " << CandFile.Data()<< std::endl;
  // std::cout << "GFTrackFile: " << gfTrkFile.Data()<< std::endl;
  //  std::cout << "(GFfitted+BackPropagation)TrackFile: " << outFile.Data()<< std::endl;
  std::cout << "TrackFile: " << outFile.Data()<< std::endl;

  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(CandFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
   // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);


  // =========================================================================
  // ======                         Track fitter                        ======
  // =========================================================================
  TString HitNames,TrackNames;
  TrackNames = "LMDTrackCand";
  if(senstype>0){
    if(mergedHits)
      HitNames = "LMDHitsMerged";
    else HitNames = "LMDHitsPixel";
  }
  else HitNames = "LMDHitsStrip";
    
  PndLmdKalmanTask* lmdkalmanfitter = new PndLmdKalmanTask(HitNames, TrackNames);
  lmdkalmanfitter->SetVerbose(verboseLevel);
  lmdkalmanfitter->SetScalePError(scaleP);
  lmdkalmanfitter->SetScaleMError(scaleM);
  if(trkRep=="GEANE"){
    lmdkalmanfitter->SetGeaneTrkRep();
  }
  else{
      if(trkRep=="RK"){
	lmdkalmanfitter->SetRKTrkRep();
      }
      else{
	cout<<"Please, set correct track representation for Kalman Filter task (GEANE or Runge-Kutta). "<<trkRep.Data()<<" is none of them!"<<endl;
      }
  }
  fRun->AddTask(lmdkalmanfitter);

  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of  Track fitter                          =====
  // =========================================================================

  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);
  cout<<"fRun->Run(0, "<<nEvents<<") was done!"<<endl;
  // ------------------------------------------------------------------------

 rtdb->saveOutput();
 rtdb->print();
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


  return 0;
}
