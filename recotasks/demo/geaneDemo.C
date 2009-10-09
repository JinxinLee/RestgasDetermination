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
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libTrkBase");  
  gSystem->Load("libGeane");
  gSystem->Load("libStt");
  //gSystem->Load("libPlane");
  gSystem->Load("libGen");
  //gSystem->Load("libGeaneEx");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("libtrackrep");
  gSystem->Load("librecotasks");

  FairRunAna *fRun = new FairRunAna();

  TString base="demo";
  TString infile=base+".mc.root";
  TString outfile=base+".g.root";
  fRun->SetInputFile(infile);
  fRun->SetOutputFile(outfile);

  // setup the run for  use with Geane:
  // Geane object will call scripts to load Geant3
  // the object will not be used afterwards!
  FairGeane *Geane = new FairGeane(infile);

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
     
  
  // Setup a trackrep which uses geane
  FairGeanePro* gePro=new FairGeanePro();
  
  FairTrackParP dummy;

  TVector3 pos(1.0,0.5,0.0);
  TVector3 mom(0.5,0.1,1.);
  TVector3 poserr=TVector3(0.1,0.1,0.1);
  TVector3 momerr=TVector3(0.5,0.5,0.5);

  GFDetPlane initplane(TVector3(0.0,0.0,0.0),TVector3(1.,0.,0.),TVector3(0.,1.,0.));
  double q=-1.;
  
  
  FairTrackParP par(pos,mom,poserr,momerr,q,
		  			initplane.getO(),initplane.getU(),initplane.getV());
  par.Print();
  

   // The track representation:
  GeaneTrackRep* rep=new GeaneTrackRep(gePro,initplane,mom,poserr,momerr,q,211);
  rep->Print();
  rep->setPropDir(1);

  double z=20;
  // now exrapolate the parameters to another plane
  GFDetPlane plane(TVector3(0.1,0.1,z),TVector3(1.,0.,0.),TVector3(0.,1.,0.));

  TMatrixT<double> newpar(5,1);
  TMatrixT<double> newcov(5,5);
  TMatrixT<double> jac(5,5); // will be empty!

  rep->extrapolate(plane,newpar,newcov,jac);
  newpar.Print();
  newcov.Print();

  double x=newpar[3][0]+1.;
  double y=newpar[4][0]+1.;

  // now create a simple recohit:
  // the DemoRecoHit uses the GFDetPlane (0,0,z)(1,0,0)(0,1,0) -> u=x v=y

  AbsRecoHit* hit=new DemoSPHit(x,y,z,      // coordinates (u,v,z)
				0.01,0.01,0.01);   // errors (du,dv)

  GFDetPlane pl=hit->getDetPlane(rep);

  pl.Print();


  TCanvas *c1 = new TCanvas("c1");
  TView *view = TView::CreateView(1);
  view->SetRange(0,-10,10,20,10,30);

  TPolyLine3D *uline,*vline, *nline,*dirline;
  TPolyMarker3D *gplane;
  TPolyMarker3D *hits=new TPolyMarker3D(1);
  TPolyMarker3D *trackp=new TPolyMarker3D(1);
  trackp->SetMarkerStyle(20);

  hits->SetPoint(0,x,y,z);
  hits->SetMarkerStyle(23);

  pl.getGraphics(1,2,&gplane,&uline,&vline,&nline);

  uline->Draw();
  vline->Draw();
  nline->Draw();
  gplane->Draw();
  
  
  rep->extrapolate(pl,newpar,newcov,jac);
  
  newpar.Print();
  newcov.Print();
  pl.Print();

  rep->setState(newpar);
  rep->setCov(newcov);
  rep->setReferencePlane(pl);

  TVector3 trkp=rep->getPos();
  TVector3 dir=rep->getMom();
  dir.SetMag(5);
  TVector3 trkp2=trkp+dir;

  dirline=new TPolyLine3D(2);
  dirline->SetPoint(0,trkp.X(),trkp.Y(),trkp.Z());
  dirline->SetPoint(1,trkp2.X(),trkp2.Y(),trkp2.Z());

  dirline->Draw();
  
  // set extroplated track position
  trackp->SetPoint(0,trkp.X(),trkp.Y(),trkp.Z());
  hits->Draw();
  trackp->Draw();

  TMatrixT<double> res=hit.residualVector(rep,newpar);
  res->Print();
  // Note that the residual calcualtion already assumes
  // the track to be extrapolated to the right frame!



}  
  
