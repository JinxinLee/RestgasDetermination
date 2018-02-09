//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoVectorErr                                                         //
//                                                                      //
// Vector w/ error class                                                //
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99                                   //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoVector3Err.h"
#include "TBuffer.h"
#include <float.h>

ClassImp ( RhoVector3Err )

TBuffer& operator>> ( TBuffer& buf, RhoVector3Err *&obj )
{
  obj = ( RhoVector3Err* ) buf.ReadObject ( RhoVector3Err::Class() );
  return buf;
}

#include <iostream>
using namespace std;

RhoVector3Err::RhoVector3Err() : TVector3(), fCovMatrix ( 3 ) {}

// auto casting constructor
RhoVector3Err::RhoVector3Err ( const TVector3& p ) : TVector3 ( p ), fCovMatrix ( 3 ) {}
RhoVector3Err::RhoVector3Err ( const TVector3& p, const RhoError& covMat ) : TVector3 ( p ),
  fCovMatrix ( 3 )
{
  fCovMatrix=covMat;
}

// copy constructor:
RhoVector3Err::RhoVector3Err ( const RhoVector3Err& v ) : TVector3 ( v ),
  fCovMatrix ( v.CovMatrix() )  {}

// destructor MAY be needed later
// virtual ~TVectorErr() {};

// assignment operator:
RhoVector3Err& RhoVector3Err::operator= ( const RhoVector3Err& v )
{
  if ( this != &v ) {
    TVector3::operator= ( v );
    fCovMatrix = v.CovMatrix();
  }
  return *this;
}

RhoVector3Err& RhoVector3Err::operator - ()
{
  ( *this ) *= -1.;
  return *this;  // fCovMatrix remains unaltered
}

RhoVector3Err& RhoVector3Err::operator += ( const RhoVector3Err& v )
{
  TVector3::operator+= ( v );
  fCovMatrix += v.CovMatrix();
  return *this;
}

RhoVector3Err& RhoVector3Err::operator -= ( const RhoVector3Err& v )
{
  TVector3::operator-= ( v );
  fCovMatrix += v.CovMatrix();
  return *this;
}

RhoVector3Err& RhoVector3Err::Transform ( const TRotation& rot )
{
  TVector3::Transform ( rot );
  fCovMatrix = fCovMatrix.Similarity ( rot );
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
Double_t RhoVector3Err::DetermineChisq ( const TVector3& refVector )
{
  TVectorD temp ( 3 );
  temp ( 0 ) = refVector.X()-this->X();
  temp ( 1 ) = refVector.Y()-this->Y();
  temp ( 2 ) = refVector.Z()-this->Z();
  return fCovMatrix.DetermineChisq ( temp );
}


RhoVector3Err operator + ( const RhoVector3Err& v, const RhoVector3Err& w )
{
  RhoVector3Err ve ( TVector3 ( v.X() +w.X(),v.Y() +w.Y(),v.Z() +w.Z() ),
                     ( v.CovMatrix() +w.CovMatrix() ) );
  return ve;
}

RhoVector3Err operator - ( const RhoVector3Err& v, const RhoVector3Err& w )
{
  RhoVector3Err ve ( TVector3 ( v.X()-w.X(),v.Y()-w.Y(),v.Z()-w.Z() ),
                     ( v.CovMatrix() +w.CovMatrix() ) );
  return ve;
}





std::ostream& operator<< ( std::ostream& stream, const RhoVector3Err& verr )
{
  stream << ( const TVector3& ) verr
         << ", " << verr.CovMatrix();

  return stream;
}

std::istream& operator>> ( std::istream& stream, RhoVector3Err& verr )
{
  RhoError mat ( 3 );
  stream >> verr >> mat;
  verr.SetCovMatrix ( mat );

  return stream;
}

