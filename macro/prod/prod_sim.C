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

void prod_sim(TString prefix="", Int_t nEvents = 100, TString inputGen="", Float_t pbeam = 0. )
{
  if (prefix=="" || inputGen=="" || pbeam==0.) 
  {
    cout << "USAGE:\n";
    cout << "prod_sim.C( <pref>,  <nevt>, <gen>, <pbeam> )\n\n";
    cout << "   <pref>     : output file names prefix\n";
    cout << "   <nevt>     : number of events\n";
    cout << "   <gen>      : generator input: EvtGen decfile; DPM/FTF/BOX uses DPM/FTF generator (inelastic mode) or BOX generator instead\n";
    cout << "                DPM settings: DPM  = inelastic only,  DPM1 = inel. + elastic, DPM2 = elastic only\n";
    cout << "                FTF settings: FTF  = inel. + elastic, FTF1 = inelastic only\n";
    cout << "                BOX settings: type[pdgcode,mult] and optional ranges 'p/tht/phi[min,max]' separated with colon; example: 'BOX:type[211,1]:p[1,5]:tht[45]:phi[90,210]'\n";    
    cout << "   <pbeam>    : pbar momentum (for BOX generator it still controls the magnetic field) \n\n";
    cout << "Example 1 : root -l -b -q 'prod_sim.C(\"EvtD0D0b\", 100, \"D0toKpi.dec:pbarpSystem0\", 12.)'\n";
    cout << "Example 2 : root -l -b -q 'prod_sim.C(\"DpmInel\",  100, \"DPM\", 12.)'\n";
    cout << "Example 3 : root -l -b -q 'prod_sim.C(\"SingleK\",  100, \"BOX:type[321,1]:p[0.1,10]:tht[22,140]:phi[0,360]\", 12.)'\n\n";
    
    return;
  }

  // set random random seed
  gRandom->SetSeed();
  
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
  TString  SimEngine      = "TGeant3";
  TString  Workdir        = gSystem->Getenv("VMCWORKDIR");
  TString  parAsciiFile   = "all.par";

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
  fRun->SetName(SimEngine);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetNumberOfEvents(nEvents);
  fRun->SetBeamMom(pbeam);
  // -----  Initialization   ------------------------------------------------
  fRun->Setup(prefix);
  // -----   Geometry   -----------------------------------------------------
  fRun->CreateGeometry();
  // -----   Event generator   ----------------------------------------------
  fRun->SetGenerator();

  // -----   Event filter setup   -------------------------------------------
  FairFilteredPrimaryGenerator *primGen = fRun->GetFilteredPrimaryGenerator();
  primGen->SetVerbose(0);
  // ---- Example configuration for the event filter ------------------------
  //FairEvtFilterOnSingleParticleCounts* chrgFilter = new FairEvtFilterOnSingleParticleCounts("chrgFilter");
  //chrgFilter->AndMinCharge(4, FairEvtFilter::kCharged);
  //primGen->AndFilter(chrgFilter);  

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddSimTasks();
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(nEvents); 
  fRun->Finish();
}  
  
