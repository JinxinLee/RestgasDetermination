#include"TCabsHough.h"

TCabsHough::~TCabsHough(){}

TCabsHough::TCabsHough(const TVector3& _yp,const TVector3& _zp){
  yp=_yp;zp=_zp;
}

void TCabsHough::convert(std::vector<TCcluster>& _c){
  TVector3 xp=yp.Cross(zp);
  TMatrixT<double> S(3,3);
  S[0][0]=xp.X();
  S[1][0]=xp.Y();
  S[2][0]=xp.Z();
  S[0][1]=yp.X();
  S[1][1]=yp.Y();
  S[2][1]=yp.Z();
  S[0][2]=zp.X();
  S[1][2]=zp.Y();
  S[2][2]=zp.Z();

  //TMatrixT<double> Stransp = S;
  //Stransp.T();

  clear();
  for(unsigned int i=0;i<_c.size();++i){
	TVector3 hitPrime = S * _c.at(i).posXYZ();
	//xprime isnt needed

	ypHit.push_back(hitPrime.Y());
	zpHit.push_back(hitPrime.Z());
  }

  //renormalize y and z
  double y1=-1.;
  double y2=1.;
  double z1=-1;
  double z2=1.;

  double minY=1.E50;
  double maxY=-1.E50;
  double minZ=1.E50;
  double maxZ=-1.E50;

  for(unsigned int i=0;i<nHits();++i){
	if(ypHit.at(i)>maxY) maxY=ypHit.at(i);
	if(ypHit.at(i)<minY) minY=ypHit.at(i);
	if(zpHit.at(i)>maxZ) maxZ=zpHit.at(i);
	if(zpHit.at(i)<minZ) minZ=zpHit.at(i);
  }

  if( fabs(maxY-minY)<1.E-10)minY-=1.E-5;
  if( fabs(maxZ-minZ)<1.E-10)minZ-=1.E-5;

  for(unsigned int i=0;i<nHits();++i){
	ypHit.at(i)=y1+(y2-y1)*(ypHit.at(i)-minY)/(maxY-minY);
	zpHit.at(i)=z1+(z2-z1)*(zpHit.at(i)-minZ)/(maxZ-minZ);
  }
  
}

