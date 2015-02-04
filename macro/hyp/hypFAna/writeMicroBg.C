// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


void writeMicroBg(TString base="Hit_hypIdReco",int nevts=0, TString outfile="")
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
  /*gSystem->Load("libGeoBase");
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
 gSystem->Load("libtrackrep");*/
 //gSystem->Load("librecotasks");
 gSystem->Load("librazhyp");
 gSystem->Load("libHyp");
 gSystem->Load("libTof");
 gSystem->Load("libHypGe");
 //gSystem->Load("libMvd");
 //gSystem->Load("libMvdReco");
 //gSystem->Load("libLHETrack");
 //gSystem->Load("libanatask");
 //gSystem->Load("libfsim");
 //gSystem->Load("libRho");
 
  //if (outfile=="") outfile=infile+"_ana.root";

  //analysis hyperfr 7396 ev
 /* TString infile=base+".mcrecdos.root";
  if (outfile=="")
  	outfile=base+".full.root";
  TString simfile="/d/panda01/asanchez/PANDA/BuphypAllXXb.root";
 */

 /*   
      //background 8394 events
      TString infile=base+".Bgrecdos.root";
      if (outfile=="")
      outfile=base+".Bgfull.root";
      TString simfile="/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08.root";
      TString simfile2 = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____1.root";
      TString simfile3 = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____2.root";
      TString simfile4 = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____3.root";
 */

/* 
 TString infile=base+".G3recdos.root";
 if (outfile=="")
   outfile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3fullfiv.root";
 TString simfile="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck.root";
  TString simfile2 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__1.root";
  TString simfile3 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__2.root";
  TString simfile4 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__3.root";
  TString simfile5 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__4.root";
  TString RecoFile = "/d/panda01/asanchez/PANDA/hit_G3Idealreco.root";*/

 /*
   TString infile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3RZHrecdos2.root";
   if (outfile=="")
   outfile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3fullRZP.root";
   
   TString RecoFile = "/d/panda01/asanchez/PANDA/hit_G3RZHIdealreco.root";
 */
 
 /*
   TString infile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3RZHximrecdos2.root";
   if (outfile=="")
   outfile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3fullRZPxim.root";
   
   TString RecoFile = "/d/panda01/asanchez/PANDA/hit_G3RZHximIdealreco.root";
 */
 
 /*TString infile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.G3RZHmrecdos.root";
   if (outfile=="")
   outfile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.G3fullRZPm.root";
   
   TString RecoFile = "/d/pndint02/asanchez/PANDA/hit_G3RZHIdrec2.root";*/
 
 // TString infile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.G3RZHm2recdos.root";
 

 //  TString simfile = "/d/pndint02/asanchez/sim_BupG3hypcheck5.root";
 //  TString simfile2 = "/d/pndint02/asanchez/sim_BupG3hypcheck5_1.root";
 //  TString simfile3 = "/d/pndint02/asanchez/sim_BupG3hypcheck5_2.root";
 //  TString simfile4 = "/d/pndint02/asanchez/sim_BupG3hypcheck5_3.root";
 //  if (outfile=="")  outfile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.G3fullRZPm2.root";
 
 //TString RecoFile = "/d/pndint02/asanchez/PANDA/hit_G3RZHIdrec2.root";
 
 /*
     
   
   TString infile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.G4Bup6recdos.root";
   if (outfile=="")
   outfile="/d/pndint02/asanchez/PANDA/Hit_hypIdReco.fullG4Bup6.root";
   
   TString RecoFile = "/d/pndint02/asanchez/PANDA/hit_G4Bup6Idealreco.root";
   */

 /* //TString infile=base+".G3Ximrecdos.root";//ends saving in file
    TString infile=base+".G3Ximrecdos2.root";//ends no finding file
    //TString outfile="Hit_hypIdReco.G3Ximfull4.root";//49900
    TString outfile="Hit_hypIdReco.G3Xim3full.root";//50000
    TString simfile="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck2.root";
    TString simfile2 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck2_1.root";
 */

 // Aida calculations 28.10.09 200K ev.
 // first step

 // TString infile="/d/pndint02/asanchez/FFeb09/HypIdFi1TAidGrecdos.root";
 //  if (outfile=="")  outfile="/d/pndint02/asanchez/FFeb09/Hit_hypIdReco.fullG4Fi1TAidG.root";
 //  TString RecoFile = "/d/pndint02/asanchez/FFeb09/hypIdealFi1TAidGreco.root";

 // second step
 // TString infile="/d/pndint02/asanchez/FFeb09/HypIdBup1TAidGrecdos.root";
 //  if (outfile=="")  outfile="/d/pndint02/asanchez/FFeb09/Hit_hypIdReco.fullG4Bup1TAidG.root";
 //  TString RecoFile = "/d/pndint02/asanchez/FFeb09/hypIdealBup1TAidGreco.root";

 //date 7.09.09 tof trig cut tpc repair 1T

 // first step
 //TString infile="/d/pndint02/asanchez/FFeb09/Hit_G4FiSF1TTrigIdrecdos.root";
 //if (outfile=="")  outfile="/d/pndint02/asanchez/FFeb09/Hit_hypIdReco.fullG4FS1TTg.root";
 //TString RecoFile = "../../tof/trig_kaonmulcheck.root";
 // second step
 // TString infile="/d/pndint02/asanchez/FFeb09/HypIdBup1TTgrecdos.root";
 //   if (outfile=="")  outfile="/d/pndint02/asanchez/FFeb09/Hit_hypIdReco.fullG4Bup1TTg.root";
 //  TString RecoFile = "/d/pndint02/asanchez/FFeb09/hypIdealBup1TTgreco.root";

 // date 9.11.09 Aida Calc 0.5 T
 // first step
 // TString infile="/d/pndint02/asanchez/FOct09/HitIdealFSG405TAxxbGrecdos.root";
 //  if (outfile=="")  outfile="/d/pndint02/asanchez/FOct09/Hit_hypIdReco.fullFSG405TAG.root";
 //  TString RecoFile ="/d/pndint02/asanchez/FOct09/hypIdealFSG405TAxxbGreco.root";
 //  TString parFile="../SimG405TAxxbGparams.root";
 // second step
 // TString infile="/d/pndint02/asanchez/FOct09/HitIdBup405TAidGrecdos.root";
//  if (outfile=="")  outfile="/d/pndint02/asanchez/FOct09/Hit_hypIdReco.fullBupG405TAG.root";
//  TString RecoFile ="/d/pndint02/asanchez/FOct09/hypIdealBupG405TAxxbGreco.root";
//  TString parFile="../BupG405TAxxbGparams.root";

 //tof trigger second step 1 T
 //second step
 // TString infile="/d/pndint02/asanchez/FOct09/HitIdBup41TTgAidGrecdos.root";
//  if (outfile=="")  outfile="/d/pndint02/asanchez/FOct09/Hit_hypIdReco.fullBupG41TTgAG.root";
//  TString RecoFile ="/d/pndint02/asanchez/FOct09/hypIdealBupG41TTgAxxbGreco.root";
//  TString parFile="../BupG41TTgAxxbGparams.root";

 //first step
 // TString infile="/d/pndint02/asanchez/FOct09/HitIdhyp41TTgAidGrecdos.root";
 //  if (outfile=="")  outfile="/d/pndint02/asanchez/FOct09/Hit_hypIdReco.fullhypG41TTgAG.root";
 //   TString RecoFile ="../hyp/trig_kaonmulAidaG1T80LHps.root";
 //   TString parFile="../SimG41TAxxbGparams.root";

 //urqmd first step
 TString infile="/d/pndint02/asanchez/FOct09/HitIdBg1TTg450recdos.root";
 if (outfile=="")  outfile="/d/pndint02/asanchez/FOct09/Hit_hypIdReco.fullBgG41TTg450.root";
 TString RecoFile ="../../tof/trig_kaonmulBg1T450psLHc.root";
 TString parFile="../SimG41TUrqmdSmmparams.root";
 
 FairRunAna *fRunA= new FairRunAna();
  fRunA->SetInputFile(infile);
  fRunA->AddFriend(RecoFile);

  // fRunA->SetInputFile(simfile);
  //   fRunA->AddFriend(infile);
  //   fRunA->AddFile(simfile2);
  //   fRunA->AddFriend(infile);
  //   fRunA->AddFile(simfile3);
  //   fRunA->AddFriend(infile);
  //   fRunA->AddFile(simfile4);
  //   fRunA->AddFriend(infile);
  //fRunA->AddFile(simfile5);
  //fRunA->AddFriend(infile);
  
  //fRunA->SetInputFile(infile);

  fRunA->SetOutputFile(outfile.Data());
  
  // -----  Parameter database   --------------------------------------------
  
  FairRuntimeDb* rtdb = fRunA->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  Bool_t kParameterMerged=kTRUE;
  rtdb->setFirstInput(parInput1);

  //append the analysis task
 
  
  PndHypMicroWriter *tcandTask=new PndHypMicroWriter();
  fRunA->AddTask(tcandTask);


  //PndHypMicroIdealWriter *tcandTask=new PndHypMicroIdealWriter();
  //fRunA->AddTask(tcandTask);
  
  //PndSimpleAnalysis *anaTask=new PndSimpleAnalysis(cfgfile.Data());
  //fRunA->AddTask(anaTask);
  
  fRunA->Init();
  //if (nevts==0) nevts=10;
  fRunA->Run(0,nevts);
  
  	
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
