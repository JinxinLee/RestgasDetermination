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
        gSystem->Load("libGeaneExEmc");
        gSystem->Load("libTrkBase");

	TFile *f=new TFile("geane.root");
	TTree *simtree=f->Get("pndsim") ;

        fTrackParGeane = new TClonesArray("FairTrackParH");
 	fTrackParIni = new TClonesArray("FairTrackParH");
  	fTrackParFinal = new TClonesArray("FairTrackParH");

        TH1F *h1=new TH1F("h1","Phi ",100,-10.,10.);
	TH1F *h2=new TH1F("h2","Lambda",100,-10,10);
	TH1F *h3=new TH1F("h3","Qp",100,-10.,10.);
	TH1F *h4=new TH1F("h4","Y",100,-10,10);
	TH1F *h5=new TH1F("h5","Z",100,-10,10);
	TH1F *h6=new TH1F("h6","Px",100, -10,10);
	TH1F *h7=new TH1F("h7","Py",100,-10,10);
	TH1F *h8=new TH1F("h8","Pz",100,-10,10);
        TH1F *h9=new TH1F("h9","Ysc",100,-10,10);
	TH1F *h10=new TH1F("h10","Ysc",100,-10,10);
	TH1F *h11=new TH1F("h11","Zsc",100,-10,10);

	TH2F *h101=new TH2F("h101","X_MC vs X_GE",100,-100,100,100,-100,100);
	TH2F *h102=new TH2F("h102","Y_MC vs Y_GE",100,-100,100,100,-100,100);
	TH2F *h103=new TH2F("h103","Z_MC vs Z_GE",100,-150,250,100,-150,250);

	TH2F *h201=new TH2F("h201","PX_MC vs PX_GE",100,-2,2,100,-2,2);
	TH2F *h202=new TH2F("h202","PY_MC vs PY_GE",100,-2,2,100,-2,2);
	TH2F *h203=new TH2F("h203","PZ_MC vs PZ_GE",100,-2,2,100,-2,2);

        simtree->SetBranchAddress("GeaneTrackFinal",&fTrackParFinal);
	simtree->SetBranchAddress("GeaneTrackPar",&fTrackParGeane);
        FairTrackParH *fTrkF  ;
        FairTrackParH *fTrkG;
        Int_t Nevents= simtree->GetEntriesFast();
	cout<<Nevents<<endl;
	for(Int_t i=0; i<Nevents; i++){
		fTrackParGeane->Delete();
		fTrackParFinal->Delete();
		simtree->GetEntry(i);
		 for (Int_t k=0; k<fTrackParGeane->GetEntriesFast(); k++)	{
    			fTrkF = (FairTrackParH *)fTrackParFinal->At(k);
			fTrkG = (FairTrackParH *)fTrackParGeane->At(k);
			if(fTrkF &&fTrkG ){

			  h101->Fill(fTrkF->GetX(),fTrkG->GetX());
			  h102->Fill(fTrkF->GetY(),fTrkG->GetY());
			  h103->Fill(fTrkF->GetZ(),fTrkG->GetZ());

			  h201->Fill(fTrkF->GetPx(),fTrkG->GetPx());
			  h202->Fill(fTrkF->GetPy(),fTrkG->GetPy());
			  h203->Fill(fTrkF->GetPz(),fTrkG->GetPz());

			if(fTrkG->GetDPhi())h1->Fill((fTrkF->GetPhi()-fTrkG->GetPhi())/fTrkG->GetDPhi());
		        if(fTrkG->GetDLambda())h2->Fill((fTrkF->GetLambda()-fTrkG->GetLambda())/fTrkG->GetDLambda());
			if(fTrkG->GetDQp())h3->Fill((fTrkF->GetQp()-fTrkG->GetQp())/fTrkG->GetDQp());
                        if(fTrkG->GetDY())h4->Fill((fTrkF->GetY() -fTrkG->GetY()) /fTrkG->GetDY());
                        if(fTrkG->GetDZ())h5->Fill((fTrkF->GetZ() -fTrkG->GetZ()) /fTrkG->GetDZ());
			if(fTrkG->GetDPx())h6->Fill((fTrkF->GetPx()-fTrkG->GetPx())/fTrkG->GetDPx());
			if(fTrkG->GetDPy())h7->Fill((fTrkF->GetPy()-fTrkG->GetPy())/fTrkG->GetDPy());
			if(fTrkG->GetDPz())h8->Fill((fTrkF->GetPz()-fTrkG->GetPz())/fTrkG->GetDPz());
                        if(fTrkG->GetDY_sc())h9->Fill((fTrkF->GetY_sc()-fTrkG->GetY_sc())/fTrkG->GetDY_sc());
			if(fTrkG->GetDY_sc())h10->Fill((fTrkF->GetY_sc()-fTrkG->GetY_sc())/fTrkG->GetDY_sc());
			if(fTrkG->GetDZ_sc())h11->Fill((fTrkF->GetZ_sc()-fTrkG->GetZ_sc())/fTrkG->GetDZ_sc());
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
