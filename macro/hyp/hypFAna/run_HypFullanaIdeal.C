// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


void run_HypFullanaIdeal(TString base="Hit_hypIdReco",int nevts=0, TString outfile="")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  //rootlogon();
 gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
 gSystem->Load("libGeoBase");
 gSystem->Load("libParBase");
 gSystem->Load("libBase");
 gSystem->Load("libMCStack");
 gSystem->Load("libField");
 gSystem->Load("libPassive");
 gSystem->Load("libGen");  
 gSystem->Load("libEmc"); 
 gSystem->Load("libgenfit");
gSystem->Load("libTrkBase");
 gSystem->Load("libGeane");
 gSystem->Load("libtrackrep");
 gSystem->Load("libtpc"); 
 gSystem->Load("libtpcreco");
 
 gSystem->Load("librecotasks");

 gSystem->Load("librazhyp");
 gSystem->Load("libHyp");
 gSystem->Load("libTof");
 gSystem->Load("libHypGe");
 gSystem->Load("libRho");

 //gSystem->Load("libMvd");
 //gSystem->Load("libMvdReco");
 //gSystem->Load("libLHETrack");
 // gSystem->Load("libanatask");
 //gSystem->Load("libfsim");
 /*
 gSystem->Load("librazhyp");
 gSystem->Load("libHyp");
 gSystem->Load("libHypGe");
 gSystem->Load("libRho");*/
 
  //if (outfile=="") outfile=infile+"_ana.root";

  //analysis
/* TString recofile="/d/panda01/asanchez/PANDA/hit_G3XimIdealreco.root";
   if (outfile=="")
   outfile=base+".AnaFG3Xim2.root";
   TString simfile="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck2.root";
   TString simfile2="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck2_1.root";
   TString infile="/d/panda01/asanchez/PANDA/jul08fsim/Hit_hypIdReco.G3Ximfull4.root";
*/
 
/*TString recofile="/d/panda01/asanchez/PANDA/hit_G3RZHIdealreco.root";
  if (outfile=="")
  outfile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.AnaFG3RZH.root";
  TString simfile="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck3.root";
  TString simfile2="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck3_1.root";
  TString simfile3="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck3_2.root";
  TString simfile4="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck3_3.root";
  TString infile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3fullRZP.root";*/
 
 /*TString recofile="/d/panda01/asanchez/PANDA/hit_G3RZHximIdealreco.root";
 if (outfile=="")
   outfile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.AnaFG3RZHxim.root";
 TString simfile="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck4.root";
 TString simfile2="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck4_1.root";
 
 TString infile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3fullRZPxim.root";*/
 
 /*TString infile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.G3fullRZPm.root";
   if (outfile=="")
   outfile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.AnaFG3RZPm.root";
   TString simfile="/d/pndint02/asanchez/sim_BupG3hypcheck5.root";
   TString simfile2="/d/pndint02/asanchez/sim_BupG3hypcheck5_1.root";
   TString simfile3="/d/pndint02/asanchez/sim_BupG3hypcheck5_2.root";
   TString simfile4="/d/pndint02/asanchez/sim_BupG3hypcheck5_3.root";
   TString recofile = "/d/pndint02/asanchez/PANDA/hit_G3RZHIdrec2.root";*/
 
 /*
   TString infile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.fullG4Bup6.root";
   if (outfile=="")
   outfile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.AnaFG4Bup6.root";
   TString simfile="/d/pndint02/asanchez/sim_BupGenG4hypcheck6.root";
   TString recofile = "/d/pndint02/asanchez/PANDA/hit_G4Bup6Idealreco.root";
 */

 TString infile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.G3fullRZPm2.root";
 if (outfile=="")
   outfile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.FG3RZPm2.root";
 TString simfile="/d/pndint02/asanchez/sim_BupG3hypcheck5.root";
 TString simfile2="/d/pndint02/asanchez/sim_BupG3hypcheck5_1.root";
 TString simfile3="/d/pndint02/asanchez/sim_BupG3hypcheck5_2.root";
 TString simfile4="/d/pndint02/asanchez/sim_BupG3hypcheck5_3.root";
 
 //TString recofile = "/d/pndint02/asanchez/PANDA/hit_G4Bup6Idealreco.root";


   CbmRunAna *fRunA= new CbmRunAna();
  fRunA->SetInputFile(simfile);
  fRunA->AddFriend(infile);
  //fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile2.Data());
  fRunA->AddFriend(infile);
  //fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile3.Data());
  fRunA->AddFriend(infile);
  //fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile4.Data());
  fRunA->AddFriend(infile);
  //fRunA->AddFriend(recofile);
  fRunA->SetOutputFile(outfile.Data());
  
  //append the analysis task
  //PndMicroWriter *tcandTask=new PndMicroWriter();
  //fRunA->AddTask(tcandTask);
  
  PndHypFullIdealAna *anaTask=new PndHypFullIdealAna();
  fRunA->AddTask(anaTask);
  
  fRunA->Init();
  //if (nevts==0) nevts=10;
  fRunA->Run(0,nevts);
  anaTask->Finish(base.Data());
  
  	
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
