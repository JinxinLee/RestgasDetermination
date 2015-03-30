///  Method="Follow" - Track-following method, Method="CA" - Cellular Automaton
///  missPl=true - use "missing plane" algorithm
void runLumiPixel3Finder(const int nEvents=1000, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=5,  TString Method="Follow", const bool missPl=true, const bool mergedHits=true, const bool trkcuts=true, double mom=1.5)
{
  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"/Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  TString RecoMergedFile = storePath+"/Lumi_recoMerged_";
  RecoMergedFile += startEvent;
  RecoMergedFile += ".root";
  TString RecoFile = storePath+"/Lumi_reco_";
  RecoFile += startEvent;
  RecoFile += ".root";
  // Parameter file
  TString parFile = storePath+"/Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";

  // // ----  Load libraries   -------------------------------------------------
  // gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // gSystem->Load("libSds");
  // gSystem->Load("libSdsReco");
  // gSystem->Load("libLmd");
  // gSystem->Load("libLmdReco");
  // gSystem->Load("libLmdTrk");
  // // ------------------------------------------------------------------------
  // Output file
    TString outFile = storePath+"/Lumi_TCand_";
    outFile += startEvent;
    outFile += ".root";
    
    std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
    std::cout << "TCandFile: " << outFile.Data()<< std::endl;
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
  if(mergedHits) fRun->AddFriend(RecoMergedFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);


  // =========================================================================
  // ======                       Track Finder                          ======
  // =========================================================================
  
  // -----  LMD collections names & importain parameters --------------------------------------------
  TString inHits = "LMDHitsPixel";
  TString inClusters = "LMDPixelClusterCand";
  TString inDigis = "LMDPixelDigis";
  int nsensors = 100;
  int nplanes = 8;
  double accurF = 0.01;//parameter for trk-finder corridor
  double accurCA;//parameter for CA neigboring search (breaking angle)
  // //set accurCA for diff Pbeam cases ---------------------------------------------
  double accCAv[5]={5e-6, 1e-6, 9e-7, 8e-7, 8e-7}; //1.5, 4.06, 8.9, 11.91, 15
  switch (mom) {
  case 1.5:
    accurCA = accCAv[0];
    break;
  case 4.06:
    accurCA = accCAv[1];
    break;
  case 8.9:
    accurCA = accCAv[2];
    break;
  case 11.91:
    accurCA = accCAv[3];
    break;
  case 15:
    accurCA = accCAv[4];
    break;
  default:
    accurCA = 1e-5;
    cout<<"! CA algorithm needs to know beam momentum for choose of proper cut !";
    break;
  }
  // //------------------------------------------------------------------------------------------
  if(trkcuts!=true) accurCA = 1e-3;//misalignment sensors case (TODO: study it in multiple trks case)
  if(mergedHits){
    inHits = "LMDHitsMerged";
    nplanes = 4;
  }

  if(Method=="Follow") {
    PndLmdTrackFinderTask* lmdfinder = new PndLmdTrackFinderTask(missPl,inHits,inClusters,inDigis, nsensors);
    lmdfinder->SetInaccuracy(accurF);
    lmdfinder->SetSensPixelFlag(true);

  }else{
    if(Method=="CA"){
      cout<<"CA is going to use cut="<<accurCA<<endl;
      PndLmdTrackFinderCATask* lmdfinder = new PndLmdTrackFinderCATask(missPl,accurCA,nsensors,nplanes,inHits,inClusters,inDigis); //for merged hits
      lmdfinder->SetSensPixelFlag(true);
      lmdfinder->SetTrkCandCutsFlag(trkcuts);//value=false for misaligned sensors only
      //  lmdfinder->SetTrkCandCutsFlag(true);
    }
    else{
      cout<<"Method "<<Method.Data()<<" doesn't exist!"<<endl;
      break;
    }
  }
  lmdfinder->SetVerbose(verboseLevel);
  fRun->AddTask(lmdfinder);

  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of TrackFinding                           =====
  // =========================================================================

  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);
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


}
