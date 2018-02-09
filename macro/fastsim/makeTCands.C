// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


int makeTCands(TString base="dsdsj20k.evt",int nevts=10)
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
 gSystem->Load("libField");
 gSystem->Load("libPassive");
 gSystem->Load("libGen");  
 gSystem->Load("libEmc"); 
 gSystem->Load("libgenfit");
 gSystem->Load("libtpc"); 
 gSystem->Load("libtpcreco");
 gSystem->Load("libtrackrep");
 //gSystem->Load("librecotasks");
 //gSystem->Load("libMvd");
 //gSystem->Load("libMvdReco");
 //gSystem->Load("libLHETrack");
 gSystem->Load("libanatask");
 //gSystem->Load("libfsim");
 gSystem->Load("libRho");
 
  //if (outfile=="") outfile=infile+"_ana.root";

  //analysis
  TString infile=base+".mcreco.root";
  TString outfile=base+".full.root";
  	
  FairRunAna *fRunA= new FairRunAna();
  fRunA->SetInputFile(infile);
  fRunA->SetOutputFile(outfile.Data());
  
  //append the analysis task
  PndStoreTCands *tcandTask=new PndStoreTCands();
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
  
