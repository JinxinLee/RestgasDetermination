//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoDoubleErr                                                         //
//                                                                      //
// Double w/ error                                                      //
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99                                   //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoDoubleErr.h"
#include "TBuffer.h"
#include <math.h>

ClassImp ( RhoDoubleErr )

TBuffer& operator>> ( TBuffer& buf, RhoDoubleErr *&obj )
{
  obj = ( RhoDoubleErr* ) buf.ReadObject ( RhoDoubleErr::Class() );
  return buf;
}

#include <iostream>
using namespace std;

static const char rscid[] = "$Id: TDoubleErr.cxx,v 1.3 2002-02-01 22:34:57 marcel Exp $";

RhoDoubleErr::RhoDoubleErr ( const RhoDoubleErr& original ) :
TObject(original)
{
  *this = original;
}

RhoDoubleErr& RhoDoubleErr::operator = ( const RhoDoubleErr& original )
{
  fValue = original.fValue;
  fCovariance = original.fCovariance;
  return *this;
}

Double_t
RhoDoubleErr::DetermineChisq ( Double_t ref ) const
{
  if ( 0 >= fCovariance ) {
    return -1.;
  } else {
    Double_t diff = ( ref - fValue );
    return ( diff * diff ) / fCovariance;
  }
}

RhoDoubleErr RhoDoubleErr::operator-()
{
  return RhoDoubleErr ( -fValue, fCovariance );
}

RhoDoubleErr RhoDoubleErr::operator - ( const RhoDoubleErr& bde )
{
  return RhoDoubleErr ( fValue - bde.fValue, fCovariance - bde.fCovariance );
}

RhoDoubleErr RhoDoubleErr::operator + ( const RhoDoubleErr& bde )
{
  return RhoDoubleErr ( fValue + bde.fValue, fCovariance + bde.fCovariance );
}

RhoDoubleErr RhoDoubleErr::operator * ( const RhoDoubleErr& bde )
{
  return RhoDoubleErr ( fValue * bde.fValue,
                        fCovariance * bde.fValue * bde.fValue +
                        bde.fCovariance * fValue * fValue );
}

RhoDoubleErr RhoDoubleErr::operator / ( const RhoDoubleErr& bde )
{
  Double_t bde2 = bde.fValue * bde.fValue;

  return RhoDoubleErr ( fValue / bde.fValue,
                        fCovariance / bde2 +
                        bde.fCovariance * fValue * fValue / ( bde2 * bde2 ) );
}

RhoDoubleErr& RhoDoubleErr::operator += ( const RhoDoubleErr& bde )
{
  *this = *this + bde;
  return *this;
}

RhoDoubleErr& RhoDoubleErr::operator -= ( const RhoDoubleErr& bde )
{
  *this = *this - bde;
  return *this;
}

RhoDoubleErr& RhoDoubleErr::operator *= ( const RhoDoubleErr& bde )
{
  *this = *this * bde;
  return *this;
}

RhoDoubleErr& RhoDoubleErr::operator /= ( const RhoDoubleErr& bde )
{
  *this = *this / bde;
  return *this;
}

std::ostream& operator<< ( std::ostream& stream, const RhoDoubleErr& bde )
{
  stream << "value: " << bde.Value()
         << " covariance: " << bde.Covariance();

  return stream;
}

