// Macro for running Panda simulation with Geant3 or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// It takes basic parameter as input for flexible use
//
// to print usage information:
// root -l -b -q prod_sim.C
//
// to run with specific setting:
// root -l -b -q 'prod_sim.C("EvtD0D0b", 100, "D0toKpi.dec:pbarpSystem0", 12.)'
// root -l -b -q 'prod_sim.C("DpmInel",  100, "DPM",                      12.)'
// root -l -b -q 'prod_sim.C("Box1Kp",   100, "BOX:type(321,1):p(0.1,10):tht(22,140):phi(0,360)",1.)'

int prod_sim_hvmaps(TString prefix="", Int_t nEvents = 100, TString inputGen="", Float_t pbeam = 0., Bool_t use_mvd_hvmaps = false)
{
  if (prefix=="" || inputGen=="" || pbeam==0.) 
  {
    cout << "USAGE:\n";
    cout << "prod_sim_hvmaps.C( <pref>,  <nevt>, <gen>, <pbeam>, <use_mvd_hvmaps> )\n\n";
    cout << "   <pref>           : output file names prefix\n";
    cout << "   <nevt>           : number of events\n";
    cout << "   <gen>            : generator input: EvtGen decfile; DPM/FTF/BOX uses DPM/FTF generator (inelastic mode) or BOX generator instead\n";
    cout << "                      DPM settings: DPM  = inelastic only,  DPM1 = inel. + elastic, DPM2 = elastic only\n";
    cout << "                      FTF settings: FTF  = inel. + elastic, FTF1 = inelastic only\n";
    cout << "                      BOX settings: type[pdgcode,mult] and optional ranges 'p/tht/phi[min,max]' separated with colon; example: 'BOX:type[211,1]:p[1,5]:tht[45]:phi[90,210]'\n";    
    cout << "   <pbeam>          : pbar momentum (for BOX generator it still controls the magnetic field) \n";
    cout << "   <use_mvd_hvmaps> : boolean (true/false) to select all_hvmaps.par\n\n";
    //    cout << "   <opt>      : option string for PndRunAna (e.g. \"day1\")\n\n";
    cout << "Example 1 : root -l -b -q 'prod_sim_hvmaps.C(\"EvtD0D0b\", 100, \"D0toKpi.dec:pbarpSystem0\", 12., false)'\n";
    cout << "Example 2 : root -l -b -q 'prod_sim_hvmaps.C(\"DpmInel\",  100, \"DPM\", 12., true)'\n";
    cout << "Example 3 : root -l -b -q 'prod_sim_hvmaps.C(\"SingleK\",  100, \"BOX:type[321,1]:p[0.1,10]:tht[22,140]:phi[0,360]\", 12., false)'\n\n";
    
    return 0;
  }

  // set random random seed
  gRandom->SetSeed();

  // runsim option 
  TString opt="PndFiltPrim"; // for PndFilteredPrimaryGenerator the options must contain this key word
  
  double mp = 0.938272;
	
  // if pbeam<0, it's -E_cm -> compute momentum
  if (pbeam<0)
  {
    double X = (pbeam*pbeam-2*mp*mp)/(2*mp);
    pbeam = sqrt(X*X-mp*mp);
  }
  
  // Allow shortcut for resonance
  if (inputGen.Contains(":pbp")) inputGen.ReplaceAll(":pbp",":pbarpSystem");
  
  //-----User Settings:-----------------------------------------------
  TString  SimEngine      = "TGeant4";
  TString  Workdir        = gSystem->Getenv("VMCWORKDIR");
  TString parAsciiFile = use_mvd_hvmaps ? "all_hvmaps.par" : "all.par";

  // ---- check flag for DPM/FTF -------------------------------------
  Int_t    genflag = 0;
  if (inputGen=="DPM1" || inputGen=="FTF1") genflag=1;
  if (inputGen=="DPM2") genflag=2;
  
  //-------------------------------------------------------------------------
  // -----   Create the Simulation run manager ------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
  fRun->SetInput(inputGen);
  fRun->SetDpmFlag(genflag);
  fRun->SetFtfFlag(genflag);
  fRun->SetTargetMode(0);
  fRun->SetName(SimEngine);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetNumberOfEvents(nEvents);
  fRun->SetBeamMom(pbeam);
  //fRun->SetDpmTheta_min(0.2 / 3.1415926 * 180.);
  fRun->SetDpmTheta_min(22.);
  fRun->SetIP(0.2, 0.3, -5.);
  if (opt!="") fRun->SetOptions(opt);

  //fRun->SetStoreTraj(kTRUE);//evtdisplay

  // -----  Initialization   ------------------------------------------------
  fRun->Setup(prefix);
  // -----   Geometry   -----------------------------------------------------
  if (use_mvd_hvmaps)
  {
    FairModule *Cave = new PndCave("CAVE");
    Cave->SetGeometryFileName("pndcave.geo");
    fRun->AddModule(Cave);

    //-------------------------  Magnet   -----------------
    //   // This part is commented because the MDT geometry contains the magnet now
    //      //FairModule *Magnet= new PndMagnet("MAGNET");
    //         //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
    //            //Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
    //               //fRun->AddModule(Magnet);
    FairModule *Dipole= new PndMagnet("MAGNET");
    Dipole->SetGeometryFileName("dipole.geo");
    fRun->AddModule(Dipole);
    //-------------------------  Pipe     -----------------
    FairModule *Pipe= new PndPipe("PIPE");
    Pipe->SetGeometryFileName("beampipe_201309.root");
    fRun->AddModule(Pipe);                                    
    //-------------------------  STT       -----------------
    FairDetector *Stt= new PndStt("STT", kTRUE);
    Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
    fRun->AddModule(Stt);
    //-------------------------  MVD       -----------------
    // FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
    // Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
    // fRun->AddModule(Mvd);
    FairDetector *Mvd = new PndMvdDetector("MVD1", kTRUE);
    Mvd->SetGeometryFileName("MVD_HV_MAPS.root");
    fRun->AddModule(Mvd);

    FairDetector *Mvd_S = new PndMvdDetector("MVD2", kTRUE);
    Mvd_S->SetGeometryFileName("Mvd-2.1-Strips.root");
    fRun->AddModule(Mvd_S);
    //-------------------------  GEM       -----------------
    FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
    Gem->SetGeometryFileName("gem_3Stations_realistic_v2.root");
    fRun->AddModule(Gem);
    //-------------------------  EMC       -----------------
    PndEmc *Emc = new PndEmc("EMC",kTRUE);
    Emc->SetGeometryVersion(1);
    Emc->SetStorageOfData(kFALSE);
    fRun->AddModule(Emc);
    //-------------------------  SCITIL    -----------------
    FairDetector *SciT = new PndSciT("SCIT",kTRUE);
    SciT->SetGeometryFileName("SciTil_201601.root");
    fRun->AddModule(SciT);
    //-------------------------  DRC       -----------------
    PndDrc *Drc = new PndDrc("DIRC", kTRUE);
    Drc->SetGeometryFileName("dirc_e3_b3_l6_m40.root");
    Drc->SetRunCherenkov(kFALSE);
    fRun->AddModule(Drc);
    //-------------------------  DISC      -----------------
    PndDsk* Dsk = new PndDsk("DSK", kTRUE);
    Dsk->SetStoreCerenkovs(kFALSE);
    Dsk->SetStoreTrackPoints(kFALSE);
    fRun->AddModule(Dsk);
    //-------------------------  MDT       -----------------
    PndMdt *Muo = new PndMdt("MDT",kTRUE);
    Muo->SetBarrel("fast");
    Muo->SetEndcap("fast");
    Muo->SetMuonFilter("fast");
    Muo->SetForward("fast");
    Muo->SetMdtMagnet(kTRUE);
    Muo->SetMdtCoil(kTRUE);
    Muo->SetMdtMFIron(kTRUE);
    fRun->AddModule(Muo);

    //-------------------------  FTS       -----------------
    FairDetector *Fts= new PndFts("FTS", kTRUE);
    Fts->SetGeometryFileName("fts.geo");
    fRun->AddModule(Fts);
    //-------------------------  FTOF      -----------------
    FairDetector *FTof = new PndFtof("FTOF",kTRUE);
    FTof->SetGeometryFileName("ftofwall.root");
    fRun->AddModule(FTof);
    //-------------------------  RICH       ----------------
    PndRich *Rich= new PndRich("RICH",kTRUE);
    Rich->SetGeometryFileName("rich_v313.root");
    fRun->AddModule(Rich);
  }
  
  else fRun->CreateGeometry();

  // -----   Event generator   ----------------------------------------------
  fRun->SetGenerator();  // -----   Event filter setup   -------------------------------------------
  // fetch the PndFilteredPrimaryGenerator (only existing if option contains 'PndFiltPrim')
  PndFilteredPrimaryGenerator *primGen = fRun->GetPndFilteredPrimaryGenerator();
  // ---- Example configuration for the event filter ------------------------
  //primGen->SetVerbose(1);
  //primGen->AddFilter("(t+-;4..) && M(e+ e-; m[3.1,0.6])");  //require 4 tracks and at least one e+e- candidate in mass window [2.8,3.4]

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddSimTasks();
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(nEvents); 
  fRun->Finish();
  
  return 0;
}

