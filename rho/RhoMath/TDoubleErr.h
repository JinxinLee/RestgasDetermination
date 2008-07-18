#ifndef TDOUBLEERR_H
#define TDOUBLEERR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TDoubleErr		    						//
//                                                                      //
// Double w/ error							//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
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

#include "TObject.h"
#include "RhoMath/TError.h"       // for chisqUndef only

class TDoubleErr : public TObject {
public:
    
    // Constructors: The fCovariance and the fValue are 0.0 unless specified:
    TDoubleErr() :                       fValue(0.0), fCovariance(0.0) {}
    TDoubleErr(Double_t val) :             fValue(val), fCovariance(0.0) {}
    TDoubleErr(Double_t val, Double_t cov) : fValue(val), fCovariance(cov) {}
    
    // Copy Constructor
    TDoubleErr(const TDoubleErr &);
    
    // Destructor
    virtual ~TDoubleErr() {}
    
    // Assignment operator:
    TDoubleErr & operator = (const TDoubleErr &);
    
    // Accessors (const)
    Double_t Value()                    const {return fValue;}
    Double_t Covariance()               const {return fCovariance;}
    Double_t DetermineChisq(Double_t ref) const;
    
    TDoubleErr operator-();   // value() -> -value(), covariance() unaffected
    TDoubleErr operator - (const TDoubleErr &);
    TDoubleErr operator + (const TDoubleErr &);
    
    // NOTE: (a * b).covariance() is 
    // b^2 * a.covariance() + a^2 * b.covariance()
    TDoubleErr operator * (const TDoubleErr &);
    
    // NOTE: (a / b).covariance() is 
    // a.covariance() / b^2 + b.covariance() * a^2 / b^4
    TDoubleErr operator / (const TDoubleErr &);
    
    // modifiers:
    void SetValue(Double_t val)      {fValue = val;}
    void SetCovariance(Double_t cov) {fCovariance = cov;}
    
    TDoubleErr & operator += (const TDoubleErr &);
    TDoubleErr & operator -= (const TDoubleErr &);
    TDoubleErr & operator *= (const TDoubleErr &);
    TDoubleErr & operator /= (const TDoubleErr &);
    
    // needed for RWTValOrderedVector
    Bool_t           operator == (const TDoubleErr & other) const 
    { return (fValue == other.fValue && fCovariance == other.fCovariance); }
    Bool_t           operator <   (const TDoubleErr & other) const 
    { return (fValue < other.fValue); }
    
    
private:
    // Data members
    Double_t fValue;
    Double_t fCovariance;
    
public:
    ClassDef(TDoubleErr,1) // holds a Double_t and its error squared
};

std::ostream & operator<<(std::ostream & stream, const TDoubleErr & bde);

#endif
