#ifndef TERROR_H
#define TERROR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TError		    						//
//                                                                      //
// Error matrix class							//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// Description:
//      A wrapper for a covariance matrix.  A covariance matrix is
//	a symmetric n X n matrix.  Change in chisq from point
//	covariance matrix was determined is just
//
//	diff * covariance^-1 * diff_transpose
//
//	which is implemented in a similarity transform in TMatrixD
//	the method determineChisq carries this calculation out and requires
//	the result to be a scalar.

#include <iostream>

#include <math.h>
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TRotation.h"
#include "TLorentzRotation.h"

class TError : public TMatrixD {
    
public:
    
    static const Double_t chisqUndef;
    
    TError(Int_t n, Double_t init = 0.0);
    
    // autocast copy constructor.  TMatrixD's promoted back
    // into TError matrices.
    
    TError( const TMatrixD &p );
    
    // new constructors for this class
    TError(const TError& v);
    
    TError& operator=(const TError& v);
    
    TError& operator=(const TMatrixD& v);
    
    // destructor MAY be needed later
    virtual ~TError() {};
    
    //----------------------------------------------------------------------
    // determineChisq
    //	Compute v^T * V^(-1)*v - ie the chisq for this covariance
    //	matrix and the difference vector v.
    //----------------------------------------------------------------------
    
    Double_t DetermineChisq(TVectorD& diff); 
    
    // Get right signature for all operations performed on TError matrices
    // that should (and will) result in TError matrices.  These include
    // similarity transforms, transpose, inverse, matrix multiplication,
    // addition, and subtraction.  TMatrixD's as a result of operations
    // are promoted back into TError matrices if we start out
    // with TError matrices in the first place. (See copy constructors)
    
    TError& operator *= (Double_t t);
    
    //  TError& operator /= (Double_t t);
    
    //  TError& operator += (const TError& m2);
    
    //  TError& operator -= (const TError& m2);
    
    TError& operator - ();
    
    //TError& Similarity(const TError& E);
    // implement E * covMatrix * E
    
    TError& Similarity(const TMatrixD& m1) const;
    
    TError& SimilarityWith(const TError& m, const TMatrixD& m1);
    
    TError& Similarity(const TRotation& rot) const;
    TError& Similarity(const TLorentzRotation& rot) const;
    // When feasible implement R * covMatrix * R_transpose (= R^-1)
    
    Double_t Similarity(TVectorD &m1);
    TMatrixD& SimilarityT(TMatrixD &m1);
    
private:
    
    friend TError& operator*(Double_t t, const TError& m1);
    
    friend TError& operator*(const TError& m1, Double_t t);
    /*
    friend TError& operator/(Double_t t, const TError& m1);
    
    friend TError& operator/(const TError& m1, Double_t t);
    */
    friend TError& operator+(const TError& m1, const TError& m2);
    
    friend TError& operator-(const TError& m1, const TError& m2);
    
    friend std::ostream& operator<<(std::ostream& out, const TError& mat);
    friend std::istream& operator>>(std::istream& in, TError& mat);

public:
    ClassDef(TError,1) // Wrapper class for a covariance matrix
};

#endif






