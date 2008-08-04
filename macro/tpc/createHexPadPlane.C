#include <vector>
#include <iostream>
#include <fstream>
#include <exception>
#include "TVector2.h"
#include "TMath.h"
#include "tpc/PndTpcAbsPadShape.h"
#include "tpc/PndTpcPadShapePool.h"
#include "tpc/PndTpcPad.h"
#include "tpc/PndTpcPadPlane.h"
#include "tpc/PndTpcGem.h"


void
createHexPadPlane(){
  double ar=0.15; // hexagone radius
  double a=ar*1.5;
  double b=ar*sqrt(3.); // y-grid distance

  double outerR=42;
  double innerR=15;
  
  bool targetpipe=true;
  double phislice=5.0*TMath::Pi()/180;

  int nrows=2*floor(outerR/b)+2;
  int ncols=2*floor(outerR/a)+2;

  double startx=-outerR;
  double starty=startx;

  int nrings=1;
  int nslices=1;

  double dring=(outerR-innerR)/(double)nrings;

  try{

  PndTpcGem* _gem=new PndTpcGem(5000,           // Gain
			  0.02);          // Spread
  
  PndTpcPadShapePool* _padShapes = new PndTpcPadShapePool("tpc/Hexagons0.15.dat",
				   *_gem,
				   0.4, // lookup range
				   0.02, // Lookup Step
				   0.01); // LookupIntegrationStep

  PndTpcAbsPadShape* shape=_padShapes->GetPadShape(0);

  
  PndTpcPadPlane plane(42,42,2,2,-42,-42);

  

  int count=0;
  for(int i=0; i<ncols; ++i){
    double x=startx+a*(double)i;
    double yoff= (i%2==0) ? 0 : b/2.;
    for(int j=0; j<nrows; ++j){
      double y=starty+yoff+b*(double)j;
      
      double r=sqrt(x*x+y*y);
      TVector2 l(x,y);
      double phi=l.Phi();

      if(r>outerR)continue;
      if(r<innerR)continue;
      if(targetpipe){
	if(fabs(phi-TMath::Pi()/2)<phislice)continue;
	if(fabs(phi-3*TMath::Pi()/2)<phislice)continue;
      }

      unsigned int sliceId=(unsigned int)floor(nslices*phi/TMath::Pi()/2);
      unsigned int ringId=(unsigned int)floor((r-innerR)/dring);

      unsigned int sectorId=sliceId*nrings+ringId;

      PndTpcPad* pad=new PndTpcPad(x,y,0,
			     shape, sectorId, count++);

      plane.AddPad(pad);
      
    }
  }

  // build neighbourhoods!
  unsigned int npads=plane.GetNPads();

  std::cout<<npads<<" pads created. Building neighbourhoods..."<<std::endl;
  std::cout.flush();

  for(int k=0;k<npads;++k){
    PndTpcPad* pad=plane.GetPad(k);
    std::vector<PndTpcPad*> neighblist;
    plane.GetPadList(pad->x(),pad->y(),ar*1.01,neighblist);
    unsigned int nneigh=neighblist.size();
    unsigned int c=0;
    for(int ineigh=0;ineigh<nneigh;++ineigh){
      if(neighblist[ineigh]->sectorId()==pad->sectorId() &&
	 neighblist[ineigh]->padId()!=pad->padId()){
	pad->addNeighbour(neighblist[ineigh]->id());
	++c;
      }
    }
    //std::cout<<"found "<<c<<" neighbours"<<std::endl;
    neighblist.clear();
  }

 ofstream outfile("hexplane.dat");
 plane.WriteToStream(outfile);
 outfile.close();


  }
  catch(std::exception& ex) {
    std::cout<< ex.what() <<std::endl;
    throw ex;
  }
}
