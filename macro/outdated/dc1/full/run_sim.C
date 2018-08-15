int run_sim(Int_t nEvents=100, Double_t momentum=2.0, UInt_t seed=0){

gRandom->SetSeed(seed);

TStopwatch timer;
timer.Start();
gDebug=0;
// Load basic libraries
gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
basiclibs();

// Load this example libraries
gSystem->Load("libDpmEvtGen.so");
gSystem->Load("libGeoBase");
gSystem->Load("libParBase");
gSystem->Load("libBase");
gSystem->Load("libPndData");
gSystem->Load("libField");
gSystem->Load("libPassive");
gSystem->Load("libPGen");  
gSystem->Load("libEmc"); 
gSystem->Load("libTof"); 
gSystem->Load("libMdt"); 
gSystem->Load("libgenfit");
gSystem->Load("libtpc"); 
gSystem->Load("libtpcreco");
gSystem->Load("libtrackrep");
gSystem->Load("librecotasks");
gSystem->Load("libMvd");
gSystem->Load("libMvdReco");
gSystem->Load("libLHETrack");
gSystem->Load("libDch"); 
gSystem->Load("libDrcProp");
gSystem->Load("libDrc");

 
FairRunSim *fRun = new FairRunSim();
  
// set the MC version used
// ------------------------

fRun->SetName("TGeant3");
// Choose the Geant Navigation System
// fRun->SetGeoModel("G3Native");
  
fRun->SetOutputFile("fullmc.root");

// Set Material file Name
//-----------------------

fRun->SetMaterials("media_pnd.geo");
  
// Create and add detectors
//-------------------------

FairModule *Cave= new PndCave("CAVE");
Cave->SetGeometryFileName("pndcave.geo");
fRun->AddModule(Cave); 
  
FairModule *Magnet= new PndMagnet("MAGNET");
Magnet->SetGeometryFileName("magnet.geo");
fRun->AddModule(Magnet);

FairModule *Pipe= new PndPipe("PIPE");
Pipe->SetGeometryFileName("pipebeamtarget.geo");
fRun->AddModule(Pipe);

FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
Tpc->SetGeometryFileName("tpc.geo");
fRun->AddModule(Tpc);

FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
Mvd->SetGeometryFileName("MVD14.root");
fRun->AddModule(Mvd);
  
PndEmc *Emc = new PndEmc("EMC",kTRUE);
Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root");
fRun->AddModule(Emc);

FairDetector *Tof = new PndTof("TOF",kTRUE);
Tof->SetGeometryFileName("tofSciF.geo");
fRun->AddModule(Tof);
  
FairDetector *Muo = new PndMuo("MUO",kTRUE);
Muo->SetGeometryFileName("muon_newmag.geo"); 
fRun->AddModule(Muo);
  
FairDetector *Drc = new PndDrc("DIRC", kTRUE);
Drc->SetGeometryFileName("dirc.geo"); 
fRun->AddModule(Drc); 
 
FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
Dch->SetGeometryFileName("dch.root"); 
fRun->AddModule(Dch);
  
// Create and Set Event Generator
//-------------------------------

FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
fRun->SetGenerator(primGen);

/***
*   PndDpmDirect(Momenta GeV/c, Mode)
*/
PndDpmDirect *Dpm= new PndDpmDirect(momentum,1);
primGen->AddGenerator(Dpm);

  
PndMultiField *fField= new PndMultiField();

PndTransMap *map= new PndTransMap("TransMap", "R");
PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
fField->AddField(map);
fField->AddField(map1);
fField->AddField(map2);
  

fRun->SetField(fField);
  
fRun->Init();
// 
//   // Fill the Parameter containers for this run
//   //-------------------------------------------
//      
FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
Bool_t kParameterMerged=kTRUE;
     

PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
if (fField) {  Par->SetParameters(fField); }
Par->setInputVersion(fRun->GetRunId(),1);
Par->setChanged();

FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
output->open("testparams.root");
rtdb->setOutput(output);
rtdb->saveOutput();
rtdb->print();

fRun->Run(nEvents);
   
timer.Stop();
   
Double_t rtime = timer.RealTime();
Double_t ctime = timer.CpuTime();
printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
   
cout << " Test passed" << endl;
cout << " All ok " << endl;
  return 0;
   
}  
  
