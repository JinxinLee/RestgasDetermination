#include "tpc/PndTpcGem.h"
#include "tpc/PndTpcPadPlane.h"
#include "tpc/PndTpcPadShapePool.h"
#include "tpc/PndTpcSignal.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH2D.h"
#include "TStyle.h"

void plotSignals(){

  gStyle->SetFillColor(kWhite);

  //TODO: Get these things from Database!!!
  PndTpcGem* _gem=new PndTpcGem(5000,           // Gain
                  0.02);          // Spread

  PndTpcPadShapePool* _padShapes = new PndTpcPadShapePool("Hexagons0.2.dat",
				   *_gem,
				   0.5, // lookup range
				   0.02, // Lookup Step
				   0.01); // LookupIntegrationStep


   PndTpcPadPlane* _padPlane=new PndTpcPadPlane("pndhexplane.dat", _padShapes);





  TFile* file = new TFile("/afs/e18/data/panda/MC/FAIRRoot/DPM3/sigSlice1/test1.sig.root");
  //TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");
  
  TTree *t=(TTree*)file->Get("cbmsim") ;
  
  TClonesArray *sa=new TClonesArray("PndTpcSignal");
  t->SetBranchAddress("PndTpcSignal",&sa);

  TH2D* ma=new TH2D("map","Hit pads",420,-42,42,420,-42,42);

  for (Int_t j=0; j< t->GetEntriesFast(); j++)	{
    t->GetEntry(j);
    for (Int_t i=0; i<sa->GetEntriesFast(); i++)	{
      PndTpcSignal *sig=(PndTpcSignal*)sa->At(i);
      double x,y;
      _padPlane->GetPadXY(sig->padId(),x,y);
      ma->Fill(x,y);
    }
  }
  
  ma->Draw("BOX");
}

