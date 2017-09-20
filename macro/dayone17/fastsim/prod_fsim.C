// Macro for running fast simulation
///
// to print usage information:
// root -l -b -q prod_fsim.C
//
// to run with specific setting:
// root -l -b -q 'prod_fsim.C("EvtD0D0b", 100, "D0toKpi.dec:pbarpSystem0", 12.)'
// root -l -b -q 'prod_fsim.C("DpmInel",  100, "DPM",                      12.)'
// root -l -b -q 'prod_fsim.C("Box1Kp",   100, "BOX:type(321,1):p(0.1,10):tht(22,140):phi(0,360)",1.)'

void    getRange(TString par, double &min, double &max);
TString getInitialResonance(TString &fEvtGenFile);

int prod_fsim(TString prefix="", Int_t nEvents = 100, TString inputGen="", Float_t pbeam = 0. , TString simopt="")
{
	if (prefix=="" || inputGen=="" || pbeam==0.) 
	{
		cout << "USAGE:\n";
		cout << "prod_fsim.C( <pref>,  <nevt>, <gen>, <pbeam>, <simopt> )\n\n";
		cout << "   <pref>     : output file names prefix\n";
		cout << "   <nevt>     : number of events\n";
		cout << "   <gen>      : generator input: EvtGen decfile; DPM/FTF/BOX uses DPM/FTF generator (inelastic mode) or BOX generator instead\n";
		cout << "                DPM settings: DPM  = inelastic only,  DPM1 = inel. + elastic, DPM2 = elastic only\n";
		cout << "                FTF settings: FTF  = inel. + elastic, FTF1 = inelastic only\n";
		cout << "                BOX settings: type[pdgcode,mult] and optional ranges 'p/tht/phi[min,max]' separated with colon; example: 'BOX:type[211,1]:p[1,5]:tht[45]:phi[90,210]'\n";    
		cout << "   <pbeam>    : pbar momentum (for BOX generator it still controls the magnetic field) \n";
		cout << "   <simopt>   : option like e.g. \"SetupB:Filter2:EMC[22-120]\"\n\n";
		cout << "Example 1 : root -l -b -q 'prod_fsim.C(\"EvtD0D0b\", 100, \"D0toKpi.dec:pbarpSystem0\", 12.)'\n";
		cout << "Example 2 : root -l -b -q 'prod_fsim.C(\"DpmInel\",  100, \"DPM\", 12.)'\n";
		cout << "Example 3 : root -l -b -q 'prod_fsim.C(\"SingleK\",  100, \"BOX:type[321,1]:p[0.1,10]:tht[22,140]:phi[0,360]\", 12.)'\n\n";
		
		return 0;
	}
	
	// ----- BEGIN EVALUATE simopt string -----------
	// set the detector day1 setup (further reduction of phase 1 setup)
	int setupmode = 0;                          // Setup A: Full with 75% GEM, 35% FTS, no EMC Barrel Xtals, no TOF Fwd, no MVD Pixels
	if (simopt.Contains("SetupB")) setupmode=1; // Setup B: Full with 75% GEM, 35% FTS, no EMC Barrel Xtals, no TOF Fwd, no MVD Pixels, no Muon, no Shashlyk
	
	simopt.ReplaceAll("SetupA",""); simopt.ReplaceAll("SetupB",""); 
	
	// event filter
	int filtermode = -1;
	if (simopt.Contains("Filter")) filtermode = ((TString)simopt(simopt.Index("Filter")+6,1)).Atoi();
	
	// EMC barrel tht range
	simopt.ReplaceAll("[","(");simopt.ReplaceAll("]",")");
	TRegexp remctht("EMC([0-9]+-[0-9]+)");
	double thtmin = 22;
	double thtmax = 142;
	TString opttht = simopt(remctht);
	
	if (opttht!="")
	{
		thtmin = ((TString) opttht(opttht.Index("(")+1,opttht.Index("-")-opttht.Index("(")-1)).Atof();
		thtmax = ((TString) opttht(opttht.Index("-")+1,opttht.Index(")")-opttht.Index("-")-1)).Atof();
	}

	cout <<"------------------------------"<<endl;	
	cout <<"Setup      : "<<(setupmode==0?"A":"B")<<endl;
	cout <<"Filter     : "<<filtermode<<endl;
	cout <<"EMC Barrel : "<<thtmin<<" < tht < "<<thtmax<<endl;
	cout <<"------------------------------"<<endl;	

	// ----- END EVALUATE simopt string -----------

	// persist fast sim output?
	bool persist = true;
		
	// if pbeam<0, interprete as -E_cm
	double mp = 0.938272;
	if (pbeam<0)
	{
		double X = (pbeam*pbeam-2*mp*mp)/(2*mp);
		pbeam = sqrt(X*X-mp*mp);
	}
	double Ecm = sqrt(pow(sqrt(pbeam*pbeam + mp*mp) + mp,2) - pbeam*pbeam);
	
	
	// Prevent generator from throwing a lot of warnings
	TLorentzVector fIni(0,0,pbeam,mp+sqrt(pbeam*pbeam+mp*mp));
	TDatabasePDG *pdg = TDatabasePDG::Instance();
	pdg->AddParticle("pbarpSystem","pbarpSystem",fIni.M(),kFALSE,0.1,0, "",88888,0);
	pdg->AddParticle("pbarpSystem0","pbarpSystem0",fIni.M(),kFALSE,0.1,0, "",88880,0);
	pdg->AddParticle("eta_c1","eta_c1",4.3,kFALSE,0.02,0,"",999441);

	
	//----- Switches for Simulation Options ------------------------------
	Bool_t enableSplitoff    = true;   // create e.-m. and hadronic split offs
	Bool_t mergeNeutrals     = true;   // merge neutrals (for merged pi0s)
	Bool_t electronBrems     = true;   // bremsstrahlung loss for electrons
	
	//----- Switches for Event Filter Options ------------------------------
	Bool_t usePndEventFilter = (filtermode >= 0);  // enable Panda event filter.    *** Needs configuration (see below) *** 

	
	//-----General settings-----------------------------------------------
	TString BaseDir =  gSystem->Getenv("VMCWORKDIR");
	TString splitpars = BaseDir+"/fsim/splitpars.dat";
	gRandom->SetSeed();

	//-----User Settings:-------------------------------------------------
	TString  OutputFile     = prefix+"_fsim.root";
	gDebug                  = 0;

	
	// Start a stop watch
	TStopwatch fTimer;
	fTimer.Start();

	// --------------------------------
	// Create the Simulation run manager
	// --------------------------------
	FairRunSim *fRun = new FairRunSim();
	fRun->SetOutputFile(OutputFile.Data());
	fRun->SetGenerateRunInfo(kFALSE);
	//fRun->SetUserConfig(BaseDir+"/macro/prod/g3ConfigNoMC.C");

	FairLogger::GetLogger()->SetLogToFile(kFALSE);


	// -------------------------------
	// Create and Set Event Generator
	// -------------------------------
	FairFilteredPrimaryGenerator* primGen = new FairFilteredPrimaryGenerator();
	if (!usePndEventFilter) primGen->SetVerbose(0);
	fRun->SetGenerator(primGen);
	fRun->SetName("TGeant3");

	
	// ------------------------------------------------------
	// Determine event generator according to inputGen
	// ------------------------------------------------------
	
	// ------------------------------------------------------
	// use DPM generator; default: inelastic @ pbarmom = mom
	// ------------------------------------------------------
	if (inputGen.BeginsWith("DPM",TString::kIgnoreCase) && !inputGen.EndsWith(".dec",TString::kIgnoreCase))
	{
		inputGen.ToLower();
		int mode = 0;
		if (inputGen=="dpm1") mode = 1;
		if (inputGen=="dpm2") mode = 2;
		
		PndDpmDirect *Dpm= new PndDpmDirect(pbeam,mode);  // 0 = inelastic, 1 = inelastic & elastic, 2 = elastic
		
		// since fastsim doesn't have a transport, let all long-living resonances decay by the generator
		Dpm->SetUnstable(111);   // pi0
		Dpm->SetUnstable(310);   // K_S0
		Dpm->SetUnstable(311);   // K0
		Dpm->SetUnstable(-311);  // K0bar
		Dpm->SetUnstable(3122);  // Lambda0
		Dpm->SetUnstable(-3122); // anti-Lambda0
		Dpm->SetUnstable(221);   // eta*/
		Dpm->SetUnstable(3222);  // Sigma+
		Dpm->SetUnstable(-3222); // anti-Sigma-
		Dpm->SetUnstable(3334);  // Omega-
		primGen->AddGenerator(Dpm);
	}

	// ------------------------------------------------------
	// use FTF generator; 
	// ------------------------------------------------------
	else if (inputGen.BeginsWith("FTF",TString::kIgnoreCase) && !inputGen.EndsWith(".dec",TString::kIgnoreCase))
	{
		inputGen.ToLower();
		PndFtfDirect *Ftf = new PndFtfDirect("anti_proton", "G4_H", 1, "ftfp", pbeam, 0, (inputGen=="ftf1") ); 
		primGen->AddGenerator(Ftf);
	}

	// ------------------------------------------------------
	// use BOX generator; defaults
	// ------------------------------------------------------
	else if (inputGen.BeginsWith("BOX",TString::kIgnoreCase) && !inputGen.EndsWith(".dec",TString::kIgnoreCase))
	{
		// Set Box generator defaults
		Double_t BoxMomMin  = 0.05;   // minimum momentum for box generator
		Double_t BoxMomMax  = pbeam;  // maximum   "       "
		Double_t BoxThtMin  = 0. ;    // minimum theta for box generator
		Double_t BoxThtMax  = 180.;   // maximum   "       "
		Double_t BoxPhiMin  = 0. ;    // minimum phi for box generator
		Double_t BoxPhiMax  = 360.;   // maximum   "       "
		Bool_t   BoxCosTht  = false;  // isotropic in cos(theta) instead theta
		
		Int_t    BoxType    = 13;     // default particle muon
		Int_t    BoxMult    = 1;      // default particle multiplicity
		Double_t type=0,mult=0;       // ref. parameters for range function	
		
		inputGen.ToLower();
		
		// Parse configuratio string
		if (inputGen!="box")
		{
			inputGen.ReplaceAll("box","");
			inputGen.ReplaceAll(" ","");
			inputGen += ":";
			
			while (inputGen.Contains(":"))
			{
				TString curpar = inputGen(0,inputGen.Index(":"));
				inputGen = inputGen(inputGen.Index(":")+1,1000);
				curpar.ReplaceAll("[","("); curpar.ReplaceAll("]",")"); 
				
				if (curpar.BeginsWith("type(")) {getRange(curpar,type,mult); BoxType = (Int_t)type; BoxMult = (Int_t)mult; }
				if (curpar.BeginsWith("p("))     getRange(curpar,BoxMomMin,BoxMomMax);
				if (curpar.BeginsWith("tht("))   getRange(curpar,BoxThtMin,BoxThtMax);
				if (curpar.BeginsWith("ctht(")) {getRange(curpar,BoxThtMin,BoxThtMax); BoxCosTht=true;}
				if (curpar.BeginsWith("phi("))   getRange(curpar,BoxPhiMin,BoxPhiMax);
			}
		}
		
		cout <<"BOX generator range: type["<<BoxType<<","<<BoxMult<<"]  p["<<BoxMomMin<<","<<BoxMomMax
			<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]"<<endl;

		
		PndBoxGenerator* boxGen = new PndBoxGenerator(BoxType, BoxMult);
		boxGen->SetDebug(0);
		
		boxGen->SetPRange(BoxMomMin,BoxMomMax);      // GeV/c
		boxGen->SetPhiRange(BoxPhiMin, BoxPhiMax);   // Azimuth angle range [degree]
		boxGen->SetThetaRange(BoxThtMin, BoxThtMax); // Polar angle in lab system range [degree]
		
		if (BoxCosTht) boxGen->SetCosTheta();
		
		boxGen->SetXYZ(0., 0., 0.); //cm
		primGen->AddGenerator(boxGen);
	}

	// ------------------------------------------------------
	// EvtGen Generator
	// ------------------------------------------------------
	else 
	{
		TString Resonance = getInitialResonance(inputGen);
		Resonance.ReplaceAll("pbp","pbarpSystem");

		PndEvtGenDirect *EvtGen = new PndEvtGenDirect(Resonance, inputGen.Data(), pbeam);
		EvtGen->SetStoreTree(kTRUE);
		primGen->AddGenerator(EvtGen);
	}

	// ------------- switch off the transport of particles
	primGen->DoTracking(kFALSE);

	//---------------------Create and Set the Field(s)----------
	PndMultiField *fField= new PndMultiField("AUTO");
	fRun->SetField(fField);

	// Setup the Fast Simulation Task
	//-----------------------------
	PndFastSim* fastSim = new PndFastSim(persist);
		
	// increasing verbosity increases the amount of console output (mainly for debugging)
	fastSim->SetVerbosity(0);

	
	//-----------------------------
	// set PANDA event filters
	//-----------------------------
	
	if (usePndEventFilter)
	{
		// *** Example Configuration of Event Filter 
		cout <<"Using FairEventFilter"<<endl;
		primGen->SetFilterMaxTries(100000);
		
		switch (filtermode)
		{
		case 0: // ******* J/psi -> e+ e- filter
			{
				// require 4 charged tracks
				FairEvtFilterOnSingleParticleCounts* chrgFilter = new FairEvtFilterOnSingleParticleCounts("chrgFilter");
				chrgFilter->AndMinCharge(4, FairEvtFilter::kCharged);
				primGen->AndFilter(chrgFilter);

				// require 1 ee combination in the mass range 2.6 < m(ee) < 3.5 GeV
				PndEvtFilterOnInvMassCounts* eeInv= new PndEvtFilterOnInvMassCounts("eeInvMFilter");
				eeInv->SetPdgCodesToCombine( 11, -11);
				eeInv->SetMinMaxInvMass( 2.6, 3.6 );
				eeInv->SetMinMaxCounts(1,10000);
 				primGen->AndFilter(eeInv);  //add filter to fFilterList	
			}
			break;
			
		case 1: // ******* J/psi -> mu+ mu- filter
			{
				// require 4 charged tracks
				FairEvtFilterOnSingleParticleCounts* chrgFilter = new FairEvtFilterOnSingleParticleCounts("chrgFilter");
				chrgFilter->AndMinCharge(4, FairEvtFilter::kCharged);
				primGen->AndFilter(chrgFilter);

				// require 1 ee combination in the mass range 2.7 < m(ee) < 3.5 GeV
				PndEvtFilterOnInvMassCounts* mmInv= new PndEvtFilterOnInvMassCounts("mmInvMFilter");
				mmInv->SetPdgCodesToCombine( 13, -13);
				mmInv->SetMinMaxInvMass( 2.6, 3.6 );
				mmInv->SetMinMaxCounts(1,10000);
 				primGen->AndFilter(mmInv);  //add filter to fFilterList	
			}
			break;
			
		case 2: // ******* phi -> K+ K- filter (two cand)
			{
				// require 4 charged tracks
				FairEvtFilterOnSingleParticleCounts* chrgFilter = new FairEvtFilterOnSingleParticleCounts("chrgFilter");
				chrgFilter->AndMinCharge(4, FairEvtFilter::kCharged);
				primGen->AndFilter(chrgFilter);

				// require 2 KK combination in the mass range 0.9 < m(KK) < 1.3 GeV
				PndEvtFilterOnInvMassCounts* kkInv= new PndEvtFilterOnInvMassCounts("kkInvMFilter");
				kkInv->SetPdgCodesToCombine( 321, -321);
				kkInv->SetMinMaxInvMass( 0.9, 1.2 );
				kkInv->SetMinMaxCounts(2,10000);
 				primGen->AndFilter(kkInv);  //add filter to fFilterList	
			}
			break;
			
		case 3: // ******* pbar p -> e+ e- filter 
			{
				// require ee combination in the mass range |m - Ecm| < 0.5
				PndEvtFilterOnInvMassCounts* eeInv= new PndEvtFilterOnInvMassCounts("eeInvMFilter");
				eeInv->SetPdgCodesToCombine( 11, -11);
				eeInv->SetMinMaxInvMass( Ecm-0.5, Ecm+0.5 );
				eeInv->SetMinMaxCounts(1,10000);
 				primGen->AndFilter(eeInv);  //add filter to fFilterList	
			}
			break;
			
		case 4: // ******* pbar p -> eta_c1 eta, eta_c1->chi_c 2pi0, chi_c -> J/psi gamma, => J/psi -> e+ e- filter 
			{
				// require 1 ee combination in the mass range 2.6 < m(ee) < 3.5 GeV
				PndEvtFilterOnInvMassCounts* eeInv= new PndEvtFilterOnInvMassCounts("eeInvMFilter");
				eeInv->SetPdgCodesToCombine( 11, -11);
				eeInv->SetMinMaxInvMass( 2.6, 3.6);
				eeInv->SetMinMaxCounts(1,10000);
 				primGen->AndFilter(eeInv);  //add filter to fFilterList					
			}
			break;
		}
 	}
	
	// enable the merging of neutrals if they have similar direction
	//-----------------------------
	fastSim->MergeNeutralClusters(mergeNeutrals);

	// enable bremsstahlung loss for electrons
	//-----------------------------
	fastSim->EnableElectronBremsstrahlung(electronBrems);

	//enable the producting of parametrized neutral (hadronic) split offs
	// generate electro-magnetic / hadronic split offs in the EMC? switch off when running w/o EMC
	if (enableSplitoff) fastSim->EnableSplitoffs(splitpars.Data());

	fastSim->SetUseFlatCov(true);



	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------
	// *********            BEGIN Fast Simulation Configuration                   ********
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------

	
	// Tracking: Set up in parts of theta coverage. All modelled by PndFsmSimpleTracker.
	// Mind: Numbers on resolution (pRes,thtRes,phiRes) and efficiency are guessed
	// -----------------------------------------------------------------------------------
	// - (Full Panda Tracking: STT MVD GEM FTS)

	// **** full tracking
// 	fastSim->AddDetector("ScFts",       "thtMin=0.    thtMax=5.    ptmin=0.0 pmin=0.5 pRes=0.05 thtRes=0.002 phiRes=0.002 efficiency=0.80");

// 	fastSim->AddDetector("ScMvdGem",    "thtMin=5.    thtMax=7.8   ptmin=0.1 pmin=0.0 pRes=0.03 thtRes=0.001 phiRes=0.001 efficiency=0.60");
// 	fastSim->AddDetector("ScSttMvdGem", "thtMin=7.8   thtMax=20.9  ptmin=0.1 pmin=0.0 pRes=0.02 thtRes=0.001 phiRes=0.001 efficiency=0.85");
// 	fastSim->AddDetector("ScSttMvd",    "thtMin=20.9  thtMax=145.  ptmin=0.1 pmin=0.0 pRes=0.02 thtRes=0.001 phiRes=0.001 efficiency=0.85");
// 	fastSim->AddDetector("ScSttAlone",  "thtMin=145.  thtMax=159.5 ptmin=0.1 pmin=0.0 pRes=0.04 thtRes=0.006 phiRes=0.007 efficiency=0.25");

	// **** reduced tracking
	fastSim->AddDetector("ScMvdGem",    "thtMin=5.    thtMax=7.8   ptmin=0.1 pmin=0.0 pRes=0.04  thtRes=0.003 phiRes=0.003 efficiency=0.55");
	fastSim->AddDetector("ScSttMvdGem", "thtMin=7.8   thtMax=20.9  ptmin=0.1 pmin=0.0 pRes=0.025 thtRes=0.002 phiRes=0.002 efficiency=0.82");
	fastSim->AddDetector("ScSttMvd",    "thtMin=20.9  thtMax=145.  ptmin=0.1 pmin=0.0 pRes=0.03  thtRes=0.003 phiRes=0.003 efficiency=0.80");
	fastSim->AddDetector("ScSttAlone",  "thtMin=145.  thtMax=159.5 ptmin=0.1 pmin=0.0 pRes=0.05  thtRes=0.006 phiRes=0.007 efficiency=0.25");

	// -----------------------------------------------------------------------------------
	// Vertexing
	// -----------------------------------------------------------------------------------
	// **** full vertexing
// 	fastSim->AddDetector("ScVtxMvd",   "thtMin=5. thtMax=145. ptmin=0.1 vtxRes=0.005 efficiency=1."); // efficiency=1: all tracks found in trackers will get a vertex information
// 	fastSim->AddDetector("ScVtxNoMvd", "thtMin=0. thtMax=5.   ptmin=0.0 vtxRes=0.05  efficiency=1."); // efficiency=1: all tracks found in trackers will get a vertex information
	
	// **** reduced vertexing
	fastSim->AddDetector("ScVtxNoMvd", "thtMin=5. thtMax=145. ptmin=0.1 vtxRes=0.02 efficiency=1."); // efficiency=1: all tracks found in trackers will get a vertex information

	// -----------------------------------------------------------------------------------
	// EM Calorimeters w/ default parameters
	// -----------------------------------------------------------------------------------
	// (don't have to be set, just to list the available parameters
	// -----------------------------------------------------------------------------------

	fastSim->AddDetector("EmcFwCap", "thtMin=10.0  thtMax=22.0  Emin=0.01 dist=2.5");
	fastSim->AddDetector("EmcBwCap", "thtMin=142.0 thtMax=160.0 Emin=0.01 dist=0.7");
	if (setupmode==0) 
		fastSim->AddDetector("EmcFS",    "thtMin=0.05  thtMax=10.0  aPar=0.013 bPar=0.0283 Emin=0.01 dist=8.2");

	// configure polar angle range of EMC
	if (thtmin<thtmax)
		fastSim->AddDetector("EmcBarrel", Form("thtMin=%.1f  thtMax=%.1f Emin=0.01 barrelRadius=0.5", thtmin, thtmax));


	// -----------------------------------------------------------------------------------
	// PID
	// -----------------------------------------------------------------------------------

	// Cherenkovs
	fastSim->AddDetector("DrcBarrel","thtMin=22.0 thtMax=140.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
	//fastSim->AddDetector("DrcDisc","thtMin=5.0 thtMax=22.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
	//fastSim->AddDetector("Rich","angleXMax=5.0 angleYMax=10.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
	
	// TOF
	fastSim->AddDetector("Tof","thtMin=22.0 thtMax=140.0 dp=0.01");	

	// Trackers with dE/dx
	//Note: A dEdX parametrization from 2008
	fastSim->AddDetector("SttPid","thtMin=7.8 thtMax=159.5 ptmin=0.1 dEdxRes=0.15 efficiency=1.");
	//Note: A Bethe-Bloch-Landau-Gauss Prametrization from 2008
	fastSim->AddDetector("MvdPid","thtMin=5.  thtMax=133.6 ptmin=0.1  dEdxResMulti=1. efficiency=1.");

	// Muon counters
	if (setupmode==0)
	{
		fastSim->AddDetector("ScMdtPidBarrel", "thtMin=10.0 thtMax=130.0 pmin=0.5 efficiency=0.95 misId=0.01");
		fastSim->AddDetector("ScMdtPidForward","thtMin=0.0  thtMax=10.0  pmin=0.5 efficiency=0.95 misId=0.01");
	}
	
	// EMCs
	fastSim->AddDetector("ScEmcPidFwCap",  "thtMin=10.0  thtMax=22.0  ptmin=0.0 pmin=0.0 efficiency=1.0");
	fastSim->AddDetector("ScEmcPidBwCap",  "thtMin=142.0 thtMax=160.0  ptmin=0.0 pmin=0.0 efficiency=1.0");
	
	if (setupmode==0) 
			fastSim->AddDetector("ScEmcPidFS",     "thtMin=0.5   thtMax=10.0  ptmin=0.0 pmin=0.5 efficiency=1.0");

	// configure polar angle range of EMC
	if (thtmin<thtmax)
		fastSim->AddDetector("ScEmcPidBarrel", Form("thtMin=%.1f  thtMax=%.1f ptmin=0.2 pmin=0.0 efficiency=1.0", thtmin, thtmax));

	
	// -----------------------------------------------------------------------------------
	// *********              END Fast Simulation Configuration                   ********
	// -----------------------------------------------------------------------------------



	fRun->AddTask(fastSim);	
	
	//-------------------------  Initialize the RUN  -----------------
	fRun->Init();
	
	//-------------------------  Run the Simulation  -----------------
	fRun->Run(nEvents);
	
	//-------------------------  Write Filter Info to File -----------
	if (usePndEventFilter) primGen->WriteEvtFilterStatsToRootFile(); 
	
	//------------------------Print some info and exit----------------
	fTimer.Stop();
	FairSystemInfo sysInfo;
	Float_t maxMemory=sysInfo.GetMaxMemory();
	Double_t rtime = fTimer.RealTime();
	Double_t ctime = fTimer.CpuTime();
	
	Float_t cpuUsage=ctime/rtime;
	
	cout << endl;
	cout << "[INFO   ] Macro call       : prod_fsim.C(\""<<prefix<<"\", "<<nEvents<<", \""<<inputGen<<"\", "<<pbeam<<")" <<endl;
	cout << "[INFO   ] Generated Events : " <<primGen->GetNumberOfGeneratedEvents()<<endl;
	cout << "[INFO   ] Output file      : "    << OutputFile << endl;
	cout << "[INFO   ] Real time        : " << rtime << " s, CPU time " << ctime << "s" << endl;
	cout << "[INFO   ] CPU usage        : " << cpuUsage*100. << "%" << endl;
	cout << "[INFO   ] Max Memory       : " << maxMemory << " MB" << endl;
	cout << "[INFO   ] Macro finished successfully." << endl<<endl;
  return 0;
}

// ------ Helper functions -------

void getRange(TString par, double &min, double &max)
{
	par.ReplaceAll(" ","");
	par = par(par.Index("(")+1, par.Length()-par.Index("(")-2);
	
	TString smin=par, smax=par;
	
	if (par.Contains(",")) 
	{
		smin = par(0,par.Index(","));
		smax = par(par.Index(",")+1,1000);
	}
	
	min = smin.Atof();
	max = smax.Atof();
	
	//if (min>max) {double tmp=min;min=max;max=tmp;}
}

TString getInitialResonance(TString &fEvtGenFile)
{

	TString IniRes="";

	if (fEvtGenFile.Contains(":")) // is the initial resonance provide as <decfile>.dec:iniRes ? 
	{
		IniRes = fEvtGenFile(fEvtGenFile.Index(":")+1,1000);
		fEvtGenFile = fEvtGenFile(0,fEvtGenFile.Index(":"));
	}

	if (IniRes=="") // we need to search the decay file
	{
		std::ifstream fs(fEvtGenFile.Data());	
		char line[250];

		while (fs)
		{
			fs.getline(line,249);
			TString s(line);
			s.ReplaceAll("\r","");
			if (IniRes=="" && s.Contains("Decay "))
			{
				if (s.Contains("#")) s=s(0,s.Index("#"));
				s.ReplaceAll("Decay ","");
				s.ReplaceAll(" ","");
				IniRes = s;
			}	 
		} 
		fs.close();
	}

	return IniRes;
}
