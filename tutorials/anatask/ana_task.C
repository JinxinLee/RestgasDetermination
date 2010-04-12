void ana_task(TString dir)
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");rootlogon();
	if(isLibrary("libanatask"))gSystem->Load("libanatask");
  	
  	TString sysFile = gSystem->Getenv("VMCWORKDIR");
  	TString curDir = "/tutorials/analysis/";
  	
  	TString parFile = dir+"/params_tpccombi.root";
  	TString inSimuFile = dir+"/points_tpccombi.root";
  	TString inDataFile = dir+"/pid_tpccombi.root";
  	TString inDigiFile = dir+"/digi_tpccombi.root";
  	TString inRecoFile = dir+"/reco_tpccombi.root";
  	
  	TString outfile="";
    if (outfile=="") outfile="ana_task.root";
	
	FairRunAna *fRun= new FairRunAna();
  	fRun->SetInputFile(inDataFile);
    fRun->AddFriend(inSimuFile);
    
    fRun->SetOutputFile(outfile);
  	
  	
  	//PndMcListConverter *mcc=new PndMcListConverter();
  	//fRun->AddTask(mcc);
  	//mcc->SetVerbose(0);
  	
  	PndJpsi2PiAna *anaTask=new PndJpsi2PiAna();
  	fRun->AddTask(anaTask);

  	fRun->Init();
  	fRun->Run(0,0);
  
}
	
