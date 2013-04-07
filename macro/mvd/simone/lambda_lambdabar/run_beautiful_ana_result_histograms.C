void ctauhistogramm(TH1F* histo, TString savefile)
{
	  histo->SetLineColor(kBlack);
	  histo->Draw("");
	  TF1 *fExpo = new TF1("fExpo","expo",0,100);
	  fExpo->SetLineColor(kRed);
	  fExpo->SetLineWidth(2);
	  histo->Fit("fExpo","QR");
	  printf("Slope %f", fExpo->GetParameter("Slope"));
	  fExpo->GetParameter("Slope");

	  TPaveText *pt = new TPaveText(7.553879,1.398014,40.38613,4.966407,"");
	  pt->UseCurrentStyle();
	  pt->SetFillColor(kWhite);
	  pt->SetBorderSize(1);
	  // pt->AddText("c#tau = " );
	  TString myString;
	//  myString=+1/fExpo->GetParameter("Slope");

	  std::stringstream virtualString;
	  virtualString<< "c#tau = ";
	  virtualString<< TMath::Abs(1/fExpo->GetParameter("Slope"));
	  virtualString<< "#pm";

	 // std::cout << " blubb " << fExpo->GetParError(fExpo->GetParNumber("Slope"))<< std::endl;
	  virtualString<<TMath::Abs(1/fExpo->GetParameter("Slope"))*TMath::Abs(1/fExpo->GetParameter("Slope"))*fExpo->GetParError(fExpo->GetParNumber("Slope"));
	  virtualString<< " cm";
	  TString bTagisGreaterThanSTRING = virtualString.str();

	  pt->AddText(bTagisGreaterThanSTRING.Data() );

	  pt->Draw();

	  gPad->SetLogy();
	  gPad->Print(savefile,"pdf");  // nicht fuer powerpoint geeignet
	//  gPad->Print("result_pictures/fMeanLife.gif","gif");  // ganz gut, aber keine Vektorgrafik
	//  gPad->Print("result_pictures/fMeanLife.png","png");  // ganz gut, aber keine Vektorgrafik
	//  gPad->Print("result_pictures/fMeanLife.jpg","jpg");  // schlechte qualitaet

}

void run_beautiful_ana_result_histograms(TString FileName="test"){
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
  // canvas->Divide(4,2);


  //  TCanvas* canvas2 = new TCanvas("canvas2", "Results", 1200,600);
  // canvas2->Divide(4,2);

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

  //===========



  //===========

  // canvas->cd(2);

  TH1F* fDecayLength= (TH1F*)f->Get("fMeanLifeLambdaCMS");
  ctauhistogramm(fDecayLength, "result_pictures/LambdaCMS.pdf");

  fDecayLength= (TH1F*)f->Get("fMeanLifeLambdaCMS");
  ctauhistogramm(fDecayLength, "result_pictures/LambdaBarCMS.pdf");

  fDecayLength= (TH1F*)f->Get("fMeanLifeLambdaLAB");
  ctauhistogramm(fDecayLength, "result_pictures/LambdaLAB.pdf");

  fDecayLength= (TH1F*)f->Get("fMeanLifeLambdaBarLAB");
  ctauhistogramm(fDecayLength, "result_pictures/LambdaBarLAB.pdf");

//  fDecayLength->SetLineColor(kBlack);
//  fMeanLifeLambdaCMS->Draw("");
//  TF1 *fExpo = new TF1("fExpo","expo",0,100);
//  fExpo->SetLineColor(kRed);
//  fExpo->SetLineWidth(2);
//  fMeanLifeLambdaCMS->Fit("fExpo","QR");
//  printf("Slope %f", fExpo->GetParameter("Slope"));
//  fExpo->GetParameter("Slope");
//
//  TPaveText *pt = new TPaveText(7.553879,1.398014,40.38613,4.966407,"");
//  pt->UseCurrentStyle();
//  pt->SetFillColor(kWhite);
//  pt->SetBorderSize(1);
//  // pt->AddText("c#tau = " );
//  TString myString;
////  myString=+1/fExpo->GetParameter("Slope");
//
//  std::stringstream virtualString;
//  virtualString<< "c#tau = ";
//  virtualString<< TMath::Abs(1/fExpo->GetParameter("Slope"));
//  virtualString<< " cm";
//  TString bTagisGreaterThanSTRING = virtualString.str();
//
//  pt->AddText(bTagisGreaterThanSTRING.Data() );
//
//  pt->Draw();
////pt->AddText(fExpo->GetParameter("Slope"));
//
//  gPad->SetLogy();
//  gPad->Print("result_pictures/fMeanLife.pdf","pdf");  // nicht fuer powerpoint geeignet
//  gPad->Print("result_pictures/fMeanLife.gif","gif");  // ganz gut, aber keine Vektorgrafik
//  gPad->Print("result_pictures/fMeanLife.png","png");  // ganz gut, aber keine Vektorgrafik
//  gPad->Print("result_pictures/fMeanLife.jpg","jpg");  // schlechte qualitaet
//
//  gPad->SetLogy(0);




  //===========


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
//  TH1I* fMVDPointsPerTrackTotal = (TH1I*)f->Get("fMVDPointsPerTrackTotal");
//  TH1I* fMVDPointsPerTrackTotalWithLambda = (TH1I*)f->Get("fMVDPointsPerTrackTotalWithLambda");
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
//  fMVDPointsPerTrackTotal->SetMaximum(12000);
//  fMVDPointsPerTrackTotal->Draw();
//  gPad->Print("result_pictures/fMVDPointsPerTrackTotal.pdf","pdf");
//  gPad->Print("result_pictures/fMVDPointsPerTrackTotal.png","png");
//
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

  TCanvas* canvas3 = new TCanvas("canvas3", "Results", 1200,600);
  canvas3->Divide(2,1);

  canvas3->cd(1);
  fMeanLifeLambdaCMS->Draw("");
//
//  canvas3->cd(2);
//  fTracksPerEvent->Draw("");


  TCanvas* canvas2 = new TCanvas("canvas2", "Results", 1200,600);
  canvas2->Divide(4,2);

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
}
