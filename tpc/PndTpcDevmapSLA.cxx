
#include "PndTpcDevmapSLA.h"
#include <iostream>


PndTpcDevmapSLA::PndTpcDevmapSLA(const char* const file, double vdrift) 
  :  PndTpcDevmapCyl(file, vdrift)
{
  std::cout<<"PndTpcDevmapSLA::PndTpcDevmapSLA(): initialized mother"<<std::endl;
  //calvulate line slopes 
  std::cout<<"fspacingX: "<<fspacingX<<",   fmaxZ: "<<fmaxZ
	   <<"\n_xBins: " <<fxBins<<",   frelPosition: ("
	   <<frelPosition.X()<<","<<frelPosition.Y()<<","
	   <<frelPosition.Z()<<")"<<std::endl<<std::endl;
  for (int i=0; i<=fxBins; i++) {
    //lines coincide with bin barriers (in r)
    TVector3 temp;
    if(i==fxBins)
      temp= TVector3(fmaxX-0.0001,0., fmaxZ-0.1);
    else
      temp= TVector3(i*fspacingX+fminX,0., fmaxZ-0.1);
    TVector3 dev = PndTpcDevmapCyl::value(temp);
    fslopesR.push_back(dev.X()/(fmaxZ-0.1 - fminZ));
    fslopesPerpR.push_back(dev.Y()/(fmaxZ-0.1 - fminZ));
    std::cout<<"slopes for lines at r="<<i*fspacingX+fminX<<": "
	     <<fslopesR.back()<<"; "<<fslopesPerpR.back()<<std::endl;
  }
  
} 


TVector3
PndTpcDevmapSLA::value(const TVector3& point) const
{
  if(!floaded || !pointOk(point)){
    std::cout<<"\nPndTpcDevmapSLA::value():  Point was not 'ok', "
	     <<"returning (0,0,0)"<<std::endl;
    return TVector3(0,0,0);
  }
  
  TVector3 inPoint = point - relPosition();
  double phi = inPoint.Phi();
  inPoint.RotateZ(-phi);

  //find right lines and calculate deviation
  int rBin = (int)((inPoint.X()-fminX)/fspacingX);   
  
  double binCoord = (inPoint.X()-fminX - rBin*fspacingX)/fspacingX;

  double devR1 = (inPoint.Z() - fminZ)*fslopesR[rBin];
  double devR2 = (inPoint.Z() - fminZ)*fslopesR[rBin+1];
  double devR = (devR2 - devR1)/fspacingX * binCoord + devR1;

  double devPR1 = (inPoint.Z() - fminZ)*fslopesPerpR[rBin];
  double devPR2 = (inPoint.Z() - fminZ)*fslopesPerpR[rBin+1];
  double devPR = (devPR2 - devPR1)/fspacingX * binCoord + devPR1;

  TVector3 dev(devR, devPR,0.);
  
  dev.RotateZ(phi);		  

  return dev;
}
  
  

  
  
 
