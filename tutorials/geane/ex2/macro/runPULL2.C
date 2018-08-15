{
	gROOT->Reset();
	gStyle->SetOptFit(1);


	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();

	// Load this example libraries
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
    	gSystem->Load("libField");
	gSystem->Load("libPndData");
	gSystem->Load("libGeane");
  	gSystem->Load("libPlane");
        gSystem->Load("libGeaneEx");
        gSystem->Load("libTrkBase");

	TFile *f=new TFile("ex2g.root");
	TTree *simtree=f->Get("pndsim") ;

        fTrackParGeane = new TClonesArray("FairTrackParP");
 	fTrackParIni = new TClonesArray("FairTrackParP");
  	fTrackParFinal = new TClonesArray("FairTrackParP");

	TH1F *h1=new TH1F("h1","V ",100,-10.,10.);
	TH1F *h2=new TH1F("h2","W",100,-10,10);
	TH1F *h3=new TH1F("h3","TV",100,-10,10);
	TH1F *h4=new TH1F("h4","TW",100,-10,10);
	TH1F *h5=new TH1F("h5","Qp",100,-10,10);
	TH1F *h6=new TH1F("h6","Y",100, -10,10);
	TH1F *h7=new TH1F("h7","Z",100,-10,10);
	TH1F *h8=new TH1F("h8","Px",100,-10,10);
	TH1F *h9=new TH1F("h9","Py",100,-10,10);
	TH1F *h10=new TH1F("h10","Pz",100,-10,10);

	simtree->SetBranchAddress("GeaneTrackFinal",&fTrackParFinal);
	simtree->SetBranchAddress("GeaneTrackPar",&fTrackParGeane);
	FairTrackParP *fTrkF  ;
	FairTrackParP *fTrkG;

	Int_t Nevents= simtree->GetEntriesFast();
	for(Int_t i=0; i<Nevents; i++){
 	  fTrackParGeane->Delete();
 	  fTrackParFinal->Delete();
	  simtree->GetEntry(i);
	  for (Int_t k=0; k<fTrackParGeane->GetEntriesFast(); k++)	{
	    fTrkF = (FairTrackParP *)fTrackParFinal->At(k);
	    fTrkG = (FairTrackParP *)fTrackParGeane->At(k);
	    if(!fTrkF || !fTrkG) continue;
	    if(fTrkF &&fTrkG ){
    if(fTrkG->GetDV())  h1->Fill((fTrkF->GetV()  - fTrkG->GetV())/fTrkG->GetDV());
 	      if(fTrkG->GetDW())  h2->Fill((fTrkF->GetW()  - fTrkG->GetW())/fTrkG->GetDW());
	      if(fTrkG->GetDTV()) h3->Fill((fTrkF->GetTV() - fTrkG->GetTV())/fTrkG->GetDTV());
	      if(fTrkG->GetDTW()) h4->Fill((fTrkF->GetTW() - fTrkG->GetTW())/fTrkG->GetDTW());
	      if(fTrkG->GetDQp())h5->Fill((fTrkF->GetQp() - fTrkG->GetQp()) /fTrkG->GetDQp());
 	      if(fTrkG->GetDY()) h6->Fill((fTrkF->GetY()  - fTrkG->GetY()) /fTrkG->GetDY());
 	      if(fTrkG->GetDZ()) h7->Fill((fTrkF->GetZ()  - fTrkG->GetZ()) /fTrkG->GetDZ());
 	      if(fTrkG->GetDPx()) h8->Fill((fTrkF->GetPx() - fTrkG->GetPx())/fTrkG->GetDPx());
 	      if(fTrkG->GetDPy()) h9->Fill((fTrkF->GetPy() - fTrkG->GetPy())/fTrkG->GetDPy());
 	      if(fTrkG->GetDPz()) h10->Fill((fTrkF->GetPz()- fTrkG->GetPz())/fTrkG->GetDPz());
	    }
	  }
 	}
	TCanvas *c=new TCanvas("c","c",900,600);
	c->Divide(5,2);
	c->cd(1);
	h1->Draw();
	h1-> Fit("gaus");
	c->cd(2);
	h2->Draw();
	h2-> Fit("gaus");
	c->cd(3);
	h3->Draw();
	h3-> Fit("gaus");
	c->cd(4);
	h4->Draw();
	h4-> Fit("gaus");
	c->cd(5);
	h5->Draw();
	h5-> Fit("gaus");
	c->cd(6);
	h6->Draw();
	h6-> Fit("gaus");
	c->cd(7);
	h7->Draw();
	h7-> Fit("gaus");
	c->cd(8);
	h8->Draw();
	h8-> Fit("gaus");
	c->cd(9);
	h9->Draw();
	h9-> Fit("gaus");
	c->cd(10);
	h10->Draw();
	h10-> Fit("gaus");


 	c->cd();

}
