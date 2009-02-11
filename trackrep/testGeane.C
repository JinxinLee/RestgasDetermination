{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libgeant321");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");

  gSystem->Load("libTrkBase");
  gSystem->Load("libGeane");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");


  FairRunAna *fRun= new FairRunAna();
  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,20. ); // values are in kG
  fMagField->SetFieldRegion(-500, 500,-500, 500, -200, 200);
  fRun->SetField(fMagField);


  Double_t v=1.;
  Double_t w=1.;

  Double_t Tv=0.1;
  double Tw=0.1;
  double qp=-1; 
  double cov[15];
  TVector3 o(0.1,0.1,0.1);
  TVector3 u(1.,0.,0.);
  TVector3 k(0.,1.,0.);

  double spu=1;

  FairTrackParP par(v,w,Tv,Tw,qp,cov,o,u,k);

  par.Print();

}
