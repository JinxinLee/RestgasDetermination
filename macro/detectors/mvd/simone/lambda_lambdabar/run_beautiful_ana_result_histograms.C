void ctauhistogramm(TH1F* histo, TString savefile)
{
	  histo->SetLineColor(kBlack);
	  histo->Draw("");
	  TF1 *fExpo = new TF1("fExpo","expo",0,100);
	  fExpo->SetLineColor(kRed);
	  fExpo->SetLineWidth(2);
	  histo->Fit("fExpo","RQ");
	  //printf("Slope %f", fExpo->GetParameter("Slope"));
	  fExpo->GetParameter("Slope");



	  std::cout << std::setw(15) << ::std::setprecision( 4 )<< 1/fExpo->GetParameter("Slope") ;
	  std::cout << std::setw(15)<<  ::std::setprecision( 4 ) << fExpo->GetParError(fExpo->GetParNumber("Slope"));
	  std::cout << std::setw(15)<<  ::std::setprecision( 4 ) << fExpo->GetChisquare();
	  std::cout << std::setw(5)<<  fExpo->GetNDF() ;
	  std::cout << std::setw(15)<<  "="<<fExpo->GetChisquare()/fExpo->GetNDF() ;
	  std::cout << std::setw(15)<< ::std::setprecision( 4 ) << histo->GetNbinsX() << std::endl;

	  TPaveText *pt = new TPaveText(44.5781,59.32019,104.1472,206.744,"");
	  pt->UseCurrentStyle();
	  pt->SetFillColor(kWhite);
	  pt->SetBorderSize(1);
	  TString myString;

	  std::stringstream virtualString;
	  virtualString<< "c#tau = ";
	  virtualString<< TMath::Abs(1/fExpo->GetParameter("Slope"));
	  virtualString<< "#pm";

	  virtualString<<TMath::Abs(1/fExpo->GetParameter("Slope"))*TMath::Abs(1/fExpo->GetParameter("Slope"))*fExpo->GetParError(fExpo->GetParNumber("Slope"));
	  virtualString<< " cm";
	  TString bTagisGreaterThanSTRING = virtualString.str();

	  pt->AddText(bTagisGreaterThanSTRING.Data() );

	  pt->Draw();

	  gPad->SetLogy();
	  //gPad->Print(savefile,"pdf");  // nicht fuer powerpoint geeignet
	//  gPad->Print(savefile,"eps");
	//  gPad->Print("result_pictures/fMeanLife.gif","gif");  // ganz gut, aber keine Vektorgrafik
	//  gPad->Print("result_pictures/fMeanLife.png","png");  // ganz gut, aber keine Vektorgrafik
	//  gPad->Print("result_pictures/fMeanLife.jpg","jpg");  // schlechte qualitaet
}

void DrawMVD()
{
	Int_t linewidth=3;

	TLine *Disk1 = new TLine(2,0,2,3.656);
	Disk1->SetLineWidth(linewidth);
	Disk1->Draw();

	TLine *Disk2 = new TLine(4,0,4,3.656);
	Disk2->SetLineWidth(linewidth);
	Disk2->Draw();

	TLine *Disk3 = new TLine(7,0,7,7.393);
	Disk3->SetLineWidth(linewidth);
	Disk3->Draw();

	TLine *Disk4 = new TLine(10,0,10,7.393);
	Disk4->SetLineWidth(linewidth);
	Disk4->Draw();

	TLine *Disk5 = new TLine(16,0,16,13.115);
	Disk5->SetLineWidth(linewidth);
	Disk5->Draw();

	TLine *Disk6 = new TLine(23,0,23,13.115);
	Disk6->SetLineWidth(linewidth);
	Disk6->Draw();

	TLine *Barrel1 = new TLine(0,2.5,0.98,2.5);
	Barrel1->SetLineWidth(linewidth);
	Barrel1->Draw();

	TLine *Barrel2 = new TLine(0,5,5.78,5);
	Barrel2->SetLineWidth(linewidth);
	Barrel2->Draw();

	TLine *Barrel3 = new TLine(0,9.5,13.9,9.5);
	Barrel3->SetLineWidth(linewidth);
	Barrel3->Draw();

	TLine *Barrel4 = new TLine(0,13.5,13.9,13.5);
	Barrel4->SetLineWidth(linewidth);
	Barrel4->Draw();
}

void DrawLambdaDiskPosition0()
{
	Int_t linewidth=3;

	TLine *LambdaDisk1 = new TLine(40,0,40,13.115);
	LambdaDisk1->SetLineWidth(linewidth);
	LambdaDisk1->Draw();

	TLine *LambdaDisk2 = new TLine(60,0,60,13.115);
	LambdaDisk2->SetLineWidth(linewidth);
	LambdaDisk2->Draw();

}

void DrawLambdaDiskPosition1()
{
	Int_t linewidth=3;

	TLine *LambdaDisk1 = new TLine(37,0,37,13.115);
	LambdaDisk1->SetLineWidth(linewidth);
	LambdaDisk1->Draw();

	TLine *LambdaDisk2 = new TLine(43,0,43,13.115);
	LambdaDisk2->SetLineWidth(linewidth);
	LambdaDisk2->Draw();

}

void DrawLambdaDiskPosition2()
{
	Int_t linewidth=3;

	TLine *LambdaDisk1 = new TLine(57,0,57,13.115);
	LambdaDisk1->SetLineWidth(linewidth);
	LambdaDisk1->Draw();

	TLine *LambdaDisk2 = new TLine(63,0,63,13.115);
	LambdaDisk2->SetLineWidth(linewidth);
	LambdaDisk2->Draw();

}

void DrawLambdaDiskPosition3()
{
	Int_t linewidth=3;

	TLine *LambdaDisk1 = new TLine(77,0,77,13.115);
	LambdaDisk1->SetLineWidth(linewidth);
	LambdaDisk1->Draw();

	TLine *LambdaDisk2 = new TLine(83,0,83,13.115);
	LambdaDisk2->SetLineWidth(linewidth);
	LambdaDisk2->Draw();

}

int run_beautiful_ana_result_histograms(TString FileName="test"){
  // Input file (MC events)

  gROOT->Reset();
 

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat("e");
  gStyle->SetOptFit(1111);
  gStyle->SetPalette(1);

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  PndFileNameCreator creator(FileName.Data());

  std::cout << "creator.GetCustomFileName('ana')" << creator.GetCustomFileName("ana") << std::endl;
  std::string inFile = creator.GetCustomFileName("ana");

  TFile* f = new TFile(inFile.c_str());

  //===========

  TCanvas* canvas = new TCanvas("canvas", "Results");

  //===========

  // canvas->cd(1);
//  TH1I* fTracksPerEventVer1 =(TH1I*)f->Get("fTracksPerEventVer1");
//  fTracksPerEventVer1->SetMaximum(6500);
//  fTracksPerEventVer1->Draw("");
//  gPad->Print("result_pictures/fTracksPerEventVer1.pdf","pdf");
//  gPad->Print("result_pictures/fTracksPerEventVer1.png","png");
//
//  TH1I* fTracksPerEventVer2 =(TH1I*)f->Get("fTracksPerEventVer2");
//  fTracksPerEventVer2->Draw("");
//  fTracksPerEventVer2->SetMaximum(6500);
//  fTracksPerEventVer2->Draw("");
//  gPad->Print("result_pictures/fTracksPerEventVer2.pdf","pdf");
//  gPad->Print("result_pictures/fTracksPerEventVer2.png","png");
//
//  TH1I* fTracksPerEventWithLambdaVer1 =(TH1I*)f->Get("fTracksPerEventWithLambdaVer1");
//  fTracksPerEventWithLambdaVer1->SetMaximum(6500);
//  fTracksPerEventWithLambdaVer1->Draw("");
//  gPad->Print("result_pictures/fTracksPerEventWithLambdaVer1.pdf","pdf");
//  gPad->Print("result_pictures/fTracksPerEventWithLambdaVer1.png","png");
//
//  TH1I* fTracksPerEventWithLambdaVer2 =(TH1I*)f->Get("fTracksPerEventWithLambdaVer2");
//  fTracksPerEventWithLambdaVer2->Draw("");
//  fTracksPerEventWithLambdaVer2->SetMaximum(6500);
//  fTracksPerEventWithLambdaVer2->Draw("");
//  gPad->Print("result_pictures/fTracksPerEventWithLambdaVer2.pdf","pdf");
//  gPad->Print("result_pictures/fTracksPerEventWithLambdaVer2.png","png");

  //===========

//  THStack* fSHAntiproton =(THStack*)f->Get("fSHAntiproton");
//  fSHAntiproton->Draw("");
//  gPad->BuildLegend();
//  gPad->Print("result_pictures/fSHAntiproton.pdf","pdf");

  //===========

//  THStack* fSHPionplus =(THStack*)f->Get("fSHPionplus");
//  fSHPionplus->Draw("");
//  gPad->BuildLegend();
//  gPad->Print("result_pictures/fSHPionplus.pdf","pdf");

  //===========

//  THStack* fSHPionminus =(THStack*)f->Get("fSHPionminus");
//  fSHPionminus->Draw("");
//  gPad->BuildLegend();
//  gPad->Print("result_pictures/fSHPionminus.pdf","pdf");

  //===========

//  THStack* fSHProton =(THStack*)f->Get("fSHProton");
//  fSHProton->Draw("");
//  gPad->BuildLegend();
//  gPad->Print("result_pictures/fSHProton.pdf","pdf");

  TH2F* fPositionDecayVertexLambdaLambdaBar2D = (TH2F*)f->Get("fPositionDecayVertexLambdaLambdaBar2D");

//  TH1I* fMVDPointsPerTrackProton = (TH1I*)f->Get("fMVDPointsPerTrackProton");
//  TH1I* fMVDPointsPerTrackProtonWithLambda = (TH1I*)f->Get("fMVDPointsPerTrackProtonWithLambda");
//
//  TH1I* fMVDPointsPerTrackAntiproton = (TH1I*)f->Get("fMVDPointsPerTrackAntiproton");
//  TH1I* fMVDPointsPerTrackAntiprotonWithLambda = (TH1I*)f->Get("fMVDPointsPerTrackAntiprotonWithLambda");
//
//  TH1I* fMVDPointsPerTrackPionplus = (TH1I*)f->Get("fMVDPointsPerTrackPionplus");
//  TH1I* fMVDPointsPerTrackPionplusWithLambda = (TH1I*)f->Get("fMVDPointsPerTrackPionplusWithLambda");
//
//  TH1I* fMVDPointsPerTrackPionminus = (TH1I*)f->Get("fMVDPointsPerTrackPionminus");
//  TH1I* fMVDPointsPerTrackPionminusWithLambda = (TH1I*)f->Get("fMVDPointsPerTrackPionminusWithLambda");
//
  TH1I* fMVDPointsPerTrackTotal = (TH1I*)f->Get("fMVDPointsPerTrackTotal");
  TH1I* fMVDPointsPerTrackTotalWithLambda = (TH1I*)f->Get("fMVDPointsPerTrackTotalWithLambda");
//
//  TH1I* fTotalPointsPerTrackAllWithLambda = (TH1I*)f->Get("fTotalPointsPerTrackAllWithLambda");
//  TH1I* fTotalPointsPerTrackAll = (TH1I*)f->Get("fTotalPointsPerTrackAll");
//
//  fMVDPointsPerTrackProton->SetMaximum(3000);
//  fMVDPointsPerTrackProton->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackProton.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackProton.png","png");
//
//  fMVDPointsPerTrackProtonWithLambda->SetMaximum(3000);
//  fMVDPointsPerTrackProtonWithLambda->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackProtonWithLambda.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackProtonWithLambda.png","png");
//
//  fMVDPointsPerTrackAntiproton->SetMaximum(3000);
//  fMVDPointsPerTrackAntiproton->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackAntiproton.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackAntiproton.png","png");
//
//  fMVDPointsPerTrackAntiprotonWithLambda->SetMaximum(3000);
//  fMVDPointsPerTrackAntiprotonWithLambda->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackAntiprotonWithLambda.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackAntiprotonWithLambda.png","png");
//
//  fMVDPointsPerTrackPionplus->SetMaximum(3000);
//  fMVDPointsPerTrackPionplus->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionplus.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionplus.png","png");
//
//  fMVDPointsPerTrackPionplusWithLambda->SetMaximum(3000);
//  fMVDPointsPerTrackPionplusWithLambda->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionplusWithLambda.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionplusWithLambda.png","png");
//
//  fMVDPointsPerTrackPionminus->SetMaximum(3000);
//  fMVDPointsPerTrackPionminus->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionminus.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionminus.png","png");
//
//  fMVDPointsPerTrackPionminusWithLambda->SetMaximum(3000);
//  fMVDPointsPerTrackPionminusWithLambda->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionminusWithLambda.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionminusWithLambda.png","png");
//
 // fMVDPointsPerTrackTotal->SetMaximum(12000);


  TH1F* fDecayLength= (TH1F*)f->Get("fMeanLifeLambdaCMS");
    TH1F* blubb = fDecayLength->Rebin(2,"blubb");
    TH1F* blubb1 = fDecayLength->Rebin(2,"blubb1");
      TH1F* blubb2 = blubb1->Rebin(2,"blubb2");
      TH1F* blubb3 = blubb2->Rebin(2,"blubb3");
      TH1F* blubb4 = blubb3->Rebin(2,"blubb4");
       TH1F* blubb5 = blubb4->Rebin(2,"blubb5");
        TH1F* blubb6 = blubb5->Rebin(2,"blubb6");
        TH1F* blubb7 = blubb6->Rebin(2,"blubb7");
        TH1F* blubb8 = blubb7->Rebin(2,"blubb8");

        std::cout << std::setw(15);
  	  std::cout << "Slope ";
  	std::cout << std::setw(15);
  	  std::cout << "Error ";
  	std::cout << std::setw(15);
  	  std::cout<<  "Chi quadrat/NDG ";
  	std::cout << std::setw(15);
  	  std::cout<<  "Chi quadrat/NDG ";
  	std::cout << std::setw(15);
  	  std::cout << "NBins " << std::endl;

  ctauhistogramm(fDecayLength, "result_pictures/LambdaCMS.eps");

  ctauhistogramm(blubb, "result_pictures/LambdaCMSblubb.eps");

  ctauhistogramm(blubb1, "result_pictures/LambdaCMSblubb1.eps");

  ctauhistogramm(blubb2, "result_pictures/LambdaCMSblubb2.eps");

  ctauhistogramm(blubb3, "result_pictures/LambdaCMSblubb3.eps");

  ctauhistogramm(blubb4, "result_pictures/LambdaCMSblubb4.eps");

   ctauhistogramm(blubb5, "result_pictures/LambdaCMSblubb5.eps");

    ctauhistogramm(blubb6, "result_pictures/LambdaCMSblubb6.eps");

    ctauhistogramm(blubb7, "result_pictures/LambdaCMSblubb7.eps");

    ctauhistogramm(blubb8, "result_pictures/LambdaCMSblubb8.eps");

 // fDecayLength= (TH1F*)f->Get("fMeanLifeLambdaBarCMS");
  //ctauhistogramm(fDecayLength, "result_pictures/LambdaBarCMS.eps");

  //fDecayLength= (TH1F*)f->Get("fMeanLifeLambdaLAB");
  //ctauhistogramm(fDecayLength, "result_pictures/LambdaLAB.eps");

  //fDecayLength= (TH1F*)f->Get("fMeanLifeLambdaBarLAB");
  //ctauhistogramm(fDecayLength, "result_pictures/LambdaBarLAB.eps");

  gPad->SetLogy(0);
  gPad->SetGrid();
  gStyle->SetOptStat(110);
  fMVDPointsPerTrackTotal->Draw();

  gPad->Print("result_pictures/fMVDPointsPerTrackTotal.pdf","pdf");
  gPad->Print("result_pictures/fMVDPointsPerTrackTotal.eps","eps");

  gPad->SetLogy(0);
  gPad->SetGrid();
  gStyle->SetOptStat(110);
  fMVDPointsPerTrackTotalWithLambda->Draw();
  gPad->Print("result_pictures/fMVDPointsPerTrackTotalWithLambda.pdf","pdf");
  gPad->Print("result_pictures/fMVDPointsPerTrackTotalWidthLambda.eps","eps");

  gStyle->SetOptStat(10);
  fMVDPointsPerTrackTotal->GetXaxis()->SetRange(0,12);
  fMVDPointsPerTrackTotal->SetLineColor(2);
  fMVDPointsPerTrackTotal->SetLineWidth(2);
  fMVDPointsPerTrackTotal->Draw();

  fMVDPointsPerTrackTotalWithLambda->Draw("SAME");
  fMVDPointsPerTrackTotalWithLambda->SetLineWidth(2);

  gPad->SetGrid();

  TLegend* legend = gPad->BuildLegend(0.5204598,0.5529661,0.9897701,0.7627119);
  legend->SetFillColor(0);

  fMVDPointsPerTrackTotal->SetTitle("#Hits/Track with and without #Lambda-Disks");

  gPad->Print("result_pictures/fMVDPointsPerTrackTotalcomparison.pdf","pdf");
  gPad->Print("result_pictures/fMVDPointsPerTrackTotalcomparison.eps","eps");

  TCanvas* canvas3 = new TCanvas("canvas3", "Results", 900,400);

  gPad->SetLogy(0);
  fPositionDecayVertexLambdaLambdaBar2D->Draw("colz");
  DrawMVD();
  DrawLambdaDiskPosition0();

  gPad->Print("result_pictures/PositionDecayVertexLambdaLambdabarPosition0.eps","eps");

  gPad->Clear();
  fPositionDecayVertexLambdaLambdaBar2D->Draw("colz");
  DrawMVD();
  DrawLambdaDiskPosition1();
  gPad->Print("result_pictures/PositionDecayVertexLambdaLambdabarPosition1.eps","eps");

  gPad->Clear();
  fPositionDecayVertexLambdaLambdaBar2D->Draw("colz");
  DrawMVD();
  DrawLambdaDiskPosition2();
  gPad->Print("result_pictures/PositionDecayVertexLambdaLambdabarPosition2.eps","eps");

  gPad->Clear();
  fPositionDecayVertexLambdaLambdaBar2D->Draw("colz");
  DrawMVD();
  DrawLambdaDiskPosition3();
  gPad->Print("result_pictures/PositionDecayVertexLambdaLambdabarPosition3.eps","eps");







//  fMVDPointsPerTrackTotalWithLambda->SetMaximum(12000);
//  fMVDPointsPerTrackTotalWithLambda->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackTotalWithLambda.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackTotalWithLambda.png","png");
//
//  fTotalPointsPerTrackAll->SetMaximum(3000);
//  fTotalPointsPerTrackAll->Draw();
//  gPad->Print("result_pictures/fTotalPointsPerTrackAll.pdf","pdf");
//  gPad->Print("result_pictures/fTotalPointsPerTrackAll.png","png");
//
//  fTotalPointsPerTrackAllWithLambda->SetMaximum(3000);
//  fTotalPointsPerTrackAllWithLambda->Draw();
//  gPad->Print("result_pictures/fTotalPointsPerTrackAllWithLambda.pdf","pdf");
//  gPad->Print("result_pictures/fTotalPointsPerTrackAllWithLambda.png","png");
//
//  //===========
//  gStyle->SetOptStat("");
//  //===========
//
//
//  fMVDPointsPerTrackTotal->SetBarOffset(0.1);
//  fMVDPointsPerTrackTotal->SetBarWidth(0.4);
//  fMVDPointsPerTrackTotal->SetStats(0);
//  fMVDPointsPerTrackTotal->Draw("bar");
//
//  fMVDPointsPerTrackTotalWithLambda ->SetBarOffset(0.5);
//  fMVDPointsPerTrackTotalWithLambda->SetBarWidth(0.4);
//  fMVDPointsPerTrackTotal->SetStats(0);
//  fMVDPointsPerTrackTotalWithLambda->Draw("same, bar");
//
//  TLegend *LegendTotal = new TLegend(0.5043103,0.6779661,0.8850575,0.8771186,NULL,"brNDC");
//  LegendTotal->AddEntry(fMVDPointsPerTrackTotal,"without #Lambda discs","f");
//  LegendTotal->AddEntry(fMVDPointsPerTrackTotalWithLambda,"with #Lambda discs","f");
//  LegendTotal->SetFillColor(kWhite);
//  LegendTotal->Draw();
//  fMVDPointsPerTrackTotal->SetTitle("# MVD hits per Track for all Particles");
//  //  TPaveStats *st = (TPaveStats*)fMVDPointsPerTrackAntiproton->FindObject("stats");
//  // st->SetOptStats("");
//
//
//  gPad->Print("result_pictures/fMVDPointsPerTrackTotalBoth.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackTotalBoth.png","png");
//
// //===========
//
//  fMVDPointsPerTrackProton->SetBarOffset(0.1);
//  fMVDPointsPerTrackProton->SetBarWidth(0.4);
//  fMVDPointsPerTrackProton->SetStats(0);
//  fMVDPointsPerTrackProton->Draw("bar");
//  // fMVDPointsPerTrackProton->SetStats(0);
//
//  fMVDPointsPerTrackProtonWithLambda ->SetBarOffset(0.5);
//  fMVDPointsPerTrackProtonWithLambda->SetBarWidth(0.4);
//  fMVDPointsPerTrackProtonWithLambda->SetStats(0);
//  fMVDPointsPerTrackProtonWithLambda->Draw("same, bar");
//  //  fMVDPointsPerTrackProtonWithLambda->SetStats(0);
//
//  TLegend *LegendProton = new TLegend(0.5043103,0.6779661,0.8850575,0.8771186,NULL,"brNDC");
//  LegendProton->AddEntry(fMVDPointsPerTrackProton,"without #Lambda discs","f");
//  LegendProton->AddEntry(fMVDPointsPerTrackProtonWithLambda,"with #Lambda discs","f");
//  LegendProton->SetFillColor(kWhite);
//  LegendProton->Draw();
//  fMVDPointsPerTrackProton->SetTitle("# MVD hits per Track for p");
//  // TPaveStats *st = (TPaveStats*)fMVDPointsPerTrackProton->FindObject("stats");
//  //  st->SetOptStats("");
//
//  gPad->Print("result_pictures/fMVDPointsPerTrackProtonBoth.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackProtonBoth.png","png");
//
// //===========
//
//  fMVDPointsPerTrackAntiproton->SetBarOffset(0.1);
//  fMVDPointsPerTrackAntiproton->SetBarWidth(0.4);
//  fMVDPointsPerTrackAntiproton->SetStats(0);
//  fMVDPointsPerTrackAntiproton->Draw("bar");
//
//  fMVDPointsPerTrackAntiprotonWithLambda ->SetBarOffset(0.5);
//  fMVDPointsPerTrackAntiprotonWithLambda->SetBarWidth(0.4);
//  fMVDPointsPerTrackAntiproton->SetStats(0);
//  fMVDPointsPerTrackAntiprotonWithLambda->Draw("same, bar");
//
//  TLegend *blubb = new TLegend(0.5043103,0.6779661,0.8850575,0.8771186,NULL,"brNDC");
//  blubb->AddEntry(fMVDPointsPerTrackAntiproton,"without #Lambda discs","f");
//  blubb->AddEntry(fMVDPointsPerTrackAntiprotonWithLambda,"with #Lambda discs","f");
//  blubb->SetFillColor(kWhite);
//  blubb->Draw();
//  fMVDPointsPerTrackAntiproton->SetTitle("# MVD hits per Track for #bar{p}");
//  //  TPaveStats *st = (TPaveStats*)fMVDPointsPerTrackAntiproton->FindObject("stats");
//  // st->SetOptStats("");
//
//
//  gPad->Print("result_pictures/fMVDPointsPerTrackAntiprotonBoth.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackAntiprotonBoth.png","png");
//
//  //===========
//
//  fMVDPointsPerTrackPionplus->SetBarOffset(0.1);
//  fMVDPointsPerTrackPionplus->SetBarWidth(0.4);
//  fMVDPointsPerTrackPionplus->SetStats(0);
//  fMVDPointsPerTrackPionplus->Draw("bar");
//
//  fMVDPointsPerTrackPionplusWithLambda->SetBarOffset(0.5);
//  fMVDPointsPerTrackPionplusWithLambda->SetBarWidth(0.4);
//  fMVDPointsPerTrackPionplus->SetStats(0);
//  fMVDPointsPerTrackPionplusWithLambda->Draw("same, bar");
//
//  TLegend *LegendPionplus = new TLegend(0.5043103,0.6779661,0.8850575,0.8771186,NULL,"brNDC");
//  LegendPionplus->AddEntry(fMVDPointsPerTrackPionplus,"without #Lambda discs","f");
//  LegendPionplus->AddEntry(fMVDPointsPerTrackPionplusWithLambda,"with #Lambda discs","f");
//  LegendPionplus->SetFillColor(kWhite);
//  LegendPionplus->Draw();
//  fMVDPointsPerTrackPionplus->SetTitle("# MVD hits per Track for #pi^{+}");
//
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionplusBoth.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionplusBoth.png","png");
//
//  //===========
//
//  fMVDPointsPerTrackPionminus->SetBarOffset(0.1);
//  fMVDPointsPerTrackPionminus->SetBarWidth(0.4);
//  fMVDPointsPerTrackPionminus->SetStats(0);
//  fMVDPointsPerTrackPionminus->Draw("bar");
//
//  fMVDPointsPerTrackPionminusWithLambda->SetBarOffset(0.5);
//  fMVDPointsPerTrackPionminusWithLambda->SetBarWidth(0.4);
//  fMVDPointsPerTrackPionminus->SetStats(0);
//  fMVDPointsPerTrackPionminusWithLambda->Draw("same, bar");
//
//  TLegend *LegendPionminus = new TLegend(0.5043103,0.6779661,0.8850575,0.8771186,NULL,"brNDC");
//  LegendPionminus->AddEntry(fMVDPointsPerTrackAntiproton,"without #Lambda discs","f");
//  LegendPionminus->AddEntry(fMVDPointsPerTrackAntiprotonWithLambda,"with #Lambda discs","f");
//  LegendPionminus->SetFillColor(kWhite);
//  LegendPionminus->Draw();
//  fMVDPointsPerTrackPionminus->SetTitle("# MVD hits per Track for #pi^{-}");
//
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionminusBoth.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackPionminusBoth.png","png");

  //===========

 // TCanvas* canvas2 = new TCanvas("canvas2", "Results", 1200,600);
 // canvas2->Divide(4,2);

//  canvas2->cd(1);
//  fMVDPointsPerTrackProton->Draw();
//
//  canvas2->cd(5);
//  fMVDPointsPerTrackProtonWithLambda->Draw();
//
//  canvas2->cd(2);
//  fMVDPointsPerTrackAntiproton->Draw();
//
//  canvas2->cd(6);
//  fMVDPointsPerTrackAntiprotonWithLambda->Draw();
//
//  canvas2->cd(3);
//  fMVDPointsPerTrackPionplus->Draw();
//
//  canvas2->cd(7);
//  fMVDPointsPerTrackPionplusWithLambda->Draw();
//
//  canvas2->cd(4);
//  fMVDPointsPerTrackPionminus->Draw();
//
//  canvas2->cd(8);
//  fMVDPointsPerTrackPionminusWithLambda->Draw();

  // -----   Finish   -------------------------------------------------------

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

//exit();
  return 0;
}
