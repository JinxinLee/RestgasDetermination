// *******
// Macro for running fast simulation
// *******

// The parameters are
// -------------------
// Prefix    : Prefix string for the output file
// Decfile   : 1) name of the EvtGen decay file;  2) 'DPM' for using DpmGen;  3) 'BOX' for using Box Generator
// Mom       : 1) EvtGen and DpmGen: pbar momentum in GeV/c;  2) BoxGen, Mom > 0: generate 0.1 < p < Mom;  3) BoxGen, Mom < 0: generate only p = -Mom
// nEvents   : Number of events to be generated
// Resonance : Initial resonance name for EvtGen
// Pdgcode   : Only BoxGen: pdgcode of particle to be generated

void simfast(TString Prefix, TString Decfile, Float_t Mom, Int_t nEvents = 1000, TString Resonance="pbarpSystem0", int Pdgcode = 11 )
{
	TString BaseDir =  gSystem->Getenv("VMCWORKDIR");
	TString splitpars = BaseDir+"/fsim/splitpars.dat";
	gRandom->SetSeed();

	//-----User Settings:-----------------------------------------------
	TString  OutputFile     = Prefix+"_fast.root";
	gDebug             = 0;

	// generate electro-magnetic / hadronic split offs in the EMC? switch off when running w/o EMC
	Bool_t enableSplitoff   = kFALSE;

	// choose your event generator
	Bool_t UseEvtGenDirect  = kTRUE;
	Bool_t UseDpm           = kFALSE;
	Bool_t UseBoxGenerator  = kFALSE;

	// use DPM generator; default: inelastic @ pbarmom = mom
	if (Decfile=="DPM")
	{
		UseEvtGenDirect = kFALSE;
		UseDpm 	      = kTRUE;
	}

	// use BOX generator; default: single mu-, 0<tht<180, 0<phi<360, 0.1<p<mom
	if (Decfile=="BOX")
	{
		UseEvtGenDirect = kFALSE;
		UseBoxGenerator = kTRUE;
	}

	Double_t MomMin  = 0.1;  // minimum momentum for box generator
	Double_t MomMax  = Mom;  // maximum   "       "

	// Start a stop watch
	TStopwatch timer;
	timer.Start();

	// Create the Simulation run manager
	// --------------------------------
	FairRunSim *fRun = new FairRunSim();
	fRun->SetOutputFile(OutputFile.Data());
	fRun->SetWriteRunInfoFile(kFALSE);

	FairLogger::GetLogger()->SetLogToFile(kFALSE);


	// Create and Set Event Generator
	// -------------------------------
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	fRun->SetGenerator(primGen);
	fRun->SetName("TGeant3");

	if(UseBoxGenerator){  // Box Generator
		FairBoxGenerator* boxGen = new FairBoxGenerator(Pdgcode, 1); // 211 = pion; 1 = multipl.
		boxGen->SetPRange(MomMin,MomMax); // GeV/c
		boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
		boxGen->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
		boxGen->SetXYZ(0., 0., 0.); //cm
		primGen->AddGenerator(boxGen);
	}
	if(UseDpm){
		PndDpmDirect *Dpm= new PndDpmDirect(Mom,1);
		primGen->AddGenerator(Dpm);
	}
	if(UseEvtGenDirect){
		PndEvtGenDirect *EvtGen = new PndEvtGenDirect(Resonance, Decfile.Data(), Mom);
		EvtGen->SetStoreTree(kTRUE);
		primGen->AddGenerator(EvtGen);
	}

	// ------------- switch off the transport of particles
	primGen->DoTracking(kFALSE);

	//---------------------Create and Set the Field(s)----------
	PndMultiField *fField= new PndMultiField("FULL");
	fRun->SetField(fField);

	//Setup the Fast Simulation Task
	//-----------------------------
	PndFastSim* fastSim = new PndFastSim();

	//increasing verbosity increases the amount of console output (mainly for debugging)
	fastSim->SetVerbosity(0);

	//enable the producting of parametrized neutral (hadronic) split offs
	if (enableSplitoff)
		fastSim->EnableSplitoffs(splitpars.Data());

	// -----------------------------------------------------------------------------------
	//Tracking: Set up in parts of theta coverage. All modelled by PndFsmSimpleTracker.
	// Mind: Numbers on resolution (pRes,thtRes,phiRes) and efficiency are guessed
	// -----------------------------------------------------------------------------------
	// - (Full Panda Tracking: STT MVD GEM FTS)
	fastSim->AddDetector("ScSttAlone",  "thtMin=145.  thtMax=159.5 ptmin=0.1 pmin=0.0 pRes=0.04  thtRes=0.001 phiRes=0.001 efficiency=0.25");
	fastSim->AddDetector("ScSttMvd",    "thtMin=20.9  thtMax=145.  ptmin=0.1 pmin=0.0 pRes=0.016 thtRes=0.001 phiRes=0.001 efficiency=0.95");
	fastSim->AddDetector("ScSttMvdGem", "thtMin=7.8   thtMax=20.9  ptmin=0.1 pmin=0.0 pRes=0.018 thtRes=0.001 phiRes=0.001 efficiency=0.95");
	fastSim->AddDetector("ScMvdGem",    "thtMin=5.    thtMax=7.8   ptmin=0.1 pmin=0.0 pRes=0.03  thtRes=0.001 phiRes=0.001 efficiency=0.80");
	fastSim->AddDetector("ScFts",       "thtMin=0.    thtMax=5.    ptmin=0.0 pmin=0.5 pRes=0.05  thtRes=0.002 phiRes=0.002 efficiency=0.80");
	// - other options:
	//fastSim->AddDetector("ScMvdGemFts", "thtMin=5.    thtMax=7.8   ptmin=0.1 pmin=0.0 pRes=0.03  thtRes=0.001 phiRes=0.001 efficiency=0.80");
	//fastSim->AddDetector("ScMvdFts",    "thtMin=5.    thtMax=10    ptmin=0.0 pmin=0.0 pRes=0.05  thtRes=0.002 phiRes=0.002 efficiency=0.80");
	//fastSim->AddDetector("ScGemFts",    "thtMin=3.    thtMax=10    ptmin=0.0 pmin=0.0 pRes=0.05  thtRes=0.002 phiRes=0.002 efficiency=0.80");
	// - STT alone:
	//fastSim->AddDetector("ScSttAlone",  "thtMin=133.6 thtMax=159.5 ptmin=0.1 pmin=0.0 pRes=0.030 thtRes=0.06 phiRes=0.1 efficiency=0.25");
	//fastSim->AddDetector("ScSttAlone",  "thtMin=20.9  thtMax=133.6 ptmin=0.1 pmin=0.0 pRes=0.026 thtRes=0.06 phiRes=0.1 efficiency=0.95");
	//fastSim->AddDetector("ScSttAlone",  "thtMin=7.8   thtMax=20.9  ptmin=0.1 pmin=0.0 pRes=0.026 thtRes=0.06 phiRes=0.1 efficiency=0.25");

	// -----------------------------------------------------------------------------------
	// Vertexing
	// -----------------------------------------------------------------------------------
	// - A
	fastSim->AddDetector("ScVtxMvd",   "thtMin=5. thtMax=145. ptmin=0.1 vtxRes=0.005 efficiency=1."); // efficiency=1: all tracks found in trackers will get a vertex information
	fastSim->AddDetector("ScVtxNoMvd", "thtMin=0. thtMax=5.   ptmin=0.0 vtxRes=0.05  efficiency=1."); // efficiency=1: all tracks found in trackers will get a vertex information
	// - B
	//fastSim->AddDetector("ScVtxNoMvd", "thtMin=0. thtMax=160. ptmin=0.1 vtxRes=0.1 efficiency=1."); // efficiency=1: all tracks found in trackers will get a vertex information

	// -----------------------------------------------------------------------------------
	// EM Calorimeters w/ default parameters
	// (don't have to be set, just to list the available parameters
	// -----------------------------------------------------------------------------------
	fastSim->AddDetector("EmcBarrel","thtMin=22.0 thtMax=142.0 Emin=0.01 barrelRadius=0.5");
	fastSim->AddDetector("EmcFwCap", "thtMin=5.0 thtMax=22.0 Emin=0.01 dist=2.5");
	fastSim->AddDetector("EmcBwCap", "thtMin=142.0 thtMax=160.0 Emin=0.01 dist=0.7");
	fastSim->AddDetector("EmcFS",    "thtMin=0.05 thtMax=5.0 aPar=0.02 bPar=0.0274 Emin=0.01 dist=8.0");

	// -----------------------------------------------------------------------------------
	// PID
	//
	// -----------------------------------------------------------------------------------
	fastSim->AddDetector("MvdPid","thtMin=5.  thtMax=133.6 ptmin=0.1  dEdxResMulti=1. efficiency=1."); //Note: A Bethe-Bloch-Landau-Gauss Prametrization from 2008
	fastSim->AddDetector("SttPid","thtMin=7.8 thtMax=159.5 ptmin=0.1 dEdxRes=1. efficiency=1."); //Note: A dEdX parametrization from 2008
	
	fastSim->AddDetector("DrcBarrel","thtMin=22.0 thtMax=140.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
	fastSim->AddDetector("DrcDisc","thtMin=5.0 thtMax=22.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
	fastSim->AddDetector("Rich","angleXMax=5.0 angleYMax=10.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
	
	fastSim->AddDetector("ScMdtPidBarrel", "thtMin=10.0 thtMax=130.0 pmin=1.0 efficiency=0.95 misId=0.01");
	fastSim->AddDetector("ScMdtPidForward","thtMin=0.0  thtMax=10.0  pmin=1.5 efficiency=0.95 misId=0.01");
	
	fastSim->AddDetector("ScEmcPidFwCap",  "thtMin=10.0  thtMax=22.0  ptmin=0.0 pmin=0.0 efficiency=1.0");
	fastSim->AddDetector("ScEmcPidBarrel", "thtMin=22.0  thtMax=142.0 ptmin=0.2 pmin=0.0 efficiency=1.0");
	fastSim->AddDetector("ScEmcPidBwCap",  "thtMin=142.0 thtMax=160.0  ptmin=0.0 pmin=0.0 efficiency=1.0");

	fRun->AddTask(fastSim);
	//-------------------------  Initialize the RUN  -----------------
	fRun->Init();
	//-------------------------  Run the Simulation  -----------------
	fRun->Run(nEvents);
	//------------------------Print some info and exit----------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

