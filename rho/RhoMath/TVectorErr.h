#ifndef TVECTORERR_H
#define TVECTORERR_H
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

#include <iostream>

#include "RhoMath/TError.h"
#include "TRotation.h"

class TVectorErr : public TVector3 {
    
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
    TVectorErr();
    
    // auto casting constructor
    TVectorErr(const TVector3 &p);
    TVectorErr(const TVector3 &p, const TError& covMat);
    
    // copy constructor:
    TVectorErr(const TVectorErr& v);
    
    // destructor MAY be needed later
    virtual ~TVectorErr() {};
    
    // assignment operator:
    TVectorErr& operator=(const TVectorErr& v);
    
    TVectorErr& operator - ();
    
    TVectorErr& operator += (const TVectorErr& v);
    
    TVectorErr& operator -= (const TVectorErr& v);
    
    TVectorErr& Transform(const TRotation& rot);
    
    Double_t DetermineChisq(const TVector3& refVector);   
    // returns Chisquare
    // refVector refers to the same origin as the TVector3 of this
    // ie refVector is not relative to this Vector
    
    
    inline const TError& CovMatrix() const { return fCovMatrix; }
    
    //TError CovRTPMatrix() const;
    // returns the covariance Matrix in spherical coordinate
    // use   PolarCoordinateIndex enum to get the components
    
    //TError CovRZPMatrix() const;
    // returns the covariance Matrix in cylindrical coordinate
    // use   CylindricalCoordinateIndex enum to get the components
    
    inline void SetCovMatrix(const TError& v) { fCovMatrix = v; }
    
    //  void printOn( std::ostream& o=std::cout ) const;
    
private:
    
    TError fCovMatrix; //!
    
public:
    ClassDef(TVectorErr,1) // Add errors to a vector
};


TVectorErr operator + (const TVectorErr&, const TVectorErr&);

TVectorErr operator - (const TVectorErr&, const TVectorErr&);

std::ostream & operator<<(std::ostream & stream, const TVectorErr & verr);
std::istream & operator>>(std::istream & stream, TVectorErr & verr);

#endif






