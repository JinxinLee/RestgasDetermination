/*
 * rumDPMsim.C* *  Created on: Jan 4, 2010 *      Author: huagen
 */
//The macro is to make the simulation run after constructing the detector Geometry, defining
// the physics process, creating the event generator.

void runMCsim()
{ 	TStopwatch timer;
	timer.Start();

    Int_t nEvents = 1e3;
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Double_t pi=TMath::Pi();
	FairRunSim *fRun = new FairRunSim();

	//Output data FileNames
//	TString simOutput="/private2/huagen/fairroot/pandaroot_bak/macro/lmd/Lmd_DPM_6.2_elastic_1.9mrad_3M.root";
	TString simOutput="/private/huagen/simdata/Lmd_MC_DPM_elastic_6.2_1.9mrad_10M_test.root";
	TString parOutput="/private/huagen/simdata/LmdParamsRecoil.root";// ?

	fRun->SetName("TGeant3");
	fRun->SetOutputFile(simOutput);
	fRun->SetMaterials("media_pnd.geo");

// *************************************************************
// Step 1 of the main flowchart to create detector
// *************************************************************
	// Create and add detectors	// Add cave of panda, it's one of the FAIR moduel
	FairModule *Cave= new PndCave("CAVE");
        Cave->SetGeometryFileName("pndcave.geo");
	fRun->AddModule(Cave);
         // Add magnet of panda on this macro, also FAIR module
//	FairModule *Magnet= new PndMagnet("MAGNET");
//	Magnet->SetGeometryFileName("magnet.geo");
//	fRun->AddModule(Magnet);
         // Add beam pine around target, FAIR module
	FairModule *Pipe= new PndPipe("PIPE");
//	Pipe->SetGeometryFileName("pipebeamtarget_recoil.geo");
	fRun->AddModule(Pipe);
         // Define the Lumi monitor detector, it's a FairDetector instead of FAIR module.
    //cout<<"the fairdetector definition is OK!";
	FairDetector* Lmd= new PndLmdDetector("lmd",kTRUE);
	Lmd->SetGeometryFileName("recoil.geo");
	Lmd->SetVerboseLevel(1);
    fRun->AddModule(Lmd);

// ***************************************************************
// Step 2 of the main flowchart to define the event generator
// ***************************************************************
    // Create and set event generator    // Invoke the primGen for this simulation run
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    //  Dpm Generator
      PndDpmGenerator* dpmGen =
     new PndDpmGenerator("/home/huagen/fairroot/pandarootJun24/trunk/pgenerators/DpmEvtGen/Background-micro_6.2_elastic_0.113_1.9mrad_10M.root");
     	primGen->AddGenerator(dpmGen);

// Field Map Definition
	//	// MultiField:
	PndMultiField *fField= new PndMultiField();

	PndTransMap    *map_t  = new PndTransMap("TransMap", "R");
	PndDipoleMap   *map_d = new PndDipoleMap("DipoleMap", "R");
//	PndSolenoidMap *map_s = new PndSolenoidMap("SolenoidMap", "R");
	PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
	PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
	PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
	PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");

//	fField->AddField(map_t);
//	fField->AddField(map_d);
//	fField->AddField(map_s);
//	fField->AddField(map_s1);
//	fField->AddField(map_s2);
//	fField->AddField(map_s3);
//	fField->AddField(map_s4);

//switch offf the field on Jul. 13 2009
//	fRun->SetField(fField);

	fRun->SetStoreTraj(kTRUE);
	//fRun->SetStoreTraj(kFALSE);
	fRun->Init();

	FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
	Bool_t kParameterMerged=kTRUE;
	FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
	output->open(parOutput.Data(), "RECREATE");
	rtdb->setOutput(output);

	// Initalize the monte carlo	        // ------------------------
	PndMultiFieldPar* fieldPar = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
	if ( fField ) {  fieldPar->SetParameters(fField); }
	fieldPar->setInputVersion(fRun->GetRunId(),1);
	fieldPar->setChanged(kTRUE);

	rtdb->saveOutput();
	rtdb->print();

	fRun->Run(nEvents);

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
int runDPMsim()
{
    runMCsim();
	  return 0;
}
