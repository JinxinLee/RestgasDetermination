/*
 * runLumiSimBox.cxx
 *
 *  Created on: Nov 28, 2012
 *      Author: jasinski
 */



// Panda FullSim macro

/*
#include<TString.h>
#include<TStopwatch.h>
#include<TROOT.h>
#include<FairModule.h>
#include<PndCave.h>
#include<PndMagnet.h>
#include<PndPipe.h>
#include<PndMvdDetector.h>
#include<FairRunSim.h>
#include<PndLmdDetector.h>
#include<FairPrimaryGenerator.h>
#include<FairBoxGenerator.h>
#include<PndMultiField.h>
#include<PndTransMap.h>
#include<PndDipoleMap.h>
#include<PndSolenoidMap.h>
#include<FairParRootFileIo.h>
#include<PndMultiFieldPar.h>
// important for compiled version ****************
#include<PndSensorNameContFact.h>
#include<FairBaseContFact.h>
#include<PndFieldContFact.h>
#include <PndPassiveContFact.h>
#include <TSystem.h>
// ******************important for compiled version
*/
//#include<.h>

int nEvents = 1000000; // number of primary events
double mom = 15; // beam momentum
double mom_spread = 1e-4; // HESR momentum resolution (relative to mom)
double dx = 0.; // displacement of the primary vertex in x
double dy = 0.; // displacement of the primary vertex in y
double phi_low = 0;
double phi_high = 360.;
double theta_low = 2.5e-3/3.141 *180.; //2.5 mrad
double theta_high = 10.e-3/3.141 *180.; //10 mrad
TString storePath="."; //tmpOutput";
const int particle=-2212; // particle id (-2212 = antiproton)

//static TROOT* pROOT = gROOT;

//void runLumi0SimBox(const int nEvents=10, const double mom=15, TString storePath="tmpOutput", const int verboseLevel=0, const int particle=-211)
void runGeaneTestSimBox()
{

//	cout << nEvents << " " << mom << " " << dx << " " << dy << " " << endl;
  const int verboseLevel=0;//7;
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  //output1
  TString simOutput=storePath+"/Lumi_MC.root";
  TString parOutput=storePath+"/Lumi_Params.root";

  //Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libSds");
  gSystem->Load("libLmd");
  FairRunSim *fRun = new FairRunSim();
  std::cout<<"All libraries succsesfully loaded!"<<std::endl;

  //set the MC version used
  //fRun->SetName("TGeant4");
  fRun->SetName("TGeant3");

  fRun->SetOutputFile(simOutput);
  std::cout << "$VMCWORKDIR is " << getenv("VMCWORKDIR") << " ." << std::endl;

  /*
  // ************ important when running standalone *********************
  // initialization of singleton classes
  PndSensorNameContFact& sensornamecontfact = PndSensorNameContFact::gPndSensorNameContFact();
  FairBaseContFact& basecontfact = FairBaseContFact::gFairBaseContFact();
  PndFieldContFact& fieldcontfact = PndFieldContFact::gPndFieldContFact();
  PndPassiveContFact& passivecontfact = PndPassiveContFact::gPndPassiveContFact();
  // ***********************************************************************
*/

  //set material
  fRun->SetMaterials("media_pnd.geo");

  //create and add detectors
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcaveVAC.geo");
  fRun->AddModule(Cave);

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  //FairModule *Pipe= new PndPipe("PIPE");
  //Pipe->SetGeometryFileName("pipebeamtarget.geo");
  //fRun->AddModule(Pipe);

  //FairModule *Pipe= new PndPipe("PIPE");
  //Pipe->SetGeometryFileName("../macro/lmd/geo/beampipe_201210.root");// with lumi beam pipe
  //Pipe->SetGeometryFileName("beampipe_201202_no_lmd_pipe.root");
  //Pipe->SetGeometryFileName("beampipe_201112.root");
  //Pipe->SetGeometryFileName("../macro/lmd/geo/HV_MAPS-Design.root");
  //fRun->AddModule(Pipe);

  /*FairDetector *Stt= new PndStt("STT", kFALSE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);*/

  //FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
  //Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  //fRun->AddModule(Mvd);

 /* PndEmc *Emc = new PndEmc("EMC",kFALSE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); // if you want to use new geometry for FwEndCap
  fRun->AddModule(Emc);

  FairDetector *Tof = new PndTof("TOF",kFALSE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);

  FairDetector *Drc = new PndDrc("DIRC", kFALSE);
  Drc->SetGeometryFileName("dirc.geo");
  fRun->AddModule(Drc);

  PndMdt *Muo = new PndMdt("MDT",kFALSE);
  Muo->SetGeometryFileName("muopars.root");
  Muo->SetMdtVersion("torino");
  fRun->AddModule(Muo);*/



  PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
  Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
  //Lum->SetGeometryFileName("../macro/lmd/geo/F0-Design.root"); //!!!
  Lum->SetGeometryFileName("lmd_geane_test.root"); //!!!
  //Lum->SetGeometryFileName("beampipe_201210_active.root");
  //Lum->SetGeometryFileName("../macro/lmd/geo/beampipe_201205_w_sensors.root"); //!!!

  //  Lum->SetGeometryFileName("../macro/lmd/geo/MyTest-Dipol-Design.root"); //!!!

  //  Mvd->SetGeometryFileName("lumi.geo");
  //   Mvd->SetGeometryFileName("MVD14.root");
  Lum->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Lum);

  //  FairDetector *Stt= new CbmStt("STT", kTRUE);
  //  Stt->SetGeometryFileName("straws_axial.geo");
  //  fRun->AddModule(Stt);

  //  FairDetector *Emc = new CbmEmc("EMC",kTRUE);
  //  Emc->SetGeometryFileName("emc_module12345.dat");
  //  fRun->AddModule(Emc);

  //  FairDetector *Drc = new CbmDrc("DIRC", kTRUE);
  //  Drc->SetGeometryFileName("dirc.geo");
  //  fRun->AddModule(Drc);


  //particle generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Particle Generator (pdgid,mult, px,py,pz, vx,vy,vz)

  // single pions for testing
  //      FairParticleGenerator* partGenX   = new FairParticleGenerator(211,1, 1.,0.,0., 0.,0.,0.);
  //      FairParticleGenerator* partGenY   = new FairParticleGenerator(211,1, 0.,1.,0., 0.,0.,0.);
  //      FairParticleGenerator* partGenZ   = new FairParticleGenerator(211,1, 0.,0.1,1., 0.,0.,0.);
  //      FairParticleGenerator* partGenXYZ = new FairParticleGenerator(211,1, 1.,1.,5., 0.,0.,0.);
  //      primGen->AddGenerator(partGenX);
  //      primGen->AddGenerator(partGenY);
  //      primGen->AddGenerator(partGenZ);
  //      primGen->AddGenerator(partGenXYZ);

  // Ion Generator
  //    FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
  //    primGen->AddGenerator(fIongen);

  // Box Generator
  FairBoxGenerator *fBox = new FairBoxGenerator(particle, 1);
  //FairBoxGenerator *fBox = new FairBoxGenerator(0, 1);
  fBox->SetPRange(mom,mom);//mom-mom*mom_spread,mom+mom*mom_spread); // momentum +- 10e-4
  //fBox->SetThetaRange(0.2,0.4); //3...8 mrad
  //  fBox->SetThetaRange(0.13,0.56);
  // fBox->SetThetaRange(0.13,0.7); // 2... 12 mrad
  // fBox->SetThetaRange(0.3,0.3);
  //  fBox->SetThetaRange(0.,0.7);
  //fBox->SetThetaRange(1e-3/3.141 *180., 10e-3/3.141 *180.);
  //fBox->SetThetaRange(1e-8/3.141 *180., 1e-3/3.141 *180.);
  //fBox->SetThetaRange(4e-3/3.141 *180., 8e-3/3.141 *180.);
  // fBox->SetThetaRange(0., 3.14);
  // fBox->SetThetaRange(0.4, 0.46); //7-8 mrad
  //fBox->SetThetaRange(0.257831, 0.315127); //4.5 ... 5.5 mrad
  //  fBox->SetThetaRange(0.257831, 0.257831); //4.5 mrad
  // fBox->SetThetaRange(0., 90.);
  // fBox->SetPhiRange(0.,360.);
  //fBox->SetThetaRange(0., 0.);
  //fBox->SetPhiRange(89.,91.);
  //  fBox->SetPhiRange(0.,20.);//TEST
  //  fBox->SetPhiRange(44.,46.);
  //fBox->SetPhiRange(-90.,90.);
  //  fBox->SetPhiRange(22.5,22.5);

  fBox->SetThetaRange(theta_low, theta_high);
  fBox->SetPhiRange(phi_low, phi_high);

  fBox->SetXYZ(dx, dy, 0.);

  //fBox->SetThetaRange(0.,0.); //3.5 ... 8.? mrad
  //fBox->SetPhiRange(-90.,-90.);
  //
  //  fBox->SetPhiRange(1*180./TMath::Pi(),2*180./TMath::Pi());
  //fBox->SetPhiRange(80.,100.);
  primGen->AddGenerator(fBox);


  // //EvtGen Generator
  // PndEvtGenDirect *EvtGen = new PndEvtGenDirect("pbarpSystem","PBARSYSTEMTO4PIPHSP.DEC",mom);
  // primGen->AddGenerator(EvtGen);


  // Urqmd  Generator
  //    FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
  //    primGen->AddGenerator(urqmdGen);

  // DPM Generator
  //  PndDpmGenerator* dpmGen = new PndDpmGenerator("DpmInput/el_100k_aida/el_6_2GeV.root");
  //  primGen->AddGenerator(dpmGen);


  //reading the new field map in the old format
  fRun->SetBeamMom(mom);
  PndMultiField *fField= new PndMultiField();

  //PndTransMap *map_t = new PndTransMap("TransMap_v1", "R");
  //PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1_v1", "R");
  //PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2_v1", "R");
  PndTransMap *map_t = new PndTransMap("TransMap", "R");
  PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R");
  PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R");
  PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
  PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
  PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
  PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");

  fField->AddField(map_t);
  fField->AddField(map_d1);
  fField->AddField(map_d2);
  fField->AddField(map_s1);
  fField->AddField(map_s2);
  fField->AddField(map_s3);
  fField->AddField(map_s4);

  fRun->SetField(fField);

  if(nEvents<101)
    fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
  else
    fRun->SetStoreTraj(kFALSE);


  fRun->Init();

 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

 // Set cuts for storing the trajectpries
//   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
//     trajFilter->SetStepSizeCut(0.01); // 1 cm
//     trajFilter->SetVertexCut(-200., -200., -200, 200., 200., 200.);
//     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
//     trajFilter->SetStorePrimaries(kTRUE);
//     trajFilter->SetStoreSecondaries(kTRUE);


  // Fill the Parameter containers for this run
  //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutput.Data(),"RECREATE");
  rtdb->setOutput(output);
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  // Transport nEvents
  // -----------------

  fRun->Run(nEvents);

  rtdb->saveOutput();
  rtdb->print();


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

#include<TApplication.h>
#include<iostream>

using namespace std;

/* arguments
 * 1: total number of events to simulate
 * 2: energy of anti protons
 * 3: displacement of the vertex in x [cm]
 * 4: displacement of the vertex in y [cm]
 */


int main(int argc, char **argv){
	cout << " running box generator " << endl;
	if (argc > 1){
		nEvents = atoi(argv[1]);
		cout << " generating " << nEvents << " anti protons " << endl;
	}
	if (argc > 2){
		mom = atof(argv[2]);
		cout << " with a momentum of " << mom << " GeV/c " << endl;
	}
	if (argc > 3){
		dx = atof(argv[3]);
		cout << " a displacement of " << dx << " [cm] in x " << endl;
	}
	if (argc > 4){
		dy = atof(argv[4]);
		cout << " a displacement of " << dy << " [cm] in y " << endl;
	}
	//TApplication myapp("runLumi0SimBox",0,0);
	runLumiSimBox();// 8.9);
	//myapp.Run();
	return 0;
}

