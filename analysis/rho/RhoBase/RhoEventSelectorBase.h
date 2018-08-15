#ifndef RHOEVENTSELECTORBASE_H
#define RHOEVENTSELECTORBASE_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoEventSelectorBase                                                 //
//                                                                      //
// Selector classes for event selection                                 //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include <iostream>

#include "TString.h"
#include "TVector3.h"
#include "TTreeFormula.h"
#include "RhoBase/RhoCandList.h"
#include "RhoBase/RhoEventInfo.h"
#include "RhoSelector/RhoGoodTrackSelector.h"

// RhoEventSelector is the common base class for all the
// selectors in the hierarchy and manages the common settings.
// The default setup has all cuts disabled, i.e. all events are accepted.

class RhoEventSelectorBase : public TNamed
{

  public:

    //Constructor
    RhoEventSelectorBase ( const char* name = "RhoEventSelectorBase" ) :
      TNamed ( name,name ),
      fTight ( kFALSE ),
      fNTracksMin ( 0 ), fNTracksMax ( 65535 ), fTotChar ( 65535 ),
      fDocaMax ( 1.E8 ), fZMin ( 1.E8 ), fZMax ( 1.E8 ),
      fETotLow ( 0.0 ), fETotHigh ( 1.E8 ),
      fPTotLow ( 0.0 ), fPTotHigh ( 1.E8 ),
      fELow ( 0.0 ), fEHigh ( 1.E8 ),
      fEmcLow ( 0.0 ), fEmcHigh ( 1.E8 ),
      fPLow ( 0.0 ), fPHigh ( 1.E8 ),
      fPtLow ( 0.0 ), fPtHigh ( 1.E8 ),
      fThetaLow ( 0.0 ), fThetaHigh ( 2.*3.14159265358979323846 ),
      fPhiLow ( 0.0 ), fPhiHigh ( 2.*3.14159265358979323846 )
    {}

    //Destructor
    virtual ~RhoEventSelectorBase() {}

    //operations
    virtual Bool_t Accept ( RhoEventInfo& ) = 0;  // The basic selector functionality
    virtual void PrintOn ( std::ostream& o=std::cout ) const;        // Show the settings

    // All descendents work on the same basis...

    // General settings
    void SetTight() {
      fTight = kTRUE;
    };
    void SetLoose() {
      fTight = kFALSE;
    };
    // Properties of the event
    void SetNumberOfTracks ( UInt_t n1, UInt_t n2 = 65535 ) {
      fNTracksMin = n1;
      fNTracksMax = n2;
    }
    void SetPrimaryVertex ( Double_t d, Double_t zMin, Double_t zMax ) {
      fDocaMax = d;
      fZMin = zMin;
      fZMax = zMax;
    }
    void SetTotalCharge ( Int_t n ) {
      fTotChar = n;
    }
    void SetTotalEnergy ( Double_t e1, Double_t e2=1.E8 ) {
      fETotLow = e1;
      fETotHigh = e2;
    }
    void SetTotalMomentum ( Double_t p1, Double_t p2=1.E8 ) {
      fPTotLow = p1;
      fPTotHigh = p2;
    }
    // Properties of the tracks
    void SetEnergy ( Double_t e1, Double_t e2=1.E8 ) {
      fELow = e1;
      fEHigh = e2;
    }
    void SetShowerEnergy ( Double_t e1, Double_t e2=1.E8 ) {
      fEmcLow = e1;
      fEmcHigh = e2;
    }
    void SetMomentum ( Double_t p1, Double_t p2=1.E8 ) {
      fPLow = p1;
      fPHigh = p2;
    }
    void SetPt ( Double_t p1, Double_t p2=1.E8 ) {
      fPtLow = p1;
      fPtHigh = p2;
    }
    void SetTheta ( Double_t t1, Double_t t2=2.*3.14159265358979323846 ) {
      fThetaLow = t1;
      fThetaHigh = t2;
    }
    void SetPhi ( Double_t p1, Double_t p2=2.*3.14159265358979323846 ) {
      fPhiLow = p1;
      fPhiHigh = p2;
    }

  protected:
    Bool_t fTight;
    Int_t fNTracksMin, fNTracksMax, fTotChar;
    Double_t fDocaMax, fZMin, fZMax;
    Double_t fETotLow, fETotHigh, fPTotLow, fPTotHigh;
    Double_t fELow, fEHigh, fEmcLow, fEmcHigh, fPLow, fPHigh, fPtLow, fPtHigh;
    Double_t fThetaLow, fThetaHigh, fPhiLow, fPhiHigh;

  public:
    ClassDef ( RhoEventSelectorBase,1 )  // Event Selector base class
};

// standalone print
std::ostream&  operator << ( std::ostream& o, const RhoEventSelectorBase& );

#endif
