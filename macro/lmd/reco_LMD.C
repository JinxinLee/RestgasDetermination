// reco macro for lmd
// hit reco
// hit merge
// track search with Trk-Following
// track fit with "broken-lines" method
// track filter (erase double track-candidates, aply X&Y cuts)
// back-propagation with GEANE
// created 23/07/2015 by A.Karavdina

void reco_LMD(const int nEvents=10, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, const double mom=1.5)
{
  // parameters ================================================================
  bool misalign=false; //no misalignment is expected: important for hit reco and track search
  bool missPl=true;  // switch on "missing plane" in track reconstruction
  bool mergedHits=true;
  int nplanes = 4;
  int nsensors = 100;
  double radlen=0.31;
  TString inHits = "LMDHitsMerged";//"LMDHitsPixel";
  TString inClusters = "LMDPixelClusterCand";
  TString inDigis = "LMDPixelDigis";
  TString inTrks="LMDTrackCand";
  //=========================================================================


  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"/Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  // Parameter file
  TString parFile = storePath+"/Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";

  // Output file
  TString outFile = storePath + "/Lumi_Geane_";
  outFile += startEvent;
  outFile += ".root";

    
    std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << outFile.Data()<< std::endl;
 
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  // fRun->SetInputFile(DigiFile);
  // fRun->AddFriend(MCFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  // alignment parameter file
  //TString alignparFile = "LMD_alignment/lumi.align_pixel_ideal.par";//perfect
TString alignparFile = "../params/lumi.align_pixel_ideal.par";//perfect
 
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(alignparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

   // ------------------------------------------------------------------------
  
  // -----           LMD hit reco         --------------------------------------------
 
  PndLmdPixelClusterTask* lmdhitrec = new PndLmdPixelClusterTask();
  lmdhitrec->SetVerbose(verboseLevel);
  //  lmdhitrec->SetMSflag(useMSerr);//switch on\off error calculation due to multiple scaterring
  lmdhitrec->SetAlignFlag(misalign);
   if(misalign) lmdhitrec->SetMtxPath(storePath);
  fRun->AddTask(lmdhitrec);

  PndLmdHitMergeTask* lmdhitmg = new PndLmdHitMergeTask();
  lmdhitmg->SetVerbose(verboseLevel);
  lmdhitmg->SetAlignFlag(misalign);
  if(misalign) lmdhitmg->SetMtxPath(storePath);
  fRun->AddTask(lmdhitmg);

  // =====                 End of HitProducers                           =======

  // -----         LMD  trk reco -------------------------------------------------

  // ======                       Track Finder                          ======
    PndLmdTrackFinderTask* lmdfinder = new PndLmdTrackFinderTask(missPl,inHits,inClusters,inDigis, nsensors);
    lmdfinder->SetInaccuracy(1e-2);
    lmdfinder->SetSensPixelFlag(true);
    lmdfinder->SetVerbose(verboseLevel);
    fRun->AddTask(lmdfinder);
    // =========================================================================

    // ======                       Track Fitter                         ======
    PndLmdLinFitTask* lmdfit = new PndLmdLinFitTask(inTrks,inHits);
    lmdfit->SetRadLen(1e-2*radlen);
    lmdfit->SetVerbose(verboseLevel);
    fRun->AddTask(lmdfit);
    // =========================================================================

    //=====                        Track Fillter                        ======
    PndLmdTrksFilterTask* lmdfilt = new PndLmdTrksFilterTask();
    lmdfilt->SetSkipKinFilt(false);
    lmdfilt->SetXThFilt(true);
    lmdfilt->SetDX(0.);
    lmdfilt->SetYPhFilt(true);
    lmdfilt->SetDY(0.);
    lmdfilt->SetVerbose(verboseLevel);
    fRun->AddTask(lmdfilt);
    // =========================================================================

    // ======                 Back-Propagating                     ======
    FairGeane *Geane = new FairGeane();
    fRun->AddTask(Geane);
    TVector3 IP(0., 0., 0.);
    PndLmdGeaneTask* lmdgeane = new PndLmdGeaneTask(mom, IP);
    lmdgeane->SetVerbose(verboseLevel);
    fRun->AddTask(lmdgeane);
    // =========================================================================

  rtdb->setOutput(parInput1);
  rtdb->print();

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
