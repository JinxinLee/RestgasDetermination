void SetHistoSettings(TH1* histo);
void SetCanvasSettings(TCanvas* can);
void SetHistoSettings(TH2* histo);

void histogramme() {

	 int  nEvents = 10000;
	 gROOT->Reset();

	 // -----  Load libraries   ------------------------------------------------
	 gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	 rootlogon();
//	 gROOT->LoadMacro("../Tools.C");
// 	 LoadPandaStyle();
	 gROOT->SetStyle("Plain");

	 // -----   Timer   --------------------------------------------------------
	 TStopwatch timer;
	 timer.Start();
	 // ------------------------------------------------------------------------

	 std::string inFile = "sim_complete.root";
	 std::string digiFile = "digi_complete.root";
	 std::string recoFile = "reco_complete.root";
	 TString picture = "digi_complete.root";

	 TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
	 TTree *t=(TTree *) f->Get("cbmsim") ;
	 t->AddFriend("cbmsim",digiFile.c_str()); // the digi file
	 t->AddFriend("cbmsim",recoFile.c_str()); // the reco file

	 TClonesArray* mcPixel_array=new TClonesArray("PndSdsMCPoint");
	 t->SetBranchAddress("MVDPoint",&mcPixel_array);//Branch names

	 TClonesArray* digiPixel_array=new TClonesArray("PndSdsDigiPixel");
	 t->SetBranchAddress("MVDPixelDigis",&digiPixel_array);//Branch names

	 TClonesArray* recoPixel_array=new TClonesArray("PndSdsDigiPixel");
	 t->SetBranchAddress("MVDDigisCorr",&recoPixel_array);//Branch names

	 gStyle->SetOptStat("ne");
	 gStyle->SetOptFit(1111);

	 //----- Histogramme -----

	 TH2F* hisTimeWalk = new TH2F("hisTimeWalk","DigiTimeWalk and RecoTimeWalk;  Digi [ns];(Digi-Reco)/Digi ", 440,-0.5,109.5,440, -0.1,0.1);
//	 TH2F* hisTimeWalk = new TH2F("hisTimeWalk","DigiTimeWalk and RecoTimeWalk; TimeWalk Reco [ns]; TimeWalk Digi [ns]", 440, -0.5,109.5,440,-0.5,109.5  );
	 TH2F* hisTimeWalkB = new TH2F("hisTimeWalkB","DigiTimeWalk and RecoTimeWalk;  Digi [ns];(Digi-Reco)/Digi ", 440,-0.5,109.5,440, -0.01,0.1 );
	 TH2F* hisTimeWalkvs = new TH2F("hisTimeWalkvs","DigiTimeWalk and RecoTimeWalk;  Digi [ns];Reco [ns] ", 1000,0,1000,1000,0,1000 );

	 TH1F* hisTimeWalkRelError = new TH1F("hisTimeWalkRelError","Relativ Error of Reco TimeWalk;(Digi-Reco)/Digi;#",100,-0.1.,0.1.);
	 TH1F* hisTimeWalkAbsError = new TH1F("hisTimeWalkAbsError","Absolut Error of Reco TimeWalk;(Digi-Reco) [ns];#",100,-10.,10.);

	 TH1F* hisTimeStampReco = new TH1F("hisTimeStampReco", "TimeStamp after TimeWalk correction;TimeStamp [ns];#", 200, 0., 50.);


	 TH2F* hisTimeStamp = new TH2F("hisTimeStamp","DigiTimeStamp and RecoTimeStamp; Digi [ns]; Reco[ns]",220,-0.5,109.5,220,-0.5,109.5 );

	 TH2F* hisTSvTOF = new TH2F("hisTSvTOF","RecoTimeStamp vs MC Tof;MC Tof [ns]; RecoTimeStamp [ns] ",200,0,50, 200, 0,50);

	 TH2F* hisTSbeforeaftervCharge = new TH2F("hisTSbefore-aftervCharge","(DigiTimeStamp-RecoTimeStamp) vs Charge;Digi.fCharge [TOT [ns]]; (DigiTimeStamp-RecoTimeStamp) [ns] ",300,0,3000, 200, 0,100);

	 TH1F* his1DTSbeforeaftervCharge = new TH1F("his1DTSbeforeaftervCharge","(DigiTimeStamp-RecoTimeStamp) vs Charge;Digi.fCharge [TOT [ns]]; (DigiTimeStamp-RecoTimeStamp) [ns] ", 303, -30,3000);

	 TH2F* hisTSbeforeaftervCharge2 = new TH2F("hisTSbefore-aftervCharge2","(DigiTimeStamp-RecoTimeStamp) vs Charge;Reco.fCharge [e]; (DigiTimeStamp-RecoTimeStamp) [ns] ",300,0,30000, 200, 0,100);

	 TH1F* his1DTSbeforeaftervCharge2 = new TH1F("his1DTSbeforeaftervCharge2","(DigiTimeStamp-RecoTimeStamp) vs Charge;Reco.fCharge [e]; (DigiTimeStamp-RecoTimeStamp) [ns] ", 303, -30,30000);

	 //
//	 hisTimeWalk->GetXaxis()->SetTitleOffset(1);  // LabelOffset rueckt die Skala weg
//	 hisTimeWalk->GetYaxis()->SetTitleOffset(0.65);





	// hisTimeWalk->SetOption("colz");
	 gStyle->SetPalette(1);

	 for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
	 {
		 t->GetEntry(j);
//		 cout<<"Event No "<<j<<endl;


		 for (Int_t i=0; i<digiPixel_array->GetEntriesFast(); i++)
		 {

			 PndSdsMCPoint *pixelmc = (PndSdsMCPoint*) mcPixel_array->At(i);
			 PndSdsDigiPixel *pixeldigi = (PndSdsDigiPixel*) digiPixel_array->At(i);
			 PndSdsDigiPixel *pixelreco = (PndSdsDigiPixel*) recoPixel_array->At(i);
			 hisTimeWalk->Fill(pixeldigi->GetTimeWalk(),(pixeldigi->GetTimeWalk()-pixelreco->GetTimeWalk())/pixeldigi->GetTimeWalk());
			 hisTimeWalkRelError->Fill((pixeldigi->GetTimeWalk()-pixelreco->GetTimeWalk())/pixeldigi->GetTimeWalk());
			 hisTimeWalkAbsError->Fill((pixeldigi->GetTimeWalk()-pixelreco->GetTimeWalk()));
			 hisTimeWalkB->Fill(pixeldigi->GetTimeWalk(),TMath::Abs(pixeldigi->GetTimeWalk()-pixelreco->GetTimeWalk())/pixeldigi->GetTimeWalk());

			 hisTimeWalkvs->Fill(pixeldigi->GetCharge()/100,pixelreco->GetCharge()/100);

			 hisTimeStampReco->Fill(pixelreco->GetTime());
			 hisTimeStamp->Fill(pixeldigi->GetTime(),pixelreco->GetTime());  //sinnfreies histogramm.
			 hisTSvTOF->Fill(pixeldigi->GetTof(),pixelreco->GetTime());
			 hisTSbeforeaftervCharge->Fill(pixeldigi->GetCharge(),(pixeldigi->GetTime()-pixelreco->GetTime()));
			 his1DTSbeforeaftervCharge->SetBinContent(his1DTSbeforeaftervCharge->FindBin(pixeldigi->GetCharge()),(pixeldigi->GetTime()-pixelreco->GetTime()));

			 hisTSbeforeaftervCharge2->Fill(pixelreco->GetCharge(),(pixeldigi->GetTime()-pixelreco->GetTime()));
			 his1DTSbeforeaftervCharge2->SetBinContent(his1DTSbeforeaftervCharge->FindBin(pixelreco->GetCharge()),(pixeldigi->GetTime()-pixelreco->GetTime()));

		 }


	 }




	 TFile *output=new TFile("ergebnis.root","RECREATE");
	 //  output->Open();

	 //	 hisTimeWalk->Write();
	 //hisTimeWalkRelError->Write();
	 //hisTimeWalkAbsError->Write();
	 //hisTimeWalkB->Write();
	 //hisTimeWalkvs->Write();
	 hisTSbeforeaftervCharge->Write();
	 hisTSbeforeaftervCharge2->Write();
	 //his1DTSbeforeaftervCharge->Write();
	 output->Write();
	 output->Close();


	 TF1* myfit = new TF1("myfit","[0]*1/(x)");
	 myfit->SetRange(0,30000);
	 //myfit->SetParameter(2,1);
	 myfit->SetLineWidth(1);
	 myfit->SetLineStyle(6);

	hisTSbeforeaftervCharge2->Fit("myfit");

	 TF1* myfit2 = new TF1("myfit2","[0]*1/((x/[1])-([2]/x)+[3])");
	 myfit2->SetRange(0,30000);
	 myfit2->SetParameter(0,1);
	 myfit2->SetParameter(1,1);
	 myfit2->SetParameter(2,1);
	 myfit2->SetParameter(3,1);
	 myfit2->SetLineWidth(1);
	 myfit2->SetLineStyle(6);
	 hisTSbeforeaftervCharge->Fit("myfit2");

//	 TCanvas* can1 = new TCanvas("MvdTestPlot","MCHit view in MVD",0,0,a*res,b*res);

	 Int_t a=2;
	 Int_t b=3;
	 Int_t res=140;

	 //	 TCanvas* can1 = new TCanvas("TimeWalk","TimeWalk",0,0,a*res,b*res);
	 TCanvas* can2 = new TCanvas("TimeStamp","TimeStamp",100,100,a*res,b*res);

//	 SetCanvasSettings(can1);
	// SetCanvasSettings(can2);
	 //	 can1->Divide(2,2);
	 can2->Divide(1,2);

	 //	 can1->cd(1);
	 //	 gPad->SetGrid();
//	 SetHistoSettings(hisTimeWalk);
//	 hisTimeWalk->Draw("colz");

//	 can1->cd(2);
//	 gPad->SetGrid();
//	 SetHistoSettings(hisTimeWalkB);hisTSbefore-aftervCharge
//	 hisTimeWalkB->Draw("colz");

//	 can1->cd(2);
//	 gPad->SetGrid();
//	 SetHistoSettings(hisTimeWalkvs);
//	 hisTimeWalkvs->Draw("colz");

//	 can1->cd(3);
//	 gPad->SetGrid();
//	 SetHistoSettings(hisTimeWalkRelError);

//	 hisTimeWalkRelError->Draw();

//	 can1->cd(4);
//	 gPad->SetGrid();
//	 SetHistoSettings(hisTimeWalkAbsError);

//	 hisTimeWalkAbsError->Draw();


//	 can2->cd(1);
//	 gPad->SetGrid();
//	 SetHistoSettings(hisTimeStamp);
//	 hisTimeStamp->Draw("colz");


//	 gStyle->SetOptStat("");
//	 gStyle->SetOptFit(0);
//
//	 can2->cd(4);
//	 gPad->SetGrid();
//	 SetHistoSettings(hisTSvTOF);
//	 hisTSvTOF->Draw("colz");
//
//
//
//	 can2->cd(3);
//	 gPad->SetGrid();
//	 SetHistoSettings(hisTimeStampReco);
//	 hisTimeStampReco->Draw();

	 can2->cd(1);

	 TLegend *l = new TLegend(0.6344775,0.5806052,0.8650102,0.6581101,NULL,"brNDC");
	 l->SetFillColor(10);
	 l->SetTextFont(42);
	 l->SetTextSize(0.03);
	 l->SetBorderSize(1);
	 l->SetLineWidth(1);
	 l->AddEntry(myfit,"Function #frac{1}{x}*p0","L");


	 gPad->SetGrid();
	 SetHistoSettings(hisTSbeforeaftervCharge2);

	  hisTSbeforeaftervCharge2->Draw("colz");

	 l->Draw();
	 gPad->Update();
	 TPaveStats *st1= (TPaveStats*)hisTSbeforeaftervCharge2->FindObject("stats");
	 st1->SetX1NDC(0.5407843);
	 st1->SetX2NDC(0.9006462);
	 st1->SetY1NDC(0.6597222);
	 st1->SetY2NDC(0.8990385);

	 gPad->Print("hisTSbeforeaftervCharge-mitfit-recocharge.pdf","pdf");

	 can2->cd(2);

		TLegend *l2 = new TLegend(0.6351481,0.5432692,0.8966477,0.6501068,NULL,"brNDC");
		l2->SetFillColor(10);
		l2->SetTextFont(42);
		l2->SetTextSize(0.03);
		l2->SetBorderSize(1);
		l2->SetLineWidth(1);

		l2->AddEntry(myfit2,"Function p0(#frac{1}{#frac{x}{p1}-#frac{p2}{x}+p3})","l");
	 gPad->SetGrid();

	 SetHistoSettings(hisTSbeforeaftervCharge);

	  	 hisTSbeforeaftervCharge->Draw("colz");

	 l2->Draw();
	 gPad->Update();
	 TPaveStats *st2= (TPaveStats*)hisTSbeforeaftervCharge->FindObject("stats");
	 st2->SetX1NDC(0.5383853);
	 st2->SetX2NDC(0.8982471);
	 st2->SetY1NDC(0.6586538);
	 st2->SetY2NDC(0.8979701);




	 gPad->Modified();
	 can2->Update();
	 // -----   Finish   -------------------------------------------------------
	 timer.Stop();
	 Double_t rtime = timer.RealTime();
	 Double_t ctime = timer.CpuTime();
	 cout << endl << endl;
	 cout << "Macro finished succesfully." << endl;
	 cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	 cout << endl;
	 // ------------------------------------------------------------------------


	 //  exit(0);
}

void SetHistoSettings(TH1* histo) {


	histo->GetXaxis()->SetLabelFont(40);
	histo->GetXaxis()->SetTitleFont(40);
	histo->GetYaxis()->SetLabelFont(40);
	histo->GetYaxis()->SetTitleFont(40);



}
void SetHistoSettings(TH2* histo) {


	histo->GetXaxis()->SetLabelFont(40);
	histo->GetXaxis()->SetTitleFont(40);
	histo->GetYaxis()->SetLabelFont(40);
	histo->GetYaxis()->SetTitleFont(40);



}
void SetCanvasSettings(TCanvas* can) {


	 can->SetBorderSize(2);
	 can->SetGrid();
	 can->SetLeftMargin(0.07);
	 can->SetRightMargin(0.040);
	 can->SetTopMargin(0.09);
	 can->SetFrameFillColor(10);
	 can->SetFillColor(10);
	 can->SetFrameLineWidth(2);



}
