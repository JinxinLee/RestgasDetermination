int ReadLines(TString LinFile, TString MCFile){

	// load libs

	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

//	TString LinFile = "/hiskp1/bianco/Cracow/Fit_5.000000_11_9000.root";
//	TString MCFile = "/hiskp1/bianco/Cracow/Sim_5.000000_11_9000.root";

	TFile *f = new TFile(LinFile);

	TTree *t=(TTree *) f->Get("pndsim") ;

	TClonesArray* tr_array=new TClonesArray("TtFitRes");
	t->SetBranchAddress("TTFit",&tr_array);//Branch names

	TFile *fMC = new TFile(MCFile);

	TTree *tMC=(TTree *) fMC->Get("pndsim") ;

	TClonesArray* mc_array=new TClonesArray("PndSdsMCPoint");
	tMC->SetBranchAddress("LMDPoint",&mc_array);//Branch names

	if (t->GetEntries() != tMC->GetEntries()) break;

    	cout << "Events: " << t->GetEntries() << endl;

	Double_t Sx=0.,Sy=0.,Ox=0.,Oy=0.;

	TH2F *offsets = new TH2F("Residuals","Residuals",5000,-1.,+1.,5000,-1.,+1.);

	TVector3 DevPoint, FitPoint;

	TCanvas *can = new TCanvas("2D","2D",0,0,1200,600);
	TCanvas *can2 = new TCanvas("1D","1D",0,0,1200,400);

	can->Divide(2,1);
	can2->Divide(2,1);

	for (Int_t j = 0 ; j < t->GetEntries() ; j++) // loop on events
	  {

	    t->GetEvent(j);
	    tMC->GetEvent(j);

	    cout << "Event " << j << " Entries " << tr_array->GetEntries() << endl;

  	    if ( (mc_array->GetEntries()!=1) || (tr_array->GetEntries()!=1) ) continue;

	    PndSdsMCPoint *point = (PndSdsMCPoint*) mc_array->At(0);
	    TtFitRes* line = (TtFitRes*)tr_array->At(0);

	    DevPoint = 0.5*(point->GetPosition() + point->GetPositionOut());

	    Sx = (line->GetDirectionVec()).X(); // par[1]
	    Sy = (line->GetDirectionVec()).Y(); // par[3]

	    Ox = (line->GetFirstPoint()).X(); // par[0]
	    Oy = (line->GetFirstPoint()).Y(); // par[2]

	    FitPoint.SetXYZ((Ox+Sx*(DevPoint.Z())),(Oy+Sy*(DevPoint.Z())),DevPoint.Z());

	    offsets->Fill( (FitPoint.X()-DevPoint.X()) , (FitPoint.Y()-DevPoint.Y()) );

	   } // end loop on events

	offsets->GetXaxis()->SetRangeUser(-10*offsets->GetRMS(1),+10*offsets->GetRMS(1));
	offsets->GetYaxis()->SetRangeUser(-10*offsets->GetRMS(2),+10*offsets->GetRMS(2));

	offsets->GetXaxis()->SetTitle("#DeltaX [cm]");
	offsets->GetXaxis()->SetTitle("#DeltaY [cm]");

	TString Res2D = LinFile;
	Res2D.Remove(Res2D.Sizeof()-6);
	Res2D = Res2D+"_2DResiduals.png";

	TString Res1D = LinFile;
	Res1D.Remove(Res1D.Sizeof()-6);
	Res1D = Res1D+"_1DResiduals.png";

	gStyle->SetPalette(1.);

	can->cd(1);

	offsets->Draw("COLZ");

	can->cd(2);

	offsets->Draw("LEGO2");

	can->SaveAs(Res2D.Data(),"png");



	TH1D *resX = offsets->ProjectionX("_Xproj");
	resX->SetTitle("X Redisuals");
	resX->GetXaxis()->SetTitle("#DeltaX [cm]");


	TH1D *resY = offsets->ProjectionY("_Yproj");
	resY->SetTitle("Y Redisuals");
	resY->GetXaxis()->SetTitle("#DeltaY [cm]");

	gStyle->SetOptFit(1);

	can2->cd(1);

	resX->Draw();
	resX->Fit("gaus");

	can2->cd(2);

	resY->Draw();
	resY->Fit("gaus");

	can2->SaveAs(Res1D.Data(),"png");
  return 0;
}

