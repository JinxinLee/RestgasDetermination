#ifndef RHODOUBLEERR_H
#define RHODOUBLEERR_H
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

// Description:
// Class TDoubleErr holds a Double_t and its error squared,
// the equivalent of a TVectorErr with one dimension.
//
// determineChisq(Double_t ref) returns (ref-value())^2 / covariance().
// If the covariance() <= 0 then returns TError::chisqUndef.
//
// The mathematical operators can be used to correctly take into account
// error propagation through simple mathematical operations. Beware of
// operations whose covariance may not be the same as the one which
// results from a sequence of simple operations. For example, a*a will give
// the wrong covariance, since the algorithm assumes that the two arguments
// of the operator* are independent.

#include "TBuffer.h"
#include "TObject.h"
#include "RhoError.h"       // for chisqUndef only

class RhoDoubleErr : public TObject
{
  public:

    // Constructors: The fCovariance and the fValue are 0.0 unless specified:
    RhoDoubleErr() :                       fValue ( 0.0 ), fCovariance ( 0.0 ) {};
    RhoDoubleErr ( Double_t val ) :             fValue ( val ), fCovariance ( 0.0 ) {};
    RhoDoubleErr ( Double_t val, Double_t cov ) : fValue ( val ), fCovariance ( cov ) {};

    // Copy Constructor
    RhoDoubleErr ( const RhoDoubleErr& );

    // Destructor
    virtual ~RhoDoubleErr() {}

    // Assignment operator:
    RhoDoubleErr& operator = ( const RhoDoubleErr& );

    // Accessors (const)
    Double_t Value()                    const {
      return fValue;
    }
    Double_t Covariance()               const {
      return fCovariance;
    }
    Double_t DetermineChisq ( Double_t ref ) const;

    RhoDoubleErr operator-();   // value() -> -value(), covariance() unaffected
    RhoDoubleErr operator - ( const RhoDoubleErr& );
    RhoDoubleErr operator + ( const RhoDoubleErr& );

    // NOTE: (a * b).covariance() is
    // b^2 * a.covariance() + a^2 * b.covariance()
    RhoDoubleErr operator * ( const RhoDoubleErr& );

    // NOTE: (a / b).covariance() is
    // a.covariance() / b^2 + b.covariance() * a^2 / b^4
    RhoDoubleErr operator / ( const RhoDoubleErr& );

    // modifiers:
    void SetValue ( Double_t val )      {
      fValue = val;
    }
    void SetCovariance ( Double_t cov ) {
      fCovariance = cov;
    }

    RhoDoubleErr& operator += ( const RhoDoubleErr& );
    RhoDoubleErr& operator -= ( const RhoDoubleErr& );
    RhoDoubleErr& operator *= ( const RhoDoubleErr& );
    RhoDoubleErr& operator /= ( const RhoDoubleErr& );

    // needed for RWTValOrderedVector
    Bool_t           operator == ( const RhoDoubleErr& other ) const {
      return ( fValue == other.fValue && fCovariance == other.fCovariance );
    }
    Bool_t           operator < ( const RhoDoubleErr& other ) const {
      return ( fValue < other.fValue );
    }


  private:
    // Data members
    Double_t fValue;
    Double_t fCovariance;

  public:
    ClassDef ( RhoDoubleErr,1 ) // holds a Double_t and its error squared
};

std::ostream& operator<< ( std::ostream& stream, const RhoDoubleErr& bde );

#endif
