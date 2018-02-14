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

int ana_task_mult(TString outpre="test", int from=1, int to=1, double pbarmom = 6.232069, int nevts=0)
{
 	// *** set this to your output path
	TString OutPath  = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/scrut/data";	
	TString Filename = OutPath + "/" + outpre(outpre.Last('/')+1,outpre.Length()); // cut away input path
	
	TString OutFile  = TString::Format("%s_fana_%d_%d.root",  Filename.Data(), from, to); 
 	TString OutDummy = TString::Format("%s_dummy_%d_%d.root", Filename.Data(), from, to); 
	
  	FairRunAna *fRun= new FairRunAna();
  
   	bool firstfile=true;

  	// *** Add input files
  	for (int i=from;i<=to;++i)
  	{
		TString fname = TString::Format("%s_%d_fast.root",outpre.Data(),i);

		if ( checkfile(fname) )
		{
			if (firstfile) 
				fRun->SetInputFile(fname);
			else 
				fRun->AddFile(fname);
			
			firstfile=false;
		}
  	}
						
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);

	fRun->SetGenerateRunInfo(kFALSE);
	fRun->SetOutputFile(OutDummy);

	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);

	// *** HERE OUR ANALYSIS TASK GOES!
	PndScrutAnaTask *scrutTask = new PndScrutAnaTask(pbarmom, OutFile);
	fRun->AddTask(scrutTask);

	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
  return 0;
}
