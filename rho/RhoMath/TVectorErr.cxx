//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TVectorErr		    						//
//                                                                      //
// Vector w/ error class						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TVectorErr.h"
#include <float.h>

ClassImp(TVectorErr)

TBuffer &operator>>(TBuffer &buf, TVectorErr *&obj)
{
   obj = (TVectorErr *) buf.ReadObject(TVectorErr::Class());
   return buf;
}

#include <iostream>
using namespace std;

TVectorErr::TVectorErr() : TVector3(), fCovMatrix(3) {}

// auto casting constructor
TVectorErr::TVectorErr(const TVector3 &p) : TVector3(p), fCovMatrix(3)	{}
TVectorErr::TVectorErr(const TVector3 &p, const TError& covMat) : TVector3(p),
fCovMatrix(3)				{ fCovMatrix=covMat; }

// copy constructor:
TVectorErr::TVectorErr(const TVectorErr& v) : TVector3(v),
fCovMatrix(v.CovMatrix())	{}

// destructor MAY be needed later
// virtual ~TVectorErr() {};

// assignment operator:
TVectorErr& TVectorErr::operator=(const TVectorErr& v)
{
    if (this != &v) {
	TVector3::operator=(v);
	fCovMatrix = v.CovMatrix();
    }
    return *this;
}

TVectorErr& TVectorErr::operator - () {
    (*this) *= -1.;
    return *this;  // fCovMatrix remains unaltered
}

TVectorErr& TVectorErr::operator += (const TVectorErr& v){
    TVector3::operator+=(v);
    fCovMatrix += v.CovMatrix();
    return *this;
}

TVectorErr& TVectorErr::operator -= (const TVectorErr& v){
    TVector3::operator-=(v);
    fCovMatrix += v.CovMatrix();
    return *this;
}

TVectorErr& TVectorErr::Transform(const TRotation& rot){
    TVector3::Transform(rot);
    fCovMatrix = fCovMatrix.Similarity(rot);
    return *this;
}

//void TVectorErr::printOn(ostream& out) const
//{
//    out << "TVector and Covariance Matrix:" << endl;
//    TVector::printOn(out);
//    out << endl;
//    out << fCovMatrix;
//}
/* 
TError TVectorErr::covRTPMatrix() const{
// protect against 0's
Double_t xv = X()==0 ?  FLT_MIN : X();
Double_t yv = Y()==0 ?  FLT_MIN : Y();
Double_t zv = Z()==0 ?  FLT_MIN : Z();
DifNumber xDF(xv,X+1,3), yDF(yv,Y+1,3), zDF(zv,Z+1,3);
DifArray pars(3,NUM_PCOORDINATES);
pars[Rho]   =  sqrt(xDF*xDF + yDF*yDF + zDF*zDF);
pars[Phi]   = atan2(yDF,xDF);
pars[Theta] = acos(zDF/pars[Rho]);
return covMatrix().Similarity(pars.jacobian());
}

  TError TVectorErr::covRZPMatrix() const{
  // protect against 0's
  Double_t xv = X()==0 ?  FLT_MIN : X();
  Double_t yv = Y()==0 ?  FLT_MIN : Y();
  Double_t zv = Z()==0 ?  FLT_MIN : Z();
  DifNumber xDF(xv,X+1,3), yDF(yv,Y+1,3), zDF(zv,Z+1,3);
  DifArray pars(3,NUM_CCOORDINATES);
  pars[C_Rho]   =  sqrt(xDF*xDF + yDF*yDF);
  pars[C_Phi]   = atan2(yDF,xDF);
  pars[C_Zeta] = zDF;
  return covMatrix().Similarity(pars.jacobian());
  }
*/
Double_t TVectorErr::DetermineChisq(const TVector3& refVector)
{
    static TVectorD temp(3);
    temp(0) = refVector.X()-this->X();
    temp(1) = refVector.Y()-this->Y();
    temp(2) = refVector.Z()-this->Z();
    return fCovMatrix.DetermineChisq(temp);
}


TVectorErr operator + (const TVectorErr& v, const TVectorErr& w){
    TVectorErr ve(TVector3(v.X()+w.X(),v.Y()+w.Y(),v.Z()+w.Z()),
	(v.CovMatrix()+w.CovMatrix()));
    return ve;
}

TVectorErr operator - (const TVectorErr& v, const TVectorErr& w){
    TVectorErr ve(TVector3(v.X()-w.X(),v.Y()-w.Y(),v.Z()-w.Z()), 
	(v.CovMatrix()+w.CovMatrix()));
    return ve;
}





std::ostream & operator<<(std::ostream & stream, const TVectorErr & verr) {
    stream << (const TVector3&)verr
	<< ", " << verr.CovMatrix();
    
    return stream;
}

std::istream & operator>>(std::istream & stream, TVectorErr & verr) {
    TError mat(3);
    stream >> verr >> mat;
    verr.SetCovMatrix(mat);
    
    return stream;
}

