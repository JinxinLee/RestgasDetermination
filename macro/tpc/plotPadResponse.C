#include "PndTpcGem.h"
#include "PndTpcPadShapePool.h"
#include "PndTpcAbsPadShape.h"
#include <iostream>
#include <vector>
#include "TCanvas.h"
#include "TVector2.h"
#include "TArrow.h"
#include "TGraph2D.h"
#include "PndTpcPad.h"
#include "TH2D.h"


void plotPadResponse(TString padshapes, double spread){

  PndTpcGem* _gem=new PndTpcGem(5000,           // Gain
			  spread);          // Spread
  
  PndTpcPadShapePool* _padShapes = new PndTpcPadShapePool(padshapes,
						    *_gem,
						    0.4, // lookup range
						    0.02, // Lookup Step
						    0.01); // LookupIntStep

  PndTpcAbsPadShape* shape=_padShapes->GetPadShape(0);
  
  double w,h;
  shape->EvalBoundingRect(w,h,0);

  PndTpcPad pad(0,0,0,shape,0,0);
  

  // make region we want to look at a bit bigger:
  w*=2;
  h*=2;
  double x0=-w*0.5;
  double y0=-h*0.5;

  int nbins=20;
  double wstep=w/nbins;
  double hstep=h/nbins;
  double x=x0;
  double y=y0;

  TGraph2D* padresp=new TGraph2D(nbins*nbins);

  int c=0;
  for(int i=0;i<nbins+1;++i){
    for(int j=0; j<nbins+1;++j){
      padresp->SetPoint(c++,x,y,shape->GetValue(x,y));
      y+=hstep;
    }
    y=y0;
    x+=wstep;
  }

  padresp->Draw("CONT1");

  double cont[10]={0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.99};
  TH2D* hi=padresp->GetHistogram();
  hi->SetContour(10,cont);
  hi->Draw("CONT1Z");
  pad.Draw(1);

}
