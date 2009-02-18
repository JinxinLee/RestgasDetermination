{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libGen");
  gSystem->Load("libField");
  
  gSystem->Load("libPassive");
  gSystem->Load("libStt"); 
  gSystem->Load("libMdt");
  gSystem->Load("libEmc");  
  gSystem->Load("libTof");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libtpc");
  gSystem->Load("libDch");
  gSystem->Load("libMvd");
 
  gSystem->Load("libGeom.so");

  TFile* f = new TFile("testsimu10.root");
  TTree *t=f->Get("cbmsim") ;
  TClonesArray *fT=new TClonesArray("TGeoTrack");

  t->SetBranchAddress("GeoTracks",&fT) ;

 TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
 TCanvas* c1 = new TCanvas("glcanvas", "openGL", 100, 100, 800, 800);
 c1->SetFillColor(10);

 
// geoMan->SetVisLevel(10);   
 geoMan->GetTopVolume()->Draw("ogl");
 
 TGeoTrack *tr;
 TObjArray *TrList= geoMan->GetListOfTracks(); 
   geoMan->SetAnimateTracks();
  for (Int_t j=0; j< t->GetEntriesFast(); j++)	{
 	t->GetEntry(0);
        Double_t *point;
	for (Int_t i=0; i<fT->GetEntriesFast(); i++)	{
    		tr=(TGeoTrack *)fT->At(i);
                Int_t Np=tr->GetNpoints();
                FairVTrack *pt = new FairVTrack(Np); 
                pt->SetLineColor(tr->GetLineColor());
                pt->SetLineWidth(2);
                pt->SetTrack(tr);
                pt->SetParticle((TParticle *)tr->GetParticle());
                for (Int_t n=0; n<Np; n++){
                   point=tr->GetPoint(n);
                   pt->SetPoint(n,point[0],point[1],point[2]);
		}
                pt->Draw();
	}
  }
}

