#include "tpc/PndTpcGem.h"
#include "tpc/PndTpcPadPlane.h"
#include "tpc/PndTpcPadShapePool.h"
#include "tpc/PndTpcDigi.h"
#include "tpc/PndTpcDigiMapper.h"
#include "tpc/PndTpcGas.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH2D.h"
#include "TStyle.h"
#include "algorithm"
#include "sstream"
#include "tpc/PndTpcDigiAge.h"

void plotDigis(TTree* t, TString padplane, TString padshapes, bool movie=false){

  gStyle->SetFillColor(kWhite);


  TCanvas* c=new TCanvas("c1","PndTpc Digis",0,0,600,600);

  //TODO: Get these things from Database!!!
  PndTpcGem* _gem=new PndTpcGem(5000,           // Gain
                  0.02);          // Spread

  PndTpcPadShapePool* _padShapes = new PndTpcPadShapePool(padshapes.Data(),
				   *_gem,
				   0.5, // lookup range
				   0.02, // Lookup Step
				   0.01); // LookupIntegrationStep


  PndTpcPadPlane* _padPlane= new PndTpcPadPlane(padplane.Data(),_padShapes);

  PndTpcGas* _gas=new PndTpcGas("tpc/NEON-90_CO2-10_B2_PRES1013.asc",400);


  PndTpcDigiMapper::getInstance(false)->init(_padPlane,_gem,_gas,-40,-60000,40);
  

  //TFile* file = new TFile(datafile);
  //TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");
  
  //TTree *t=(TTree*)file->Get("cbmsim") ;
  
  TClonesArray *sa=new TClonesArray("PndTpcDigi");
  t->SetBranchAddress("PndTpcDigi",&sa);

  TH2D* ma=new TH2D("map","Hit pads",420,0,42,200,-42,42);
  TH2D* mz=new TH2D("mapz","hmapz",200,-50,150,100,0,42);

  for (Int_t j=0; j< t->GetEntries(); j++)	{
    t->GetEntry(j);
    for (Int_t i=0; i<sa->GetEntriesFast(); i++)	{
      PndTpcDigi *sig=(PndTpcDigi*)sa->At(i);
      double x,y;
      _padPlane->GetPadXY(sig->padId(),x,y);
      ma->Fill(x,y,sig->amp());
      TVector3 pos;
      PndTpcDigiMapper::getInstance()->map(sig,pos);
      mz->Fill(pos.Z(),pos.Perp());
    }
  }

 ma->SetDrawOption("BOX");
 ma->Draw();
 

  if(movie){
  // Produce movie
  ma->Reset();
  ma->SetDrawOption("BOX");

  

  // get first event

  t->GetEntry(0);

  std::vector<PndTpcDigi*> vd;
  Int_t ndig=sa->GetEntries();
  for (Int_t i=0; i<ndig; ++i){
    PndTpcDigi *sig=(PndTpcDigi*)sa->At(i);
    vd.push_back(sig);
  }
  //sort in time;
  sort(vd.begin(),vd.end(),PndTpcDigiAge());
  // make frames
  Int_t t1=0;
  int counter=0;
  for(Int_t i=0; i<ndig; ++i){
    PndTpcDigi *sig=vd[i];

    if(sig->t()-t1>10){
      t1=sig->t();
      // print frame
      std::ostringstream file;
      file<<"frames/digi";
      if(counter<100)file<<"0";
      if(counter<10)file<<"0";
      file<<counter<<".gif";
      ++counter;
      ma->Draw("BOX");
      c->SaveAs(file.str().c_str());
      ma->Reset();
    }
    double x,y;
    _padPlane->GetPadXY(sig->padId(),x,y);
    ma->Fill(x,y);
  }
  }
  
}

