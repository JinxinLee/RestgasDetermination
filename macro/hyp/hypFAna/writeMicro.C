// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


void writeMicro(TString base="Hit_hypIdReco",int nevts=0, TString outfile="")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //basiclibs();
 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  // Load this example libraries

 gSystem->Load("librazhyp");
 gSystem->Load("libHyp");
 //gSystem->Load("libSciT");
 //gSystem->Load("libHypGe");

 
 

 // XXb calculations 05.09.13 2K ev.
 
// first step --> Xim + Xib (annihilation) production at 3 GeV/c (pbar)

 // TString infile="/d/pndint02/asanchez/FFeb09/HypIdFi1TAidGrecdos.root";
 //  if (outfile=="")  outfile="/d/pndint02/asanchez/FFeb09/Hit_hypIdReco.fullG4Fi1TAidG.root";
 //  TString RecoFile = "/d/pndint02/asanchez/FFeb09/hypIdealFi1TAidGreco.root";

 // second step --> Hypernuclei formation
 
 TString infile="../Alicia/HypId1TXXbP4pmBuprecdos.root";
 if (outfile=="")  outfile="../Alicia/Hit_hypIdReco.fullG4Bup1TXXb.root";
 TString RecoFile = "../Alicia/hypIdealrecoXXb.root";

 


 //tof trigger second step 1 T
 //second step
 // TString infile="/d/pndint02/asanchez/FOct09/HitIdBup41TTgAidGrecdos.root";
//  if (outfile=="")  outfile="/d/pndint02/asanchez/FOct09/Hit_hypIdReco.fullBupG41TTgAG.root";
//  TString RecoFile ="/d/pndint02/asanchez/FOct09/hypIdealBupG41TTgAxxbGreco.root";
//  TString parFile="../BupG41TTgAxxbGparams.root";

 //first step tof trigger 1T
 /* TString infile="/d/pndint02/asanchez/FOct09/HitIdhyp41TTg80AidGrecdos3.root"; //80 ps
   if (outfile=="")  outfile="/d/pndint02/asanchez/FOct09/Hit_hypIdReco.fullhypG41TTgAG80.root";
    TString RecoFile ="../../tof/trig_kaonmulAidaG1T80LHcps.root";
    TString parFile="../SimG41TAxxbGparams.root";

 */


 
 FairRunAna *fRunA= new FairRunAna();
  fRunA->SetInputFile(infile);
  fRunA->AddFriend(RecoFile);

  fRunA->SetOutputFile(outfile.Data());
  
  // -----  Parameter database   --------------------------------------------
  
  /* FairRuntimeDb* rtdb = fRunA->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  Bool_t kParameterMerged=kTRUE;
  rtdb->setFirstInput(parInput1);*/

  //append the analysis task
 
  
  PndHypMicroWriter *tcandTask=new PndHypMicroWriter();
  fRunA->AddTask(tcandTask);


  //PndHypMicroIdealWriter *tcandTask=new PndHypMicroIdealWriter();
  //fRunA->AddTask(tcandTask);
  
  
  
  fRunA->Init();
  //if (nevts==0) nevts=10;
  fRunA->Run(0,nevts);
  
  	
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
