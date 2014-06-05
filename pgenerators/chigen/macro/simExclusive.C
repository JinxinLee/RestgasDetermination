simExclusive(Int_t nEvents = 100, TString SimEngine = "TGeant3", UInt_t seed = 123) {

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
    rootlogon();

    //Loading ChiGen libraries
    gSystem->Load("libEvtGenExternal");
    gSystem->Load("libEvtGen");
    gSystem->Load("libEvtGenDirect");
    gSystem->Load("libCHIGEN");

    //-----User Settings:-----------------------------------------------
    TString OutputFile = "sim_complete.root";
    TString ParOutputfile = "simparams.root";
    TString MediaFile = "media_pnd.geo";
    gDebug = 0;
    TString digiFile = "all.par"; //The emc run the hit producer directly 
    // choose your event generator 
    Bool_t UseEvtGen = kFALSE;
    Bool_t UseEvtGenDirect = kTRUE;
    Bool_t UseDpm = kFALSE;
    Bool_t UseBoxGenerator = kFALSE;

    Double_t BeamMomentum = 5.062; // beam momentum for chi_c1 (5.228 for chi_c2)

    //------------------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    gRandom->SetSeed();

    // Create the Simulation run manager--------------------------------
    FairRunSim *fRun = new FairRunSim();
    fRun->SetName(SimEngine.Data());
    fRun->SetOutputFile(OutputFile.Data());
    fRun->SetWriteRunInfoFile(kFALSE);
    fRun->SetBeamMom(BeamMomentum);
    fRun->SetMaterials(MediaFile.Data());
    FairRuntimeDb *rtdb = fRun->GetRuntimeDb();

    // Set the parameters 
    //-------------------------------
    TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
    allDigiFile += "/macro/params/";
    allDigiFile += digiFile;


    //-------Set the parameter output --------------------
    FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
    parIo1->open(allDigiFile.Data(), "in");
    rtdb->setFirstInput(parIo1);

    //---------------------Set Parameter output      ---------- 
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
    output->open(ParOutputfile.Data());
    rtdb->setOutput(output);

    // Create and add detectors

    //-------------------------  CAVE      -----------------

    FairModule *Cave = new PndCave("CAVE");
    Cave->SetGeometryFileName("pndcave.geo");
    fRun->AddModule(Cave);
    //-------------------------  Magnet   ----------------- 
    FairModule *Magnet = new PndMagnet("MAGNET");
    //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
    Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
    fRun->AddModule(Magnet);
    FairModule *Dipole = new PndMagnet("MAGNET");
    Dipole->SetGeometryFileName("dipole.geo");
    fRun->AddModule(Dipole);
    //-------------------------  Pipe     -----------------
    FairModule *Pipe = new PndPipe("PIPE");
    Pipe->SetGeometryFileName("beampipe_201112.root");
    fRun->AddModule(Pipe);
    //-------------------------  STT       -----------------
    FairDetector *Stt = new PndStt("STT", kTRUE);
    Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
    fRun->AddModule(Stt);
    //-------------------------  MVD       -----------------
    FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
    Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
    fRun->AddModule(Mvd);
    //-------------------------  GEM       -----------------
    FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
    Gem->SetGeometryFileName("gem_3Stations.root");
    fRun->AddModule(Gem);
    //-------------------------  EMC       -----------------
    PndEmc *Emc = new PndEmc("EMC", kTRUE);
    Emc->SetGeometryVersion(1);
    Emc->SetStorageOfData(kFALSE);
    fRun->AddModule(Emc);
    //-------------------------  SCITIL    -----------------
    FairDetector *SciT = new PndSciT("SCIT", kTRUE);
    SciT->SetGeometryFileName("barrel-SciTil_07022013.root");
    fRun->AddModule(SciT);
    //-------------------------  DRC       -----------------
    PndDrc *Drc = new PndDrc("DIRC", kTRUE);
    Drc->SetGeometryFileName("dirc_l0_p0_updated.root");
    Drc->SetRunCherenkov(kFALSE);
    fRun->AddModule(Drc);
    //-------------------------  DISC      -----------------
    PndDsk* Dsk = new PndDsk("DSK", kTRUE);
    Dsk->SetStoreCerenkovs(kFALSE);
    Dsk->SetStoreTrackPoints(kFALSE);
    fRun->AddModule(Dsk);
    //-------------------------  MDT       -----------------
    PndMdt *Muo = new PndMdt("MDT", kTRUE);
    Muo->SetBarrel("fast");
    Muo->SetEndcap("fast");
    Muo->SetMuonFilter("fast");
    Muo->SetForward("fast");
    Muo->SetMdtMagnet(kTRUE);
    Muo->SetMdtMFIron(kTRUE);
    fRun->AddModule(Muo);
    //-------------------------  FTS       -----------------
    FairDetector *Fts = new PndFts("FTS", kTRUE);
    Fts->SetGeometryFileName("fts.geo");
    fRun->AddModule(Fts);
    //-------------------------  FTOF      -----------------
    FairDetector *FTof = new PndFtof("FTOF", kTRUE);
    FTof->SetGeometryFileName("ftofwall.root");
    fRun->AddModule(FTof);
    //-------------------------  RICH       ----------------
    FairDetector *Rich = new PndRich("RICH", kFALSE);
    Rich->SetGeometryFileName("rich_v2.geo");
    fRun->AddModule(Rich);

    // Create and ChiGen generator
    //-------------------------------
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    //uncomment for configuration of ChiGen
    /*
    chigen::initialize(seed);
    chigen::ostreams::suppress_all_cout = false;
    chigen::ostreams::write_log_file = false;
    chigen::ostreams::suppress_pandaroot = false;
    chigen::ostreams::suppress_pandaroot_cout = false;
    chigen::ostreams::verbose_mode = true;
    */
     
    //partonic model for unpolarized chi_{c1}
    chigen::models::chi_1 chi1;
    chigen::PndChiGenExclusive chigen(chi1);
    primGen->AddGenerator(&chigen);



    //---------------------Create and Set the Field(s)---------- 
    PndMultiField *fField = new PndMultiField("AUTO");
    fRun->SetField(fField);

    // EMC Hit producer
    //-------------------------------
    PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
    fRun->AddTask(emcHitProd);

    //-------------------------  Initialize the RUN  -----------------  
    fRun->Init();
    //-------------------------  Run the Simulation  -----------------   
    fRun->Run(nEvents);
    //-------------------------  Save the parameters ----------------- 
    rtdb->saveOutput();
    //------------------------Print some info and exit----------------     
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    cout << " Test passed" << endl;
    cout << " All ok " << endl;

    //exit(0);

} 