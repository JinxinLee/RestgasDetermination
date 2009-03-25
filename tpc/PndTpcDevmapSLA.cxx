
#include "PndTpcDevmapSLA.h"
#include <iostream>


PndTpcDevmapSLA::PndTpcDevmapSLA(const char* const file, double vdrift) 
  :  PndTpcDevmapCyl(file, vdrift)
{
  std::cout<<"PndTpcDevmapSLA::PndTpcDevmapSLA(): initialized mother"<<std::endl;
  //calvulate line slopes 
  std::cout<<"_spacingX: "<<_spacingX<<",   _maxZ: "<<_maxZ
	   <<"\n_xBins: " <<_xBins<<",   _relPosition: ("
	   <<_relPosition.X()<<","<<_relPosition.Y()<<","
	   <<_relPosition.Z()<<")"<<std::endl<<std::endl;
  for (int i=0; i<=_xBins; i++) {
    //lines coincide with bin barriers (in r)
    TVector3 temp;
    if(i==_xBins)
      temp= TVector3(_maxX-0.0001,0., _maxZ-0.1);
    else
      temp= TVector3(i*_spacingX+_minX,0., _maxZ-0.1);
    TVector3 dev = PndTpcDevmapCyl::value(temp);
    _slopesR.push_back(dev.X()/(_maxZ-0.1 - _minZ));
    _slopesPerpR.push_back(dev.Y()/(_maxZ-0.1 - _minZ));
    std::cout<<"slopes for lines at r="<<i*_spacingX+_minX<<": "
	     <<_slopesR.back()<<"; "<<_slopesPerpR.back()<<std::endl;
  }
  
} 


TVector3
PndTpcDevmapSLA::value(const TVector3& point) const
{
  if(!_loaded || !pointOk(point)){
    std::cout<<"\nPndTpcDevmapSLA::value():  Point was not 'ok', "
	     <<"returning (0,0,0)"<<std::endl;
    return TVector3(0,0,0);
  }
  
  TVector3 inPoint = point - relPosition();
  double phi = inPoint.Phi();
  inPoint.RotateZ(-phi);

  //find right lines and calculate deviation
  int rBin = (int)((inPoint.X()-_minX)/_spacingX);   
  
  double binCoord = (inPoint.X()-_minX - rBin*_spacingX)/_spacingX;

  double devR1 = (inPoint.Z() - _minZ)*_slopesR[rBin];
  double devR2 = (inPoint.Z() - _minZ)*_slopesR[rBin+1];
  double devR = (devR2 - devR1)/_spacingX * binCoord + devR1;

  double devPR1 = (inPoint.Z() - _minZ)*_slopesPerpR[rBin];
  double devPR2 = (inPoint.Z() - _minZ)*_slopesPerpR[rBin+1];
  double devPR = (devPR2 - devPR1)/_spacingX * binCoord + devPR1;

  TVector3 dev(devR, devPR,0.);
  
  dev.RotateZ(phi);		  

  return dev;
}
  
  

  
  
 
