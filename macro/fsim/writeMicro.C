// *******
// Macro for converting simulated/reconstructed
// data to PndPidCandidates/TCandidates
// as input for analysis
// parameters: 
//     recofn=filename of reco file
//     simfn =filename of McPoints
//     nevts = # of events
// *******


int writeMicro(TString recofn="", TString simfn="",int nevts=10, TString outfile="")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libAnalysisTools");
  gSystem->Load("libRho");
  
  //analysis
  TString infile=recofn;
  if (outfile=="")
  	outfile=recofn+".full.root";
  TString simfile=simfn;
  	
  FairRunAna *fRunA= new FairRunAna();
  fRunA->SetInputFile(infile);
  fRunA->AddFriend(simfile);
  fRunA->SetOutputFile(outfile.Data());
  
  //append the analysis task
  PndMicroWriter *tcandTask=new PndMicroWriter();
  fRunA->AddTask(tcandTask);
  
  //PndSimpleAnalysis *anaTask=new PndSimpleAnalysis(cfgfile.Data());
  //fRunA->AddTask(anaTask);
  
  fRunA->Init();
  if (nevts==0) nevts=10;
  fRunA->Run(0,nevts);
  
  	
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}  
  
