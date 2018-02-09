//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoLorentzVectorErr                                                  //
//                                                                      //
// LorentzVector w/ error                                               //
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99                                   //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoLorentzVectorErr.h"
#include "RhoVector3Err.h"
#include "TBuffer.h"

ClassImp ( RhoLorentzVectorErr )

TBuffer& operator>> ( TBuffer& buf, RhoLorentzVectorErr *&obj )
{
  obj = ( RhoLorentzVectorErr* ) buf.ReadObject ( RhoLorentzVectorErr::Class() );
  return buf;
}

#include <iostream>
using namespace std;

RhoLorentzVectorErr::RhoLorentzVectorErr ( const RhoVector3Err& p3,
    Double_t mass ) :
  TLorentzVector ( ( const TVector3& ) p3, sqrt ( p3.Mag2() + mass* mass ) )
{
  int i=0,j=0;
  fCovMatrix = new RhoError ( 4 );
  // The 3-vector part of the error, initialized as a 0-matrix, then copied
  // from p3:
  TMatrixD p4Err ( 4,4 );
  for ( i = 0; i < 3; ++i ) {
    for ( j = i; j < 3; ++j ) { // start from j=i, since (i,j) = (j,i)
      p4Err ( i,j ) = p3.CovMatrix() ( i,j );
    }
  }

  // The energy part of the error:
  const Double_t energy = E();

  if ( energy != 0 ) {
    p4Err ( 4,4 ) = 0.0;
    for ( i = 0; i < 3; ++i ) {
      for ( j = 0; j < 3; ++j ) {
        p4Err ( 4,4 ) += p3[i] * p3[j] *
                         p3.CovMatrix() ( i, j );
      }
    }
    p4Err ( 4,4 ) /= ( energy * energy );
  }

  // The E-p correlated part of the error is
  // <(E - <E>) (Pi - <Pi>)> = dE/dPj Vjk dPi/dPk
  // Since dPi/dPk = delta_ik, we get dE/dPj Vji.
  for ( i = 0; i < 3; ++i ) {
    for ( j = 0; j < 3; ++j ) {
      p4Err ( i,3 ) += p3 ( j ) / energy * p3.CovMatrix() ( j,i );
      // No need to switch indices for a TMatrixD, so no p4Err(4,i)
    }
  }

  SetCovMatrix ( p4Err );
}

// argumentless constructor:
RhoLorentzVectorErr::RhoLorentzVectorErr() : TLorentzVector()
{
  fCovMatrix = new RhoError ( 4 );
}

// auto casting constructor
RhoLorentzVectorErr::RhoLorentzVectorErr ( const TLorentzVector& p ) :
  TLorentzVector ( p )
{
  fCovMatrix = new RhoError ( 4 );
}

RhoLorentzVectorErr::RhoLorentzVectorErr ( const TLorentzVector& p, const RhoError& covMat ) :
  TLorentzVector ( p )
{
  fCovMatrix = new RhoError ( covMat );
}

// copy constructor
RhoLorentzVectorErr::RhoLorentzVectorErr ( const RhoLorentzVectorErr& v ) : TLorentzVector ( v )
{
  fCovMatrix = new RhoError ( v.CovMatrix() );
}

// assignment operator:
RhoLorentzVectorErr& RhoLorentzVectorErr::operator= ( const RhoLorentzVectorErr& v )
{
  if ( this != &v ) {
    TLorentzVector::operator= ( v );
    fCovMatrix = new RhoError ( v.CovMatrix() );
  }
  return *this;
}


// mathematical modifiers:
RhoLorentzVectorErr& RhoLorentzVectorErr::operator - ()
{
  ( ( TLorentzVector ) *this ) *= -1.;
  return *this;  // fCovMatrix unaltered
}

RhoLorentzVectorErr& RhoLorentzVectorErr::operator += ( const RhoLorentzVectorErr& v )
{
  TLorentzVector::operator+= ( v );
  *fCovMatrix += v.CovMatrix();
  return *this;
}

RhoLorentzVectorErr& RhoLorentzVectorErr::operator -= ( const RhoLorentzVectorErr& v )
{
  TLorentzVector::operator-= ( v );
  *fCovMatrix += v.CovMatrix();
  return *this;
}

//-------------------------------------------------------------
Double_t
RhoLorentzVectorErr::DetermineChisq ( const TLorentzVector& refVector )
{
  TVectorD temp ( 4 );
  temp ( 0 ) = refVector.X()-this->X();
  temp ( 1 ) = refVector.Y()-this->Y();
  temp ( 2 ) = refVector.Z()-this->Z();
  temp ( 3 ) = refVector.T()-this->T();

  return fCovMatrix->DetermineChisq ( temp );
}


//-------------------------------------------------------------
RhoLorentzVectorErr&
RhoLorentzVectorErr::Transform ( const TLorentzRotation& rot )
{
  TLorentzVector::Transform ( rot );
  *fCovMatrix = fCovMatrix->Similarity ( rot );
  return *this;
}

//-------------------------------------------------------------
RhoLorentzVectorErr&
RhoLorentzVectorErr::Transform ( const TRotation& rot )
{
  TLorentzVector::Transform ( rot );
  TMatrixD tempRot ( 4,4 );

  // Fill a 4x4 matrix from the 3x3 TRotation. Note that they use different
  // indexing schemes (!?@#$&^*&#$@#):

  int row;
  int col;
  for ( row = 0; row < 3; ++row ) { // 3 is the size of TRotation
    for ( col = 0; col < 3; ++col ) { // (which provides no enum)
      tempRot ( row, col ) = rot ( row, col );
    }
  }

  // fill the 4th row:
  tempRot ( 3,3 ) = 1.0;
  for ( col = 0; col < 3; ++col ) {
    tempRot ( 3, col ) = 0.0;
  }

  // fill the 4th column:
  for ( row = 0; row < 3; ++row ) {
    tempRot ( row, 3 ) = 0.0;
  }

  *fCovMatrix = fCovMatrix->Similarity ( tempRot );
  return *this;
}



//-------------------------------------------------------------
RhoLorentzVectorErr
operator + ( const RhoLorentzVectorErr& v, const RhoLorentzVectorErr& w )
{
  RhoLorentzVectorErr ve ( TLorentzVector ( v.X() +w.X(),v.Y() +w.Y(),v.Z() +w.Z(),
                           v.T() +w.T() ),
                           ( v.CovMatrix() +w.CovMatrix() ) );
  return ve;
}


//-------------------------------------------------------------
RhoLorentzVectorErr
operator - ( const RhoLorentzVectorErr& v, const RhoLorentzVectorErr& w )
{
  RhoLorentzVectorErr ve ( TLorentzVector ( v.X()-w.X(),v.Y()-w.Y(),v.Z()-w.Z(),
                           v.T()-w.T() ),
                           ( v.CovMatrix() +w.CovMatrix() ) );
  return ve;
}

void RhoLorentzVectorErr::PrintOn ( std::ostream& out ) const
{
  out << X() << "\t" << Y() << "\t" << Z() << "\t" << T();
}

std::ostream& operator<< ( std::ostream& stream, const RhoLorentzVectorErr& verr )
{
  verr.PrintOn ( stream );
  return stream;
}
