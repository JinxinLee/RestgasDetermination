int runLumiPixel7TrksQA(const int nEvents=100000, const int startEvent=0, TString storePath="tmpOutnewDesign", const int verboseLevel=5, const double Plab=15, const bool wrAllMC=true, const bool isClean=false)
{
  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"/Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  TString RecoMergedFile = storePath+"/Lumi_recoMerged_";
  RecoMergedFile += startEvent;
  RecoMergedFile += ".root";
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
  // Geane file
  TString GeaFile;
  if(isClean){
    GeaFile = storePath+"/Lumi_GeaneFiltered_";
    GeaFile += startEvent;
    GeaFile += ".root";
  }
  else{
    GeaFile = storePath+"/Lumi_Geane_";
    GeaFile += startEvent;
    GeaFile += ".root";
  }
  // Output file
  TString DumFile = storePath+"/Lumi_TrksQA_";
  DumFile += startEvent;
  DumFile += ".root";

  // // ----  Load libraries   -------------------------------------------------
//   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
// //-lRooFit –lRooFitCore -lMinuit
 
//   gSystem->Load("libSds");
//   gSystem->Load("libSdsReco");
//   gSystem->Load("libLmd");
//   gSystem->Load("libLmdReco");
//   gSystem->Load("libLmdTrk");
//   gSystem->Load("libLmdTool"); 
//   // ------------------------------------------------------------------------
    
  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
  std::cout << "TCandFile: " << CandFile.Data()<< std::endl;
  std::cout << "TrackFile: " << TrkFile.Data()<< std::endl;
  std::cout << "GeaneFile: " << GeaFile.Data()<< std::endl;


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  FairRunAna *fRun= new FairRunAna();
  FairFileSource input_source(GeaFile);
  input_source.AddFriend(TrkFile);
  input_source.AddFriend(CandFile);
  input_source.AddFriend(RecoFile);
  input_source.AddFriend(MCFile);
  input_source.AddFriend(RecoMergedFile);

  fRun->SetSource(&input_source);
  fRun->SetOutputFile(DumFile);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  PndLmdTrkQTask *lmdqa;
  if(isClean){
    lmdqa = new PndLmdTrkQTask(Plab,"LMDCleanTrack", "LMDPndTrackFilt");//clean reconstructed (with runLumiPixel5bCleanSig.C applied before!)
  }
  else{
    lmdqa = new PndLmdTrkQTask(Plab,"GeaneTrackFinal", "LMDPndTrack");//raw reconstructed
  }
  //PndLmdTrkQTask *lmdqa = new PndLmdTrkQTask(Plab,"GeaneTrackFinal");//raw reconstructed
  //  PndLmdTrkQTask *lmdqa = new PndLmdTrkQTask(Plab,"LMDCleanTrack");//clean reconstructed (with runLumiPixel5bCleanSig.C applied before!)

  lmdqa->SetVerbose(verboseLevel);
  lmdqa->SetWriteMC(wrAllMC);
  fRun->AddTask(lmdqa);
  fRun->SetGenerateRunInfo(kFALSE);
  //rtdb->setOutput(parInput1);
  //  rtdb->print();
  fRun->Init();
  fRun->Run(0,nEvents);
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


	// temporary fix to avoid double frees at the destruction of te program for pandaroot/fairroot with root6
	gGeoManager->GetListOfVolumes()->Delete();
	gGeoManager->GetListOfShapes()->Delete();
	delete gGeoManager;

  return 0;
}
