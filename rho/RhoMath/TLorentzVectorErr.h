#ifndef TLORENTZVECTORERR_H
#define TLORENTZVECTORERR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TLorentzVectorErr		    					//
//                                                                      //
// LorentzVector w/ error						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TVectorErr;

#include <iostream>

#include "RhoMath/TError.h"
#include "TLorentzVector.h"
#include "TLorentzRotation.h"

class TLorentzVectorErr : public TLorentzVector {
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
    TLorentzVectorErr();
    
    // auto casting constructor
    TLorentzVectorErr(const TLorentzVector &p);
    
    TLorentzVectorErr(const TLorentzVector &p, const TError& covMat);
    
    // Useful constructor for tracks:
    TLorentzVectorErr(const TVectorErr & p3, Double_t mass);		      
    
    // copy constructor
    TLorentzVectorErr(const TLorentzVectorErr& v);
    
    // destructor MAY be needed later
    virtual ~TLorentzVectorErr() { delete fCovMatrix; }
    
    // assignment operator:
    TLorentzVectorErr& operator=(const TLorentzVectorErr& v);
    
    // mathematical modifiers:
    TLorentzVectorErr& operator - ();
    
    TLorentzVectorErr& operator += (const TLorentzVectorErr& v);
    
    TLorentzVectorErr& operator -= (const TLorentzVectorErr& v);
    
    // can't implement this since there is no
    //  TLorentzVector::transform(const HepTranslation):
    //
    //  TLorentzVectorErr& transform(const HepTranslation& trans){
    //    TLorentzVector::transform(trans);
    //    return *this;
    //  }
    
    TLorentzVectorErr& Transform(const TRotation& rot);
    
    TLorentzVectorErr& Transform(const TLorentzRotation& rot);
    
    // can't implement this since there is no 
    // TLorentzVector::transform(const HepTransformation):
    //
    //  TLorentzVectorErr& transform(const HepTransformation& transf){
    //    TLorentzVector::transform(transf);
    //    fCovMatrix = fCovMatrix.similarity(transf.rot_mat());
    //    return *this;
    //  }
    
    Double_t DetermineChisq(const TLorentzVector& refVector);   
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
    
    inline const TError& CovMatrix() const    { return *fCovMatrix; }
    inline void SetCovMatrix(const TError& v) { fCovMatrix = new TError(v); }

    void PrintOn( std::ostream& o=std::cout ) const;
    
private:
    
    TError *fCovMatrix; //!
    
public:
    ClassDef(TLorentzVectorErr,1) // Add errors to a LorentzVector
};

// globals:
TLorentzVectorErr operator + (const TLorentzVectorErr&, 
				const TLorentzVectorErr&);

TLorentzVectorErr operator - (const TLorentzVectorErr&, 
				const TLorentzVectorErr&);

std::ostream& operator<<(std::ostream & stream, const TLorentzVectorErr & verr);

#endif






