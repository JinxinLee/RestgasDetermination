#ifndef RHOVECTORERR_H
#define RHOVECTORERR_H
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

#include <iostream>

#include "RhoError.h"
#include "TRotation.h"

class RhoVector3Err : public TVector3
{

  public:
    // polar coordinates
    enum PolarCoordinateIndex {
      Rho = 0,
      Theta = 1,
      Phi = 2,
      NUM_PCOORDINATES = 3
    };

    enum CylindricalCoordinateIndex {
      C_Rho = 0,
      C_Zeta = 1,
      C_Phi = 2,
      NUM_CCOORDINATES = 3
    };
    // argumentless constructor:
    RhoVector3Err();

    // auto casting constructor
    RhoVector3Err ( const TVector3& p );
    RhoVector3Err ( const TVector3& p, const RhoError& covMat );

    // copy constructor:
    RhoVector3Err ( const RhoVector3Err& v );

    // destructor MAY be needed later
    virtual ~RhoVector3Err() {};

    // assignment operator:
    RhoVector3Err& operator= ( const RhoVector3Err& v );

    RhoVector3Err& operator - ();

    RhoVector3Err& operator += ( const RhoVector3Err& v );

    RhoVector3Err& operator -= ( const RhoVector3Err& v );

    RhoVector3Err& Transform ( const TRotation& rot );

    Double_t DetermineChisq ( const TVector3& refVector );
    // returns Chisquare
    // refVector refers to the same origin as the TVector3 of this
    // ie refVector is not relative to this Vector


    inline const RhoError& CovMatrix() const {
      return fCovMatrix;
    }

    //TError CovRTPMatrix() const;
    // returns the covariance Matrix in spherical coordinate
    // use   PolarCoordinateIndex enum to get the components

    //TError CovRZPMatrix() const;
    // returns the covariance Matrix in cylindrical coordinate
    // use   CylindricalCoordinateIndex enum to get the components

    inline void SetCovMatrix ( const RhoError& v ) {
      fCovMatrix = v;
    }

    //  void printOn( std::ostream& o=std::cout ) const;

  private:

    RhoError fCovMatrix; //!

  public:
    ClassDef ( RhoVector3Err,1 ) // Add errors to a vector
};


RhoVector3Err operator + ( const RhoVector3Err&, const RhoVector3Err& );

RhoVector3Err operator - ( const RhoVector3Err&, const RhoVector3Err& );

std::ostream& operator<< ( std::ostream& stream, const RhoVector3Err& verr );
std::istream& operator>> ( std::istream& stream, RhoVector3Err& verr );

#endif
