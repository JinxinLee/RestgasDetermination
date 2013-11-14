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

void prod_ana(TString outpre="M9999", int from=1, int to=1, int nevts=0)
{
 	TString OutFile1   = TString::Format("%s_ana1_%d_%d.root",outpre.Data(), from, to); 
 	TString OutFile2   = TString::Format("%s_ana2_%d_%d.root",outpre.Data(), from, to); 
	TString inParFile = TString::Format("%s_%d_par.root",outpre.Data(),from);
	
  	FairRunAna *fRun= new FairRunAna();
  
   	bool firstfile=true;

  	// *** Add pid files
  	for (int i=from;i<=to;++i)
  	{
		TString fname = TString::Format("%s_%d_pid.root",outpre.Data(),i);

		if ( checkfile(fname) )
		{
			if (firstfile) 
				fRun->SetInputFile(fname);
			else 
				fRun->AddFile(fname);
			
			firstfile=false;
		}
  	}
  	
	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";	
	
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	
	// *** setup parameter database 	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
	parIOPid->open(pidParFile.Data(),"in");
	
	rtdb->setFirstInput(parIO);
	rtdb->setSecondInput(parIOPid);
	rtdb->setOutput(parIO);  
	rtdb->setContainersStatic();
	
	fRun->SetOutputFile(OutFile1);
	fRun->Init();
	
	//---------------------Create and Set the Field(s)---------- 
  	PndMultiField *fField= new PndMultiField("FULL");
  	fRun->SetField(fField);
	
	//RhoCalculationTools::ForceConstantBz(20.0);

	// ***
	// *** HERE YOUR ANALYSIS CODE GOES!
	// ***
	
	PndAnalysis* fAnalysis = new PndAnalysis();
	if (nevts==0) nevts= fAnalysis->GetEntries();
	
	// *** RhoCandLists for the analysis
	RhoCandList chrg;
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open(OutFile2,"RECREATE");
	
	TH1F *hmom = new TH1F("hmom","momentum",100,0,6);
	// ***
	// the event loop
	// ***
	while (fAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) 
		{ 
			cout<<"evt " << i << endl; 
		}
	
		fAnalysis->FillList(chrg , "Charged");
		
		for (int j=0;j<chrg.GetLength();++j)
		{
			hmom->Fill(chrg[j]->P());
		}
	}
	out->cd();
	hmom->Write();
	out->Save();
	
}

















