// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


void run_HypFullAVeto(TString base="Hit_hypIdReco",int nevts=0, TString outfile="")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
 gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
 /* gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
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
 gSystem->Load("libtpc"); 
 gSystem->Load("libtpcreco");
 gSystem->Load("libtrackrep");
 gSystem->Load("librecotasks");
 gSystem->Load("librazhyp");
 gSystem->Load("libHyp");
 gSystem->Load("libTof");
 gSystem->Load("libHypGe");
 //gSystem->Load("libMvd");
 //gSystem->Load("libMvdReco");
 //gSystem->Load("libLHETrack");
 // gSystem->Load("libanatask");
 //gSystem->Load("libfsim");
*/
 gSystem->Load("librazhyp");
 gSystem->Load("libHyp");
 gSystem->Load("libHypGe");
 gSystem->Load("libRho");
 
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
 
 /*TString infile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.fullG4Bup6.root";
 if (outfile=="")
   outfile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.AnaFG4Bup6.root";
 TString simfile="/d/pndint02/asanchez/sim_BupGenG4hypcheck6.root";
 TString recofile = "/d/pndint02/asanchez/PANDA/hit_G4Bup6Idealreco.root";*/

  //g4 1ST XXb signal
 if (outfile=="")
   outfile="/d/pndint02/asanchez/Hit_hypIdReco.AVetoG4ST.root";
 TString recofile="/d/pndint02/asanchez/hit_G4STIdealreco.root";
 TString simfile = "/d/panda01/asanchez/PANDA/sim_hypAllXXb.root";
 TString simfile2 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_1.root";
 TString simfile3 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_2.root";
 TString simfile4 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_3.root";
 TString simfile5 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_4.root";
 TString simfile6 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_5.root";
 TString simfile7 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_6.root";
 TString simfile8 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_7.root";
 TString simfile9 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_8.root";
 TString simfile10 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_9.root";
 TString simfile11 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_10.root";
 TString simfile12 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_11.root";
 TString simfile13 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_12.root";
 TString simfile14 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_13.root";
 TString simfile15 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_14.root";
 TString simfile16 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_15.root";
 TString simfile17 = "/d/panda01/asanchez/PANDA/sim_hypAllXXb_16.root";

 TString infile="/d/pndint02/asanchez/Hit_hypIdReco.G4STfull.root";

   
   CbmRunAna *fRunA= new CbmRunAna();
  fRunA->SetInputFile(simfile);
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile2.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile3.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile4.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile5);
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile6.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile7.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile8.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile9);
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile10.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile11.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile12.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile13);
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile14.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile15.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile16.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);
  fRunA->AddFile(simfile17.Data());
  fRunA->AddFriend(infile);
  fRunA->AddFriend(recofile);

   /*fRunA->SetInputFile(simfile);
     fRunA->AddFriend(infile);
     fRunA->AddFriend(recofile);
     fRunA->AddFile(simfile2.Data());
     fRunA->AddFriend(infile);
     fRunA->AddFriend(recofile);
     fRunA->AddFile(simfile3.Data());
     fRunA->AddFriend(infile);
     fRunA->AddFriend(recofile);
     fRunA->AddFile(simfile4.Data());
     fRunA->AddFriend(infile);
     fRunA->AddFriend(recofile);*/
   fRunA->SetOutputFile(outfile.Data());
  
  //append the analysis task
  //PndMicroWriter *tcandTask=new PndMicroWriter();
  //fRunA->AddTask(tcandTask);
  
  PndHypFullAna *anaTask=new PndHypFullAna();
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
  
