#ifndef RHOLORENTZVECTORERR_H
#define RHOLORENTZVECTORERR_H
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

class RhoVector3Err;

#include <iostream>

#include "RhoError.h"
#include "TLorentzVector.h"
#include "TLorentzRotation.h"

class RhoLorentzVectorErr : public TLorentzVector
{
  public:
    enum MPolarCoordinateIndex {
      Mom = 0,
      Theta = 1,
      Phi = 2,
      Mass = 3,
      NUM_MPCOORDINATES = 4
    };
    enum EPolarCoordinateIndex {
      Energy = 0,
      ETheta = 1,
      EPhi = 2,
      EMom = 3,
      NUM_EPCOORDINATES = 4
    };
    // argumentless constructor:
    RhoLorentzVectorErr();

    // auto casting constructor
    RhoLorentzVectorErr ( const TLorentzVector& p );

    RhoLorentzVectorErr ( const TLorentzVector& p, const RhoError& covMat );

    // Useful constructor for tracks:
    RhoLorentzVectorErr ( const RhoVector3Err& p3, Double_t mass );

    // copy constructor
    RhoLorentzVectorErr ( const RhoLorentzVectorErr& v );

    // destructor MAY be needed later
    virtual ~RhoLorentzVectorErr() {
      delete fCovMatrix;
    }

    // assignment operator:
    RhoLorentzVectorErr& operator= ( const RhoLorentzVectorErr& v );

    // mathematical modifiers:
    RhoLorentzVectorErr& operator - ();

    RhoLorentzVectorErr& operator += ( const RhoLorentzVectorErr& v );

    RhoLorentzVectorErr& operator -= ( const RhoLorentzVectorErr& v );

    // can't implement this since there is no
    //  TLorentzVector::transform(const HepTranslation):
    //
    //  RhoLorentzVectorErr& transform(const HepTranslation& trans){
    //    TLorentzVector::transform(trans);
    //    return *this;
    //  }

    RhoLorentzVectorErr& Transform ( const TRotation& rot );

    RhoLorentzVectorErr& Transform ( const TLorentzRotation& rot );

    // can't implement this since there is no
    // TLorentzVector::transform(const HepTransformation):
    //
    //  RhoLorentzVectorErr& transform(const HepTransformation& transf){
    //    TLorentzVector::transform(transf);
    //    fCovMatrix = fCovMatrix.similarity(transf.rot_mat());
    //    return *this;
    //  }

    Double_t DetermineChisq ( const TLorentzVector& refVector );
    // returns Chisquare
    // refVector refers to the same origin as the LorentzVector of this
    // ie refVector is not relative to this Vector

    //TError covMRTPMatrix() const;
    // returns the covariance Matrix in spherical coordinate and mass
    // use   MPolarCoordinateIndex enum to get the components
    //TError covETPRMatrix() const;
    // returns the covariance Matrix in spherical coordinate and mass
    // use   EPolarCoordinateIndex enum to get the components
    // note: it is different from the others because of the different EMC convention

    inline const RhoError& CovMatrix() const    {
      return *fCovMatrix;
    }
    inline void SetCovMatrix ( const RhoError& v ) {
      fCovMatrix = new RhoError ( v );
    }

    void PrintOn ( std::ostream& o=std::cout ) const;

  private:

    RhoError* fCovMatrix; //!

  public:
    ClassDef ( RhoLorentzVectorErr,1 ) // Add errors to a LorentzVector
};

// globals:
RhoLorentzVectorErr operator + ( const RhoLorentzVectorErr&,
                                 const RhoLorentzVectorErr& );

RhoLorentzVectorErr operator - ( const RhoLorentzVectorErr&,
                                 const RhoLorentzVectorErr& );

std::ostream& operator<< ( std::ostream& stream, const RhoLorentzVectorErr& verr );

#endif
