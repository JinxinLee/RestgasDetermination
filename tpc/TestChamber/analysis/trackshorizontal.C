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

     TCanvas *tracks = new TCanvas();
     tracks->SetGrid();
     TView *view = TView::CreateView(1);
     view->SetRange(0,0,0,10,10,10);

  TPolyLine3D *outline = new TPolyLine3D(17);
  outline->SetPoint(0,0,0,0);
  outline->SetPoint(1,10,0,0);
  outline->SetPoint(2,10,1,0);
  outline->SetPoint(3,0,1,0);
  outline->SetPoint(4,0,0,0);
  outline->SetPoint(5,0,0,8);
  outline->SetPoint(6,10,0,8);
  outline->SetPoint(7,10,1,8);
  outline->SetPoint(8,0,1,8);
  outline->SetPoint(9,0,0,8);
  outline->SetPoint(10,0,1,8);
  outline->SetPoint(11,0,1,0);
  outline->SetPoint(12,0,1,8);
  outline->SetPoint(13,10,1,8);
  outline->SetPoint(14,10,1,0);
  outline->SetPoint(15,10,0,0);
  outline->SetPoint(16,10,0,8);
  outline->SetLineWidth(3);
  outline->SetLineColor(5);

//     void DrawOutlineCube(0, 8, 0, 1, 0, 10);


  myChain.SetBranchAddress("track", &intr);

  for (Int_t i=0;i<nevent;i++){

    myChain.GetEntry(i);
    TCtrack tr(*intr);

    //Ay->Fill(tr.getAy());
    //By->Fill(tr.getBy());
    //Ax->Fill(tr.getAx());
    //Bx->Fill(tr.getBx());

    Double_t x1 = tr.getBx();
    Double_t x2 = 10*tr.getAx() + tr.getBx();
    Double_t y11 = tr.getBy();
    Double_t y22 = 10*tr.getAy() + tr.getBy();

    TPolyLine3D *i = new TPolyLine3D(2); 

    i->SetPoint(0, x1, y11, 0);
    i->SetPoint(1, x2, y22, 10);
    i->Draw("same");

  }
   outline->Draw();
   tracks->Modified();
   tracks->Update();


}

