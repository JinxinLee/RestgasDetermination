#include "PndTpcPadPlane.h"
#include "PndTpcGem.h"
#include "PndTpcPadShapePool.h"
#include <iostream>
#include <vector>
#include "TCanvas.h"
#include "TVector2.h"
#include "TArrow.h"
#include "TWbox.h"
#include "TPad.h"



void plotPadPlane(TString padshapes, TString padplane, bool drawlinks=false){

  PndTpcGem* _gem=new PndTpcGem(5000,           // Gain
                  0.02);          // Spread
  
  PndTpcPadShapePool* _padShapes = new PndTpcPadShapePool(padshapes,
				   *_gem,
				   0.4, // lookup range
				   0.02, // Lookup Step
				   0.01); // LookupIntegrationStep
  
  
  PndTpcPadPlane* _padPlane= new PndTpcPadPlane(padplane, _padShapes);
  
  
  std::cout<<*_padPlane<<std::endl;

  TCanvas* c=new TCanvas("c","regions",10,10,800,800);
  double x0=_padPlane->GetX0();
  double y0=_padPlane->GetY0();
  double x1=x0+_padPlane->GetXBin()*_padPlane->GetNX();
  double y1=y0+_padPlane->GetYBin()*_padPlane->GetNY();
  


  gPad->Range(x0,y0,x1,y1);

  // plot regions
  int npads=_padPlane->GetNPads();
  std::vector<TVector2> regmin(1000);
  std::vector<TVector2> regmax(1000);

  
  for(int i=0;i<npads;++i){
    PndTpcPad* apad=_padPlane->GetPad(i);
    int sectorid=apad->sectorId();
    apad->Draw(sectorid%10+1);
    unsigned int nn=apad->nNeighbours();
    // draw clusterizer links
    if(drawlinks){
      for(unsigned int in=0;in<nn;++in){
	PndTpcPad* neigh=_padPlane->GetPad(apad->getNeighbour(in));
	TArrow* link=new TArrow(apad->x(),apad->y(),neigh->x(),neigh->y(),0.001,"-|>-");
	link->SetFillColor(1);
	link->Draw();
      }
    }
  }
  
  c->Update();
  c->Draw();

  c->SaveAs("padplane.eps");
}
