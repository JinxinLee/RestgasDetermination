// This is a collection of useful tool functions for root macros
// mostly it is to make it more eye-candy and easier to work
// Thanks go to Thomas Goepfert & Markus Warsinsky from IKTP Dresden who
// put some of these on our wiki

#include <TLatex.h>
#include <TColor.h>
#include <TString.h>
#include <TPaveStats.h>
#include <TH1.h>
#include <TLegend.h>
#include <THStack.h>
#include <TList.h>
#include <TStyle.h>
#include <TROOT.h>


void DrawText(Double_t posX = 0., Double_t posY = 0., const char* text = "",
                     Double_t size=0.08, Int_t col=1
//                      Int_t align=0, Double_t angle=0.,
//                      Int_t font, Bool_t bNDC=kTRUE
                     )
{
  // Drawing some text

  TLatex* pText=new TLatex(posX,posY,text);
  pText->SetNDC(kTRUE);
//   pText->SetNDC(bNDC);
  pText->SetTextColor(col); pText->SetTextSize(size);
//   pText->SetTextAngle(angle);
//   pText->SetTextFont(font); pText->SetTextAlign(align);
  pText->Draw();
}

void DrawNice2DHisto(TH2* h,const char* opt="",double range = 10.)
{
  // Draw a 2D histo with the rainbow colors and the palette besides
  TString options = "colz"; options += opt;
  if(gPad->GetLogz() == 0 && h->GetMaximum()<range) h->SetAxisRange(0.,range,"Z");
  h->SetStats(kFALSE);
  h->SetTitleOffset(0.8,"T");
  gPad->SetRightMargin(0.16);
  h->DrawCopy(options.Data());
}

void set_nicer_2d_plot_style()
{
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
//     gStyle->CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
}

void BetterStatBox( TPad* pad ){
/// Vorgehensweise:
/// * Histogramme in ein TPad zeichnen (1. Histogramm: Draw(), ntes Histogramm:
///   Draw("sames"))
/// * die Update() Funktion vom Canvas aufrufen (die Statistikboxen werden erst
///   erzeugt, wenn das Histogramm wirklich gezeichnet wird)
/// * der Funktion das aktuelle TPad uebergeben
///   (z.B.: TPad* p=(TPad*)gPad; BetterStatBox(p);)

  int entries  = pad->GetListOfPrimitives()->GetEntries();
  TString name = "";
  TPaveStats *st;
  int obj = 1;

  for(int i=0; i<entries; i++){
    name = pad->GetListOfPrimitives()->At(i)->IsA()->GetName();
    if( name.Contains("TH1") ){
      TH1* hist = (TH1*)pad->GetListOfPrimitives()->At(i);
      st = (TPaveStats*)hist->GetListOfFunctions()->FindObject("stats");
      if(st){
        st->SetOptStat(1);
        st->SetX1NDC(0.80);
        st->SetY1NDC(1-0.16*obj) ;
        st->SetY2NDC( st->GetY1NDC()+0.15 ) ;
        st->SetTextColor( hist->GetLineColor() );
        st->SetLineColor( hist->GetLineColor() );
        st->Draw("same");
        obj++;
      }
    }
  }
}

// TLegend* BuildLegend_THStack( THStack* stack, float x1, float y1, float x2, float y2 ){
//
//   TLegend* legend = new TLegend(x1,y1,x2,y2);
//   TList*   list = stack->GetHists();
//   TIter    next( list );
//   TH1*     hist;
//
//   while ( hist = (TH1*)next() ) legend->AddEntry(hist,"","F");
//
//   return legend;
// }

void LoadPandaStyle(void)
{

//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PBase.C,v 1.8 2006/09/22 12:04:13 kliemt Exp $
//
// Description:
//      Initialization code executed at the start of a ROOT session.
//      Set up the Panda style for approved plots.
//
// Environment:
//      Software developed for the PANDA Detector at GSI, Darmstadt
//
// Author List:
//      Sergey Ganzhur                Original Author
//
// Copyright Information:
//     Copyright (C) 2001-2002        Ruhr Universitaet Bochum
//
//------------------------------------------------------------------------

  // use the 'plain' style for plots (white backgrounds, etc)
  //cout << "...using style 'Plain'" << endl;
  gROOT->SetStyle("Plain");

  // Create the 'PANDA' style for approved plots. Note that this style may need
  // some fine tuning in your macro depending on what you are plotting, e.g.
  //
  //  gStyle->SetMarkerSize(0.75);  // use smaller markers in a histogram with many bins
  //  gStyle->SetTitleOffset(0.65,"y");  // bring y axis label closer to narrow values


  // Ralf Kliemt:
  // Ok I changed a bit for myself here


  TStyle *pandaStyle= new TStyle("PANDA","PANDA approved plots style");

  // use plain black on white colors
  pandaStyle->SetFrameBorderMode(0);
  pandaStyle->SetCanvasBorderMode(0);
  pandaStyle->SetPadBorderMode(0);
  pandaStyle->SetPadColor(0);
  pandaStyle->SetCanvasColor(0);
  pandaStyle->SetTitleColor(1);
  pandaStyle->SetStatColor(0);
//   pandaStyle->SetFillColor(0);// conflict with 2D plots
//   pandaStyle->SetPalette(1);

  // set the paper & margin sizes
  pandaStyle->SetPaperSize(20,26);
  pandaStyle->SetPadTopMargin(0.05);
  pandaStyle->SetPadRightMargin(0.05);
  pandaStyle->SetPadBottomMargin(0.16);
  pandaStyle->SetPadLeftMargin(0.12);

  // use large Times-Roman fonts
//   pandaStyle->SetTextFont(132);
//   pandaStyle->SetLabelFont(132,"x");
//   pandaStyle->SetLabelFont(132,"y");
//   pandaStyle->SetLabelFont(132,"z");
  pandaStyle->SetTextFont(22);//changed to bold (R.K.)
  pandaStyle->SetTextSize(0.08);
  pandaStyle->SetLabelFont(22,"x");//changed to bold (R.K.)
  pandaStyle->SetLabelFont(22,"y");//changed to bold (R.K.)
  pandaStyle->SetLabelFont(22,"z");//changed to bold (R.K.)
  pandaStyle->SetLabelSize(0.05,"x");
  pandaStyle->SetTitleSize(0.06,"x");
  pandaStyle->SetLabelSize(0.05,"y");
  pandaStyle->SetTitleSize(0.06,"y");
  pandaStyle->SetLabelSize(0.05,"z");
  pandaStyle->SetTitleSize(0.06,"z");

  // use bold lines and markers
  pandaStyle->SetMarkerStyle(8);
  pandaStyle->SetHistLineWidth(2);//1.85);
  pandaStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // do not display any of the standard histogram decorations
  pandaStyle->SetOptTitle(1);
  pandaStyle->SetOptStat(1);
  pandaStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  pandaStyle->SetPadTickX(1);
  pandaStyle->SetPadTickY(1);

  //R.K. avoid clumsy axis lables
  pandaStyle->SetNdivisions(509); // default root value is 510

  //cout <<"    For approved plots use: gROOT->SetStyle(\"PANDA\");"<< endl;

  pandaStyle->cd();
  gROOT->ForceStyle();
  set_nicer_2d_plot_style();//[R.K.] use nicer 2D plots
}//void PBase::LoadPandaStyle(void)



/*
 * TransformHisto.C
 *
 *  Created on: Feb 25, 2009
 *      Author: stockman
 */

TH1D TransformHisto(TH2* h2, double min, double max)
{
	TH1D result("h1","h1", 1000, min, max);
	int nbins = h2->GetNbinsX() * h2->GetNbinsY();
	for (int i = 0; i < nbins; i++){
		//std::cout << h2->GetBinContent(i) << std::endl;
		result->Fill(h2->GetBinContent(i));
		if (i == 10)
			cout << h2->GetBinContent(i) << endl;
	}
	return result;
}
