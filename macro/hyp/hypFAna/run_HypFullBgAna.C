// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


void run_HypFullBgAna(TString base="Hit_hypIdReco",int nevts=5000, TString outfile="")
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
 gSystem->Load("libHyp");
 gSystem->Load("libTof");
 gSystem->Load("libHypGe");
 //gSystem->Load("libMvd");
 //gSystem->Load("libMvdReco");
 //gSystem->Load("libLHETrack");
 gSystem->Load("libanatask");
 //gSystem->Load("libfsim");
 gSystem->Load("libRho");
 
  //if (outfile=="") outfile=infile+"_ana.root";

  //analysis

 /*
   TString simfile="/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08.root";
   TString simfile2 = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____1.root";
   TString simfile3 = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____2.root";
   TString simfile4 = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____3.root";
   
   TString infile=base+".Bgfull.root";
   
   TString recofile="../hyp/hit_BgIdealreco.root";//hit_idealreco.root";
   
   if (outfile=="")outfile=base+".AnaFBg.root";
 */
  TString simfile="/d/panda01/asanchez/PANDA/sim_BupG3hypcheck.root";
  TString simfile2 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__1.root";
  TString simfile3 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__2.root";
  TString simfile4 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__3.root";
  TString simfile5 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__4.root";

  //TString infile="/d/panda02/asanchez/fairsoft_jul08/pandaroot/trunk/tutorials/anatask/dsdsj20k.evt.full.root";
  TString infile="/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3fullfiv.root";
  
  TString recofile="/d/panda01/asanchez/PANDA/hit_G3Idealreco.root";//hit_idealreco.root";

  if (outfile=="")outfile=base+".AnaFG3Moth2.root";

  CbmRunAna *fRunA= new CbmRunAna();
  fRunA->SetInputFile(simfile.Data());
  fRunA->AddFriend(infile.Data());
  fRunA->AddFriend(recofile.Data());
  fRunA->AddFile(simfile2.Data());
  fRunA->AddFriend(infile.Data());
  fRunA->AddFriend(recofile.Data());
  fRunA->AddFile(simfile3.Data());
  fRunA->AddFriend(infile.Data());
  fRunA->AddFriend(recofile.Data());
  fRunA->AddFile(simfile4.Data());
  fRunA->AddFriend(infile.Data());
  fRunA->AddFriend(recofile.Data());
  fRunA->AddFile(simfile5.Data());
  fRunA->AddFriend(infile.Data());
  fRunA->AddFriend(recofile.Data());
  fRunA->SetOutputFile(outfile.Data());
  
  //append the analysis task
  //PndMicroWriter *tcandTask=new PndMicroWriter();
  //fRunA->AddTask(tcandTask);
  
  PndHypFullAna *anaTask=new PndHypFullAna();
  fRunA->AddTask(anaTask);
  
  fRunA->Init();
  if (nevts==0) nevts=10;
  fRunA->Run(0,nevts);
  anaTask->Finish("AnaFBg");
  
  	
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
 cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outfile << endl;
}  
  
