// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


int ana_fast(TString infile="dsdsj_20k.evt.root",int nevts=0,TString outfile="ana.root")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libGen");
  gSystem->Load("libRho");
  gSystem->Load("libfsim");
  
 
  if (outfile=="") outfile=infile+"_ana.root";

  //analysis
  	
  FairRunAna *fRunA= new FairRunAna();
  fRunA->SetInputFile(infile);
  fRunA->SetOutputFile(outfile);
  
  //append the analysis task
  PndAnalysis *anaTask=new PndAnalysis();
  fRunA->AddTask(anaTask);

  fRunA->Init();
  if (nevts==0) nevts=10;
  fRunA->Run(0,nevts);
  
  	
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}  
  
