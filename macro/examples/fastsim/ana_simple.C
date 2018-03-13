// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


int ana_simple(TString infile,TString cfgfile="analysis.cfg",int nevts=0,TString outfile="")
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
  //gSystem->Load("libPndData");
  //gSystem->Load("libGen");
  gSystem->Load("libRho");
  gSystem->Load("libfsim");

  if (outfile=="") outfile=infile+"_ana.root";

  //analysis

  TFile f(infile);
  TTree *t=(TTree*)f->Get("pndsim");

  int ntree=t->GetEntries();
  if (nevts==0 || nevts>ntree) nevts=ntree;
  f.Close();

  cout <<"\n####### Processing "<<nevts <<" events...\n"<<endl;


  FairRunAna *fRunA= new FairRunAna();
  fRunA->SetInputFile(infile);
  fRunA->SetOutputFile(outfile);

  //append the analysis task
  PndSimpleAnalysis *anaTask=new PndSimpleAnalysis(cfgfile.Data());
  fRunA->AddTask(anaTask);

  fRunA->Init();
  fRunA->Run(0,nevts);


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}

