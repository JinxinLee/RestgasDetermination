#ifndef TPOINTERR_H
#define TPOINTERR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPointErr		    						//
//                                                                      //
// Point w/ error class							//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TVectorErr.h"
#include "RhoMath/TError.h"
#include "TRotation.h"
#include "TVector3.h"

class TPointErr : public TVector3
{
public:
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
    TPointErr() : TVector3(), fCovMatrix(3) {}
    TPointErr(const TVector3 &p) : TVector3(p), fCovMatrix(3) {}
    TPointErr(const TVector3 &p, const TError& covMat) : TVector3(p), fCovMatrix(3)	{ fCovMatrix=covMat; }
    
    // new constructors for this class
    TPointErr(const TPointErr& v) : fCovMatrix(3)	{*this = v;}
    
    TPointErr& operator=(const TPointErr& v)
    {
	if (this != &v) {
	    TVector3::operator=(v);
	    fCovMatrix = v.CovMatrix();
	}
	return *this;
    }
    
    TPointErr& operator - () {
	(*this) *= -1.;
	return *this; // -covMatrix remains unaltered
    }
    
    TPointErr& operator -= (const TVectorErr& v){
	TVector3::operator -= (v);
	fCovMatrix += v.CovMatrix();
	return *this;
    }
    
    TPointErr& operator += (const TVectorErr& v){
	TVector3::operator += (v);
	fCovMatrix += v.CovMatrix();
	return *this;
    }
    
    TPointErr& Transform(const TRotation& rot){
	TVector3::Transform(rot);
	fCovMatrix = fCovMatrix.Similarity(rot);
	return *this;
    }
    
    
    // destructor MAY be needed later
    virtual ~TPointErr() {};
    
    Double_t DetermineChisq(const TVector3& diffPoint)
    {
	static TVectorD temp(3);
	temp(0) = diffPoint.X()-this->X();
	temp(1) = diffPoint.Y()-this->Y();
	temp(2) = diffPoint.Z()-this->Z();
	return fCovMatrix.DetermineChisq(temp);
    }
    
    inline const TError& CovMatrix() const		{ return fCovMatrix; }
    inline void SetCovMatrix(const TError& v)		{ fCovMatrix = v; }
    
    //  void printOn( std::ostream& o=std::cout ) const;
    
private:
    
    TError fCovMatrix;
    
public:
    ClassDef(TPointErr,1) //Point class with a covariance matrix
};

TPointErr operator + (const TPointErr&, const TVectorErr&);

TPointErr operator + (const TVectorErr&, const TPointErr&);

TPointErr operator - (const TPointErr&, const TVectorErr&);

TVectorErr operator - (const TPointErr&, const TPointErr&);

#endif





