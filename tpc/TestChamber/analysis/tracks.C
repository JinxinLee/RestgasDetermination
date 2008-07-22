#include <iostream>
#include <fstream>
#include "TString.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TROOT.h"
#include "TPolyLine3D.h"
#include "TView.h"

#include "../src/TCtrack.h"
#include "../src/TCcluster.h"


#include "consecCut.C"


void tracks(TString files){

  //gSystem->Load("../libtestChamber.so"); 


  //gROOT->Macro("macro/christian_style.C"); 

  TChain myChain("at");

  cout << "Adding " <<  myChain.Add(files) << " to the chain" << endl;

  Int_t nevent = myChain.GetEntries();

  

  TCtrack *intr=0;

     TH1D *Ay = new TH1D("Ay","",500,-4,4);
     TH1D *By = new TH1D("By","",500,-4,4);
     TH1D *Ax = new TH1D("Ax","",500,-4,4);
     TH1D *Bx = new TH1D("Bx","",500,-4,4);

     TCanvas *tracks = new TCanvas();
     tracks->SetGrid();
     TView *view = TView::CreateView(1);
     view->SetRange(0,0,0,10,10,8);


  myChain.SetBranchAddress("track", &intr);

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

    Ay->Fill(tr.getAy());
    By->Fill(tr.getBy());
    Ax->Fill(tr.getAx());
    Bx->Fill(tr.getBx());

    Double_t x1 = tr.getBx();
    Double_t x2 = 8*tr.getAx() + tr.getBx();
    Double_t y11 = tr.getBy();
    Double_t y22 = 8*tr.getAy() + tr.getBy();

    TPolyLine3D *i = new TPolyLine3D(2); 

    i->SetPoint(0, x1, y11, 0);
    i->SetPoint(1, x2, y22, 10);
    i->Draw("same");

  }
   tracks->Modified();
   tracks->Update();

  //TCanvas *ax = new TCanvas();
  //Ax->Draw();
  //canvas->SaveAs("output/yresid1.eps");
  //TCanvas *bx = new TCanvas();
  //Bx->Draw();
  //canvas->SaveAs("output/yresid2.eps");
  //TCanvas *ay = new TCanvas();
  //Ay->Draw();
  //canvas->SaveAs("output/yresid3.eps");
  //TCanvas *by = new TCanvas();
  //By->Draw();
  //canvas->SaveAs("output/yresid4.eps");

}

