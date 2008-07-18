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
  gSystem->Load("libMCStack");
  gSystem->Load("libPassive");
  gSystem->Load("libTrkBase");  
  gSystem->Load("libGeane");
  gSystem->Load("libStt");
  //gSystem->Load("libPlane");
  gSystem->Load("libGen");
  //gSystem->Load("libGeaneEx");
  gSystem->Load("libgenfit");
  
  CbmRunAna *fRun = new CbmRunAna();

  // fRun->SetInputFile("data/test1.mc.root");
//   //fRun->AddFriend("aftestd.root");
//   //fRun->AddFriend("aftestr.root");
//   fRun->SetOutputFile("aftestg.root");

   //CbmGeane *Geane = new CbmGeane();

  TString base="data/test1";
  TString infile=base+".mc.root";
  TString outfile=base+".g.root";
  fRun->SetInputFile(infile);
  fRun->SetOutputFile(outfile);

   CbmGeane *Geane = new CbmGeane(infile);

   //   CbmGeaneTr *tr= new CbmGeaneTr();
//   CbmGeaneTrC *tr= new CbmGeaneTrC();
//   CbmGeaneTrT *tr= new CbmGeaneTrT();
//   CbmGeaneTrP *tr= new CbmGeaneTrP();
//   CbmGeaneTrK *tr= new CbmGeaneTrK();
   // fRun->AddTask(tr);

 //  CbmGeaneTrP *trP= new CbmGeaneTrP();
 //  fRun->AddTask(trP);
 // -----  Parameter database   --------------------------------------------

  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();

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
  
  CbmGeanePro* gePro=new CbmGeanePro();
  
  CbmTrackParP dummy;

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
  
