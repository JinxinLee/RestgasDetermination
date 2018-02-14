// *******
// Macro for running fast simulation + soft trigger for softtrigger development
// *******

// The parameters are
// -------------------
// prod_fsim.C+( <pref>, <decfile>, <mom>, [nevt], [res], [mode], [run], [full] )\n\n";
//    <pref>     : output file names prefix\n";
//    <decfile>  : decfile; 'DPM' uses DPM generator instead\n";
//    <mom>      : pbar momentum\n";
//    [nevt]     : number of events; default = 1000\n";
//    [res]      : resonance (ignored when running DPM); default = 'pbarpSystem0'\n";
//    [mode]     : mode code; default = 900 (->DPM code)\n";
//    [run]      : run number; should be set if mergeing output files from same mode; default = -1\n";
//    [full]     : apply full selection, defined in config file 'softrig/selection_fsim.cfg'; default = 0\n\n";

// DetOpt    : Parameter string to control detector setup for scrutiny process with some predefined options. For other configurations better use simfast.C
//             Appearance of each parameter is a *positive* switch! Default value "MvdGem EmcBar Drc Dsc FwdSpec" represents complete detector.
//             Detectors always being enabled are: EmcFwd, EmcBw, STT, Barrel MUO,
//             - MvdGem    (or 1) : Enable MVD and GEM for central tracking in addition to STT
//             - EmcBarrel (or 2) : Enable EMC barrel for calorimetry (neutral detection and PID component)
//             - Drc       (or 3) : Enable Barrel DIRC for PID
//             - Dsc       (or 4) : Enable Disc DIRC for PID
//             - FwdSpec   (or 5) : Enable complete Forward Spectrometer (= Fwd Spec. EMC, Fwd Tracking, RICH, Fwd MUO)



int prod_fsim(TString Prefix="", TString Decfile="", Float_t Mom=0., Int_t nEvents = 1000, TString Resonance="pbarpSystem0", int mode = 900, int run = -1, int applyfull=0 )
{
	if (Prefix=="" || Decfile=="" || Mom==0.) 
	{
		cout << "USAGE:\n";
		cout << "prod_fsim.C+( <pref>, <decfile>, <mom>, [nevt], [res], [mode], [run], [full] )\n\n";
		cout << "   <pref>     : output file names prefix\n";
		cout << "   <decfile>  : decfile; 'DPM'/'FTF' uses DPM/FTF generator instead\n";
		cout << "   <mom>      : pbar momentum; negative values are interpreted as -E_cm\n";
		cout << "   [nevt]     : number of events; default = 1000\n";
		cout << "   [res]      : resonance (ignored when running DPM); default = 'pbarpSystem0'\n";
		cout << "   [mode]     : mode code; default = 900 (->DPM code)\n";
		cout << "   [run]      : run number; should be set if mergeing output files from same mode; default = -1\n";
		cout << "   [full]     : apply full selection, defined in config file 'softrig/selection_fsim.cfg'; default = 0\n\n";
		return;
	}
	
	// if Mom<0, interprete as -E_cm
	double mp = 0.938272;
	
	// if mom<0, it's -E_cm -> compute mom
	if (Mom<0)
	{
		double X = (Mom*Mom-2*mp*mp)/(2*mp);
		Mom = sqrt(X*X-mp*mp);
	}
	
	
	// Allow shortcut for resonance
	if (Resonance=="pbp")  Resonance = "pbarpSystem";
	if (Resonance=="pbp0") Resonance = "pbarpSystem0";
	
	// Prevent generator from throwing a lot of warnings
	TLorentzVector fIni(0,0,Mom,0.938272+sqrt(Mom*Mom+0.938272*0.938272));
	TDatabasePDG::Instance()->AddParticle("pbarpSystem","pbarpSystem",fIni.M(),kFALSE,0.1,0, "",88888);
	TDatabasePDG::Instance()->AddParticle("pbarpSystem0","pbarpSystem0",fIni.M(),kFALSE,0.1,0, "",88880);
	TDatabasePDG::Instance()->AddParticle("Z(3900)+","Z+",3.900,kFALSE,0.03,0, "",90000);
	TDatabasePDG::Instance()->AddParticle("Z(3900)-","Z-",3.900,kFALSE,0.03,0, "",-90000);
	
	//-----Evaluate Detector Setup ---------------------------------------
	bool SwMvdGem  = true;  // Enable MVD and GEM for central tracking in addition to STT
	bool SwEmcBar  = true;  // Enable EMC barrel for calorimetry (neutral detection and PID component)
	bool SwDrc     = true;  // Enable Barrel DIRC for PID
	bool SwDsc     = true;  // Enable Disc DIRC for PID
	bool SwFwdSpec = true;  // Enable complete Forward Spectrometer (= Fwd Spec. EMC, Fwd Tracking, RICH, Fwd MUO)
	
// 	TString DetOpt="MvdGem EmcBar Drc Dsc FwdSpec"
// 	if (DetOpt.Contains("MvdGem")  || DetOpt.Contains("1") ) SwMvdGem  = true;
// 	if (DetOpt.Contains("EmcBar")  || DetOpt.Contains("2") ) SwEmcBar  = true;
// 	if (DetOpt.Contains("Drc")     || DetOpt.Contains("3") ) SwDrc     = true;
// 	if (DetOpt.Contains("Dsc")     || DetOpt.Contains("4") ) SwDsc     = true;
// 	if (DetOpt.Contains("FwdSpec") || DetOpt.Contains("5") ) SwFwdSpec = true;

	//----- Switches for Simulation Options ------------------------------
	Bool_t enableSplitoff    = true;  // create e.-m. and hadronic split offs
	Bool_t mergeNeutrals     = true;  // merge neutrals (for merged pi0s)
	Bool_t electronBrems     = true;  // bremsstrahlung loss for electrons 
	Bool_t useEventFilter    = false;  // enable Fast Sim event filter. *** Needs configuration (see below) *** 
	Bool_t usePndEventFilter = false;  // enable Panda event filter. *** Needs configuration (see below) *** 
	
	//----- Presist simulation output ------------------------------
	Bool_t persist           = false;  // we just want to keep SoftTrigger Output

	//-----General settings-----------------------------------------------
	TString BaseDir =  gSystem->Getenv("VMCWORKDIR");
	TString splitpars = BaseDir+"/fsim/splitpars.dat";
	gRandom->SetSeed();

	//-----User Settings:-------------------------------------------------
	TString  OutputFile     = Prefix+"_fsim.root";
	gDebug             = 0;

	// choose your event generator
	Bool_t UseEvtGenDirect  = kTRUE;
	Bool_t UseFtf           = kFALSE;
	Bool_t UseDpm           = kFALSE;
	Bool_t UseBoxGenerator  = kFALSE;

	// use DPM generator; default: inelastic @ pbarmom = mom
	if (Decfile.BeginsWith("DPM"))
	{
		UseEvtGenDirect = kFALSE;
		UseDpm 	      = kTRUE;
	}

	// use FTF generator; 
	if (Decfile.BeginsWith("FTF"))
	{
		UseEvtGenDirect = kFALSE;
		UseFtf 	        = kTRUE;
	}

	// use BOX generator; default: single mu-, 0<tht<180, 0<phi<360, 0.1<p<mom
	if (Decfile=="BOX")
	{
		UseEvtGenDirect = kFALSE;
		UseBoxGenerator = kTRUE;
	}

	usePndEventFilter=UseDpm;

	Double_t MomMin  = 0.1;  // minimum momentum for box generator
	Double_t MomMax  = Mom;  // maximum   "       "

	// Start a stop watch
	TStopwatch timer;
	timer.Start();

	// Create the Simulation run manager
	// --------------------------------
	FairRunSim *fRun = new FairRunSim();
	fRun->SetOutputFile(OutputFile.Data());
	fRun->SetGenerateRunInfo(kFALSE);
	fRun->SetUserConfig(BaseDir+"/macro/softrig/g3ConfigNoMC.C"); // this prevents storing the MCTracks array

	FairLogger::GetLogger()->SetLogToFile(kFALSE);


	// Create and Set Event Generator
	// -------------------------------
	FairFilteredPrimaryGenerator* primGen = new FairFilteredPrimaryGenerator();
	//FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	fRun->SetGenerator(primGen);
	fRun->SetName("TGeant3");

	if(UseBoxGenerator)
	{  // Box Generator
		FairBoxGenerator* boxGen = new FairBoxGenerator(Pdgcode, 1); // 211 = pion; 1 = multipl.
		boxGen->SetPRange(MomMin,MomMax); // GeV/c
		boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
		boxGen->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
		boxGen->SetXYZ(0., 0., 0.); //cm
		primGen->AddGenerator(boxGen);
	}
	if(UseDpm)
	{
		int dpmmode = 0;
		if (Decfile=="DPM1") dpmmode = 1;
		if (Decfile=="DPM2") dpmmode = 2;
		
		PndDpmDirect *Dpm= new PndDpmDirect(Mom,dpmmode);  // 0 = inelastic, 1 = inelastic & elastic, 2 = elastic
		Dpm->SetUnstable(111);   // pi0
		Dpm->SetUnstable(310);   // K_S0
		Dpm->SetStable(3122);  // Lambda
		Dpm->SetStable(-3122); // anti-Lambda
		Dpm->SetStable(3222);  // Sigma
		Dpm->SetStable(-3222); // anti-Sigma
		Dpm->SetUnstable(221);   // eta
		primGen->AddGenerator(Dpm);
	}
	if(UseFtf)
	{
		bool noelastic = true;
		if (Decfile=="FTF1") noelastic=false;
		PndFtfDirect *Ftf = new PndFtfDirect("anti_proton", "G4_H", 1, "ftfp", Mom, 0, noelastic); 
		primGen->AddGenerator(Ftf);
	}
	if(UseEvtGenDirect)
	{
		PndEvtGenDirect *EvtGen = new PndEvtGenDirect(Resonance, Decfile.Data(), Mom);
		EvtGen->SetStoreTree(kTRUE);
		primGen->AddGenerator(EvtGen);
	}

	// ------------- switch off the transport of particles
	primGen->DoTracking(kFALSE);

	//---------------------Create and Set the Field(s)----------
	PndMultiField *fField= new PndMultiField("AUTO");
	fRun->SetField(fField);
	
	
	// ********************************
	//  Setup the Fast Simulation Task
	// ********************************

	PndFastSim* fastSim = new PndFastSim(persist);
		
	// increasing verbosity increases the amount of console output (mainly for debugging)
	fastSim->SetVerbosity(0);

	//-----------------------------
	// set PANDA event filters
	//-----------------------------
	if (mode==900) usePndEventFilter=true; // switch on filter for background events
	if (usePndEventFilter)
	{
		primGen->SetFilterMaxTries(100000); // for testing small number, for real produrction set usually to 9999999 or something very big
		FairEvtFilterOnSingleParticleCounts* lambfilt= new FairEvtFilterOnSingleParticleCounts("PdgFilter");
		// new FairEvtFilterOnSingleParticleCounts named "PdgFilter"
		lambfilt->AndMaxPdgCodes(0, 3122, -3122);  // filter out Lambda0
		lambfilt->AndMaxPdgCodes(0, 3222, -3222);  // filter out Sigma+
		primGen->AndFilter(lambfilt);		
	}
	
	//-----------------------------
	// set event filters
	//-----------------------------
	if (useEventFilter)
	{
	      // Filters are:
	      // -----------
	      // fastSim->SetMultFilter(type, min, max); 
	      // requires min <= mult <= max
	      
	      // available types are:
	      
	      //  "+"   : positive charged particles
	      //  "-"   : negative charged particles
	      //  "gam" : gammas
	      //  "pi0" : pi0 candidates ( -> 2 gammas); mass window 0.135 +- 0.03 GeV
	      //  "eta" : eta candidates ( -> 2 gammas); mass window 0.547 +- 0.04 GeV 
	      //  "ks"  : K_S candidates ( -> pi+ pi-);  mass window 0.497 +- 0.04 GeV
	      
// 	      fastSim->SetMultFilter("+",   2,1000);  // at least 2 trk+
// 	      fastSim->SetMultFilter("-",   2,1000);  // at least 2 trk-
// 	      fastSim->SetMultFilter("gam", 0,   4);  // at most 4 gammas

	      // fastSim->SetInvMassFilter(comb, m_min, m_max, mult);
	      
	      // requires at least mult combined candidates with m_min < m < m_max
	      
	      // comb is a TString describing the combinatoric
	      // - particle codes are: e+ e- mu+ mu- pi+ pi- k+ k- p+ p- gam pi0 ks eta
	      // - codes must be separated with a single blank
	      // - for charged final states only the mass is set; no pdg code selection is done! 
	      // - optional a 'cc' added at the end of also takes into account charge conjugation
	      
	      // Examples: 
	      // - ("k+ k-", 0.98, 1.1, 2)       : forms K+ K- candidate and requires >=2 in the given window
	      // - ("ks k+ pi- cc", 2.8, 3.2,1 ) : forms ks k+ pi- / ks k- pi+ cands and req. at least one in window
	      
	      fastSim->SetInvMassFilter("e+ e-",2.8,3.3,1);  // look for J/psi -> e+ e- candidate
	}

	// enable the merging of neutrals if they have similar direction
	//-----------------------------
	fastSim->MergeNeutralClusters(mergeNeutrals);

	// enable bremsstahlung loss for electrons
	//-----------------------------
	fastSim->EnableElectronBremsstrahlung(electronBrems);

	//enable the producting of parametrized neutral (hadronic) split offs
	// generate electro-magnetic / hadronic split offs in the EMC? switch off when running w/o EMC

	if (enableSplitoff)
		fastSim->EnableSplitoffs(splitpars.Data());

	fastSim->SetUseFlatCov(true);
	// -----------------------------------------------------------------------------------
	//Tracking: Set up in parts of theta coverage. All modelled by PndFsmSimpleTracker.
	// Mind: Numbers on resolution (pRes,thtRes,phiRes) and efficiency are guessed
	// -----------------------------------------------------------------------------------
	if (SwMvdGem) // MVD and GEM are enabled; combined tracking available
	{
		// - (Full Panda Tracking: STT MVD GEM FTS)

		fastSim->AddDetector("ScSttAlone",  "thtMin=145.  thtMax=159.5 ptmin=0.1 pmin=0.0 pRes=0.04 thtRes=0.006 phiRes=0.007 efficiency=0.25");
		fastSim->AddDetector("ScSttMvd",    "thtMin=20.9  thtMax=145.  ptmin=0.1 pmin=0.0 pRes=0.02 thtRes=0.001 phiRes=0.001 efficiency=0.85");
		fastSim->AddDetector("ScSttMvdGem", "thtMin=7.8   thtMax=20.9  ptmin=0.1 pmin=0.0 pRes=0.02 thtRes=0.001 phiRes=0.001 efficiency=0.85");
		fastSim->AddDetector("ScMvdGem",    "thtMin=5.    thtMax=7.8   ptmin=0.1 pmin=0.0 pRes=0.03 thtRes=0.001 phiRes=0.001 efficiency=0.60");
	}
	else // MVD and GEM are disabled; only STT tracking in central region
	{
		// - STT alone:
		fastSim->AddDetector("ScSttAlone",  "thtMin=133.6 thtMax=159.5 ptmin=0.1 pmin=0.0 pRes=0.04 thtRes=0.006 phiRes=0.007 efficiency=0.25");
		fastSim->AddDetector("ScSttAlone2", "thtMin=20.9  thtMax=133.6 ptmin=0.1 pmin=0.0 pRes=0.04 thtRes=0.006 phiRes=0.007 efficiency=0.80");
		fastSim->AddDetector("ScSttAlone3", "thtMin=7.8   thtMax=20.9  ptmin=0.1 pmin=0.0 pRes=0.04 thtRes=0.006 phiRes=0.007 efficiency=0.25");
	}

	if (SwFwdSpec) // Fwd spectrometer enabled -> use Fwd tracking system
	{
		fastSim->AddDetector("ScFts",       "thtMin=0.    thtMax=5.    ptmin=0.0 pmin=0.5 pRes=0.05  thtRes=0.002 phiRes=0.002 efficiency=0.80");
	}

	// -----------------------------------------------------------------------------------
	// Vertexing
	// -----------------------------------------------------------------------------------
	if (SwMvdGem) // MVD and GEM are enabled -> better vertexing in central region
	{
		fastSim->AddDetector("ScVtxMvd",   "thtMin=5. thtMax=145. ptmin=0.1 vtxRes=0.005 efficiency=1."); // efficiency=1: all tracks found in trackers will get a vertex information
		fastSim->AddDetector("ScVtxNoMvd", "thtMin=0. thtMax=5.   ptmin=0.0 vtxRes=0.05  efficiency=1."); // efficiency=1: all tracks found in trackers will get a vertex information
	}
	else // MVD and GEM are disabled -> no good vertexing at all
	{
		fastSim->AddDetector("ScVtxNoMvd", "thtMin=0. thtMax=160. ptmin=0.1 vtxRes=0.1 efficiency=1."); // efficiency=1: all tracks found in trackers will get a vertex information
	}
	// -----------------------------------------------------------------------------------
	// EM Calorimeters w/ default parameters
	// (don't have to be set, just to list the available parameters
	// -----------------------------------------------------------------------------------

	fastSim->AddDetector("EmcFwCap", "thtMin=10.0 thtMax=22.0 Emin=0.01 dist=2.5");
	fastSim->AddDetector("EmcBwCap", "thtMin=142.0 thtMax=160.0 Emin=0.01 dist=0.7");

	if (SwEmcBar)
	{
		// EmcBarrel also allows to set phiMin and phiMax and can be added multiple times as EmcBarrel1, EmcBarrel2, etc.
		// Should be made constistent with EmcPidBarrel below
		fastSim->AddDetector("EmcBarrel","thtMin=22.0 thtMax=142.0 Emin=0.01 barrelRadius=0.5");
	}

	if (SwFwdSpec) // Fwd spectrometer enabled -> use Fwd EMC
	{
		fastSim->AddDetector("EmcFS",    "thtMin=0.05 thtMax=10.0 aPar=0.013 bPar=0.0283 Emin=0.01 dist=8.2");
	}

	// -----------------------------------------------------------------------------------
	// PID
	// -----------------------------------------------------------------------------------

	// PID detectors being always in: STT, MUO Barrel, EMC FwdCap, EMC BwdCap
	//Note: A dEdX parametrization from 2008
	fastSim->AddDetector("SttPid","thtMin=7.8 thtMax=159.5 ptmin=0.1 dEdxRes=0.15 efficiency=1.");
	fastSim->AddDetector("ScMdtPidBarrel", "thtMin=10.0 thtMax=130.0 pmin=0.5 efficiency=0.95 misId=0.01");
	fastSim->AddDetector("ScEmcPidFwCap",  "thtMin=10.0  thtMax=22.0  ptmin=0.0 pmin=0.0 efficiency=1.0");
	fastSim->AddDetector("ScEmcPidBwCap",  "thtMin=142.0 thtMax=160.0  ptmin=0.0 pmin=0.0 efficiency=1.0");

	if (SwMvdGem) // MVD and GEM are enabled -> MVD PID available
	{
		//Note: A Bethe-Bloch-Landau-Gauss Prametrization from 2008
		fastSim->AddDetector("MvdPid","thtMin=5.  thtMax=133.6 ptmin=0.1  dEdxResMulti=1. efficiency=1.");
	}

	if (SwEmcBar) // EMC Barrel enable -> EMC barrel PID available
	{
		fastSim->AddDetector("ScEmcPidBarrel", "thtMin=22.0  thtMax=142.0 ptmin=0.2 pmin=0.0 efficiency=1.0");
	}

	if (SwDrc) // Barrel DIRC enabled
	{
		fastSim->AddDetector("DrcBarrel","thtMin=22.0 thtMax=140.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
	}

	if (SwDsc) // Disc DIRC enabled
	{
		fastSim->AddDetector("DrcDisc","thtMin=5.0 thtMax=22.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
	}

	if (SwFwdSpec) // Fwd spectrometer enabled -> use RICH, FwdMUO and EMC FS
	{
		fastSim->AddDetector("ScEmcPidFS",     "thtMin=0.5   thtMax=10.0  ptmin=0.0 pmin=0.5 efficiency=1.0");
		fastSim->AddDetector("Rich","angleXMax=5.0 angleYMax=10.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
		fastSim->AddDetector("ScMdtPidForward","thtMin=0.0  thtMax=10.0  pmin=0.5 efficiency=0.95 misId=0.01");
	}


	fRun->AddTask(fastSim);
	
	// ***********************
	// *** SoftTriggerTask ***
	// ***********************
	
	// mode code: 
	//  -> signals should have 9-digit code (e.g. 450110002; 450 = 4.5 GeV, 110 = triggerline, 002 = recoil type in decfile)
	//  -> DPM has 6-digits (550900; 550 = 5.5 GeV, 900 = mode code for DPM)
	int modeshort = mode;
	if (modeshort>1000000) modeshort /= 1000;
	if (modeshort>1000)    modeshort %= 1000;
	
	cout <<" ****** mode:"<<mode<<"  modeshort:"<<modeshort<<endl;

	PndSoftTriggerTask *stTask = new PndSoftTriggerTask(Mom, mode, run, BaseDir+"/softrig/triggerlines_fsim.cfg");
	stTask->SetFastSimDefaults();	
	stTask->ApplyFullSelection(applyfull);  // apply selection defined in 'TString selectioncfg'
	
	if (modeshort==900)	            // for DPM/FTF events
	{
		stTask->SetQAAll(true);		// -> write all QA tuples
	}
	else                            // for signal mode
	{
		stTask->SetQAAll(false);
		stTask->SetQAMode(modeshort); // -> only write signal n-tuple
		stTask->SetQAMctOnly();       // -> only keep signals with mct match in QA tuple
	}
		
	if (applyfull) stTask->SetQAAll(false); // in full selection mode don't create QA output
	
	stTask->SetQAEvent(true);		// event info
	
	fRun->AddTask(stTask);

	// ***********************
	// *** SoftTriggerTask ***
	// ***********************
	
	//-------------------------  Initialize the RUN  -----------------
	fRun->Init();
	
	//-------------------------  Run the Simulation  -----------------
	fRun->Run(nEvents);
	
	//-------------------------  Write Filter Info to File -----------
	if (usePndEventFilter) primGen->WriteEvtFilterStatsToRootFile(); 
	
	//------------------------Print some info and exit----------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}

