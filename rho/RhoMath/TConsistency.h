#ifndef TCONSISTENCY_H
#define TCONSISTENCY_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TConsistency				    				//
//                                                                      //
// Consistency base class						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// Description:
//	The TConsistency class is a top level parent for various
//      statistical classes
//
//      This class also defines an enum ConsistentStatus for representing
//      various error conditions
//
//      Return an object of this class from a chisquared calculation, for
//      example, so that probability and chisquared can be treated
//      directly.  Future subclasses might implement other statistics
//      (Poisson, etc), and this class could use operations to combine, 
//      print, etc. (Or other statistics could be added to this class, 
//      allowing it to convert between different measures; the uses
//      will dictate the structure)

#include "TNamed.h"
#include <iostream>

class TConsistencySet;

class TConsistency : public TNamed {
    
public:
    
    // default constructor; sets internal state to noMeasure and value to zero
    TConsistency();
    
    // default value for likelihood is for backward compatibility
    TConsistency( Double_t consistency, Double_t likelihood=0.);
    
    // copy constructor
    TConsistency(const TConsistency& rhs);
    
    virtual ~TConsistency() {}
    
    // assignment
    TConsistency& operator= (const TConsistency& rhs);
    
    // equality -- this will never work right since one has to compare 
    // two Double_ts. But RW sometimes requires this. 
    Bool_t operator==(const TConsistency& rhs) const;
    // less operator is better in that regard, but it's not unique
    Bool_t operator < (const TConsistency& rhs) const; 
    // greater than operator 
    Bool_t operator > (const TConsistency& rhs) const; 
    
    // members for returning 
    // the statistical significance level
    // and the likelihood 
    // of the represented observation; 
    // subclasses are responsible
    // for keeping the data member up to date
    Double_t SignificanceLevel() const;
    Double_t Likelihood() const;
    
    // Old function kept in the interface for back compatibility.
    // Avoid to use it - use significanceLevel() instead.
    // Will disappear eventually
    Double_t Consistency() const;
    
    // represent the various ways that a statistical measure could be bad
    enum EConsistentStatus { OK=0, noMeasure, underFlow, unPhysical };
    
    // OK          just that; usable statistical data
    // noMeasure   no statistical statement possible; DOF == 0, for example.
    //             may or may not be consistent
    // underFlow   probability too small to calculate with; may or may not want
    //             to treat as zero, but value may have been treated by machine
    //             underflow handling
    // unPhysical  because of a "can't happen" condition, probability is
    //             identically equal to zero;
    
    // return the status
    EConsistentStatus Status() const;
    void SetStatus(EConsistentStatus s );
    
    // represent whether the measured value was "left" or "right" of
    // the expected.  This is generally only useful if there is more
    // than one provider of consistencies in a given situation (e.g.
    // multiple systems doing PID), and they have a convention for 
    // what these mean
    enum EConsistentSign { left=-1, unknown=0, right=1 };
    
    // return the sign
    EConsistentSign Sign() const;
    void SetSign(EConsistentSign s );
    
    // use as a Double_t returns the consistency value; this member is
    // intended as just a convenience, and may be removed if it makes
    // trouble.
    
    // genealogy 
    virtual const TConsistencySet* Genealogy() const;
    
    // print
    virtual void PrintOn( std::ostream& o=std::cout ) const;

protected:
    
    EConsistentStatus fStat;
    EConsistentSign fSign;
    Double_t fValue;        // value of the consistency
    Double_t fLikelihood;   // value of the likelihood 
    
public:
    static const TConsistency& BadMeasurement();
    
    ClassDef(TConsistency,1) // Consistency of measurement
};

#endif




