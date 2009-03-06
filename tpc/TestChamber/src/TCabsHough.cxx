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
  
}

