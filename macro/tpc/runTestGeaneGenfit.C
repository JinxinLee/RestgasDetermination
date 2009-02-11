// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libPndBase");
  gSystem->Load("libPassive");
  gSystem->Load("libTrkBase");  
  gSystem->Load("libGeane");
  gSystem->Load("libStt");
  //gSystem->Load("libPlane");
  gSystem->Load("libGen");
  //gSystem->Load("libGeaneEx");
  gSystem->Load("libgenfit");
  
  FairRunAna *fRun = new FairRunAna();

  // fRun->SetInputFile("data/test1.mc.root");
//   //fRun->AddFriend("aftestd.root");
//   //fRun->AddFriend("aftestr.root");
//   fRun->SetOutputFile("aftestg.root");

   //FairGeane *Geane = new FairGeane();

  TString base="data/test1";
  TString infile=base+".mc.root";
  TString outfile=base+".g.root";
  fRun->SetInputFile(infile);
  fRun->SetOutputFile(outfile);

   FairGeane *Geane = new FairGeane(infile);

   //   FairGeaneTr *tr= new FairGeaneTr();
//   FairGeaneTrC *tr= new FairGeaneTrC();
//   FairGeaneTrT *tr= new FairGeaneTrT();
//   FairGeaneTrP *tr= new FairGeaneTrP();
//   FairGeaneTrK *tr= new FairGeaneTrK();
   // fRun->AddTask(tr);

 //  FairGeaneTrP *trP= new FairGeaneTrP();
 //  fRun->AddTask(trP);
 // -----  Parameter database   --------------------------------------------

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();

  TString dbfile=base+".param.root";
  parInput1->open(dbfile.Data()); 
  rtdb->setFirstInput(parInput1);

  // ------------------------------------------------------------------------

  fRun->Init();

  rtdb->print();
 // Set the field(if any) to Geane
  Geane->SetField(fRun->GetField());
     
  // Transport nEvents
  // -----------------
  TStopwatch timer;
  timer.Start();
  
  FairGeanePro* gePro=new FairGeanePro();
  
  FairTrackParP dummy;

  TVector3 pos(0.,0.,0.);
  TVector3 mom(0.1,0.0001,1.);
  TVector3 poserr=TVector3(0.1,0.1,0.1);
  TVector3 momerr=0.1*mom;

  DetPlane initplane(TVector3(pos),TVector3(1.,0.,0.),TVector3(0.,1.,0.));
  
  double q=1;
  
  GeaneTrackRep rep(gePro,pos,mom,poserr,momerr,q,initplane);

  rep.Print();

  DetPlane plane(TVector3(0,0,20),TVector3(1,0,0),TVector3(0,1,0));

  TMatrixT<double> newpar(5,1);
  TMatrixT<double> newcov(5,5);
  TMatrixT<double> jac(5,5);



  rep.extrapolate(plane,newpar,newcov,jac);
  
  newpar.Print();
  newcov.Print();

  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
