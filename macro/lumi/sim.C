/*
 * sim.C
 *
 *  Created on: Feb 1, 2012
 *      Author: tsito
 */

void sim()
{
	   TStopwatch timer;
		timer.Start();
		gDebug=0;

		//gROOT->Reset();
		// Load basic libraries
		gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
		rootlogon();

		Double_t pi=TMath::Pi();
		Double_t rad = 180./pi;
		Int_t nEvents = 100;
		Double_t pbeam = 8.9;

		// Create a monte carlo
		// --------------------
		FairRunSim *fRun = new FairRunSim();

		//FileNames
		TString simFile = "test_G4.root";
		TString parFile= "params_G4.root";

		// Set the MC version used
		// ------------------------
		fRun->SetName("TGeant4");
		fRun->SetOutputFile(simFile);

		// Create and add detectors
		// ------------------------
		fRun->SetMaterials("media_pnd.geo");

		FairModule *Cave= new PndCave("CAVE");
		Cave->SetGeometryFileName("pndcave1.geo");
		fRun->AddModule(Cave);

		FairDetector *Lumi = new PndLumi("Lumi", kTRUE);
		Lumi->SetGeometryFileName("lumishiftandrotate.root");
		Lumi->SetVerboseLevel(1);
	    fRun->AddModule(Lumi);

//	    FairDetector *Stt= new PndStt("STT", kTRUE);
//	    Stt->SetGeometryFileName("straws_skewed_blocks_pipe_120cm.geo");
//	    fRun->AddModule(Stt);
	    // Create and set event generator
	    // ------------------------------
	    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	    fRun->SetGenerator(primGen);

	    // Box Generator:
	    //---------------------------
	    FairBoxGenerator* boxGen = new FairBoxGenerator(-2212, 1);
	    boxGen->SetPRange(pbeam , pbeam); // GeV/c
		boxGen->SetPhiRange(0.,360.);// Azimuth angle range [degree]
		Double_t theta_min = 0.0028*rad;
		Double_t theta_max = 0.008*rad;

		boxGen->SetThetaRange(theta_min, theta_max);//TMath::ATan(0.02)*rad);
		boxGen->SetXYZ(0.0,0.0, 0.0); // vertex coordinates [cm]
		primGen->AddGenerator(boxGen);

		// Field Map
		//-------------------------
		fRun->SetBeamMom(pbeam);
		//PndMultiField *fField= new PndMultiField("DIPOLE");
		PndMultiField *fField= new PndMultiField("FULL");
		fRun->SetField(fField);


		fRun->SetStoreTraj(kTRUE);

		// Initalize the monte carlo
		// -------------------------
		fRun->Init();

		FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
		Bool_t kParameterMerged=kTRUE;

		FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
		output->open(parFile.Data(), "RECREATE");
		rtdb->setOutput(output);

		PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
		if (fField) {  Par->SetParameters(fField); }
		Par->setInputVersion(fRun->GetRunId(),1);
		Par->setChanged();

		rtdb->saveOutput();
		rtdb->print();

		fRun->Run(nEvents);
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
//		exit();
}
