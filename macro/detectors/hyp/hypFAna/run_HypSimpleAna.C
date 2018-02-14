// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


int run_HypSimpleAna(TString base="Hit_hypIdReco",int nevts=0, TString outfile="")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
 gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

 gSystem->Load("librazhyp");
 gSystem->Load("libHyp");

 gSystem->Load("libRho");
 

  //if (outfile=="") outfile=infile+"_ana.root";

  //analysis
  
  
  //aida second step 0.5 T
   TString simfile = "../Alicia/Bup_hypFSG41TXXb.root";
       Int_t nEvents = 0;//50000;
     TString parFile="../Alicia/Bup_hypFS1TG4XXbparams.root";
      TString recofile = "../Alicia/hypIdealrecoXXb.root";
      TString infile = "../Alicia/Hit_hypIdReco.fullG4Bup1TXXb.root";
      if (outfile=="")  outfile="Hit_hypIdReco.AnaSP4Bup1TXXb.root";


 
  
 


  FairRunAna *fRunA= new FairRunAna();
   
 
   fRunA->SetInputFile(simfile.Data());
   fRunA->AddFriend(infile);
   fRunA->AddFriend(recofile);
   // fRunA->AddFile(simfile2.Data());
   // fRunA->AddFriend(infile);
   // fRunA->AddFriend(recofile);
  
   
   fRunA->SetOutputFile(outfile.Data());
  
  //append the analysis task
  //PndMicroWriter *tcandTask=new PndMicroWriter();
  //fRunAA->AddTask(tcandTask);
  
  PndHypSimpleAna *anaTask=new PndHypSimpleAna();
  fRunA->AddTask(anaTask);
  
  fRunA->Init();
  //if (nevts==0) nevts=10;
  fRunA->Run(0,nevts);
  anaTask->Finish(base.Data());
  
  	
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}  
  
