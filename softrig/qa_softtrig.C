bool checkfile(TString fn)
{
	bool fileok=true;
	TFile fff(fn); 
	if (fff.IsZombie()) fileok=false;
	TTree *t=(TTree*)fff.Get("cbmsim");
	if (t==0x0) fileok=false;
	
	if (!fileok) cout <<"Skipping broken file '"<<fn<<"'"<<endl;
	return fileok;
}

void qa_softtrig(TString outpre="M9999", int mode, double pmom, int from=1, int to=1, int nEvents=0)
{
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libSofTrig");
	
  	TString OutFile   = outpre+"_st_qa.root"; 
	TString inParFile = TString::Format("%s_%d_par.root",outpre.Data(),from);
	
  	bool finefile[1000];
  	for (int i=0;i<1000;++i) finefile[i]=false;
  
  	FairRunAna *fRun= new FairRunAna();
  
   	bool firstfile=true;

  	// add pid files
  	for (int i=from;i<=to;++i)
  	{
		TString fname = outpre+"_"; fname+=i; fname+="_pid.root";
		//TString fname2 = outpre+"_"; fname2+=i; fname2+="_sim.root";

		if (checkfile(fname) )//&& checkfile(fname2))
		{
			finefile[i]=true;
			if (firstfile) fRun->SetInputFile(fname);
			else fRun->AddFile(fname);
			firstfile=false;
		}
	
  	}
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	// *** initialization
/*	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	rtdb->setFirstInput(parIO);
	rtdb->setOutput(parIO);  */
	
	fRun->SetOutputFile(OutFile);
 	
	//---------------------Create and Set the Field(s)---------- 
  	//PndMultiField *fField= new PndMultiField("FULL");
  	//fRun->SetField(fField);
	
	RhoCalculationTools::ForceConstantBz(20.0);
  
	
	// *** HERE OUR TASK GOES!
	PndSoftTriggerTask *stTask = new PndSoftTriggerTask(pmom, mode);
	
	stTask->SetPi0SignalParams(0.135, 0.01);
	stTask->SetEtaSignalParams(0.547, 0.03);
	stTask->SetKs0SignalParams(0.493, 0.05);

	stTask->SetGammaMinE(0.15);
	stTask->SetTrackMinP(0.15);
	
	TString algo = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts";
	
	stTask->SetAlgoElectron(algo);
	stTask->SetAlgoMuon(algo);
	stTask->SetAlgoPion(algo); 
	stTask->SetAlgoKaon(algo);
	stTask->SetAlgoProton(algo);

	fRun->AddTask(stTask);
	
	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nEvents);
	
	//gObjectTable->Print();
}
