{
 // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


   gSystem->Load("libEnDrc");          // our library



  TFile* f = new TFile("testsimu10.root");
  //  TFile* f = new TFile("sim_endrcP.root");
  TTree *t=f->Get("pndsim") ;
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
 	t->GetEntry(j);
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

