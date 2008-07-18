#ifndef TEVENTSELECTOR_H
#define TEVENTSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TEventSelector						        //
//                                                                      //
// Selector classes for event selection                                 //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// There are two branches in the class hierarchy:
// - TagSelectors define rough cuts to be applied at the TAG level
// - AodSelectors define more refined algorithms to be applied at the AOD level

#include <iostream>

#include "TString.h"
#include "TVector3.h"
#include "TTreeFormula.h"
#include "RhoBase/TCandList.h"
#include "RhoManager/TEventManager.h"
#include "RhoSelector/TPidSelector.h"
#include "RhoSelector/TGoodTrackSelector.h"

// TEventSelector is the common base class for all the
// selectors in the hierarchy and manages the common settings.
// The default setup has all cuts disabled, i.e. all events are accepted.

class TEventSelector : public TNamed {
    
public:
    
    //Constructor
    TEventSelector(const char *name = "TEventSelector") :
    TNamed(name,name),
    fTight(kFALSE), 
    fNTracksMin(0), fNTracksMax(65535), fTotChar(65535), 
    fDocaMax(1.E8), fZMin(1.E8), fZMax(1.E8),
    fETotLow(0.0), fETotHigh(1.E8), 
    fPTotLow(0.0), fPTotHigh(1.E8), 
    fELow(0.0), fEHigh(1.E8), 
    fEmcLow(0.0), fEmcHigh(1.E8), 
    fPLow(0.0), fPHigh(1.E8), 
    fPtLow(0.0), fPtHigh(1.E8), 
    fThetaLow(0.0), fThetaHigh(2.*3.14159265358979323846), 
    fPhiLow(0.0), fPhiHigh(2.*3.14159265358979323846)
    {}

    //Destructor
      virtual ~TEventSelector() {}
    
    //operations
    virtual Bool_t Accept(TEventManager&) = 0;  // The basic selector functionality
    virtual void PrintOn( std::ostream& o=std::cout ) const;         // Show the settings

    // All descendents work on the same basis...

    // General settings
    void SetTight() { fTight = kTRUE; };
    void SetLoose() { fTight = kFALSE; };
    // Properties of the event
    void SetNumberOfTracks(UInt_t n1, UInt_t n2 = 65535) { fNTracksMin = n1; fNTracksMax = n2; }
    void SetPrimaryVertex(Double_t d, Double_t zMin, Double_t zMax) { fDocaMax = d; fZMin = zMin; fZMax = zMax; }
    void SetTotalCharge(Int_t n) { fTotChar = n; }
    void SetTotalEnergy(Double_t e1, Double_t e2=1.E8) { fETotLow = e1; fETotHigh = e2; }
    void SetTotalMomentum(Double_t p1, Double_t p2=1.E8) { fPTotLow = p1; fPTotHigh = p2; }
    // Properties of the tracks
    void SetEnergy(Double_t e1, Double_t e2=1.E8) { fELow = e1; fEHigh = e2; }
    void SetShowerEnergy(Double_t e1, Double_t e2=1.E8) { fEmcLow = e1; fEmcHigh = e2; }
    void SetMomentum(Double_t p1, Double_t p2=1.E8) { fPLow = p1; fPHigh = p2; }
    void SetPt(Double_t p1, Double_t p2=1.E8) { fPtLow = p1; fPtHigh = p2; }
    void SetTheta(Double_t t1, Double_t t2=2.*3.14159265358979323846) { fThetaLow = t1; fThetaHigh = t2; }
    void SetPhi(Double_t p1, Double_t p2=2.*3.14159265358979323846) { fPhiLow = p1; fPhiHigh = p2; }

protected:
    Bool_t fTight;
    Int_t fNTracksMin, fNTracksMax, fTotChar;
    Double_t fETotLow, fETotHigh, fPTotLow, fPTotHigh;
    Double_t fDocaMax, fZMin, fZMax;
    Double_t fELow, fEHigh, fEmcLow, fEmcHigh, fPLow, fPHigh, fPtLow, fPtHigh;
    Double_t fThetaLow, fThetaHigh, fPhiLow, fPhiHigh;

public:
    ClassDef(TEventSelector,1)  // Event Selector base class
};

// standalone print
std::ostream&  operator <<(std::ostream& o, const TEventSelector&);

// TTagBaseSelector is the common base class for all the TAG based
// selectors in the hierarchy and manages the common settings.

class TTagBaseSelector : public TEventSelector {
    
public:
    //Constructor
    TTagBaseSelector(const char *name="TTagBaseSelector") :
    TEventSelector(name),
    fTreeFormula(0),
    fPTotalMass(0.0), fPTotalCosTh(-1.0), fENeutralCosTh(-1.0),
    fP1MagLow(0.0),fP1MagHigh(1.E8),fP2MagLow(0.0),fP2MagHigh(1.E8),
    fE1MagLow(0.0),fE1MagHigh(1.E8),fE2MagLow(0.0),fE2MagHigh(1.E8)
    {}

    //Destructor
    virtual ~TTagBaseSelector() { delete fTreeFormula; }
    
    //operations
    virtual Bool_t Accept(TEventManager&) = 0;
    virtual void PrintOn( std::ostream& o=std::cout ) const;
    void SetFormula(const char* f) { fFormula = f; }
    void SetTotalMass(Double_t m) { fPTotalMass = m; }
    void SetTotalCosTheta(Double_t t) { fPTotalCosTh = t; }
    void SetNeutralCosTheta(Double_t t) { fENeutralCosTh = t; }
    void SetPrimaryMomentum(Double_t pl, Double_t ph=1.E8) { fP1MagLow = pl; fP1MagHigh = ph; }
    void SetSecondaryMomentum(Double_t pl, Double_t ph=1.E8) { fP2MagLow = pl; fP2MagHigh = ph; }
    void SetPrimaryEnergy(Double_t el, Double_t eh=1.E8) { fE1MagLow = el; fE1MagHigh = eh; }
    void SetSecondaryEnergy(Double_t el, Double_t eh=1.E8) { fE2MagLow = el; fE2MagHigh = eh; }
    
protected:
    TTreeFormula *fTreeFormula; // !The actual formula
    TString  fFormula;
    Double_t fPTotalMass, fPTotalCosTh, fENeutralCosTh;
    Double_t fP1MagLow,fP1MagHigh,fP2MagLow,fP2MagHigh;
    Double_t fE1MagLow,fE1MagHigh,fE2MagLow,fE2MagHigh;

public:
    ClassDef(TTagBaseSelector,1)  // Tag Selector base class
};      
  
// standalone print
std::ostream&  operator <<(std::ostream& o, const TTagBaseSelector&);

// TAodBaseSelector is the common base class for all the AOD based
// selectors in the hierarchy and manages the common settings.
// It uses a TGoodTrackSelector to look for good reconstructed tracks.
// See http://www.slac.stanford.edu/BFROOT/www/Physics/Forum/forum/phonemeetings/forum_15jun99/trkselection.html
// Note: 
// There is no cut on chisquared or associated EMC deposit required to
// define a good track.

class TAodBaseSelector : public TEventSelector {
    
public:
    
    //Constructor
    TAodBaseSelector(const char *name="TAodBaseSelector");

    //Destructor
    virtual ~TAodBaseSelector();
    
    //operations
    virtual Bool_t Accept(TEventManager&) = 0;  // The basic functionality
    virtual void PrintOn( std::ostream& o=std::cout ) const = 0;     // Show the settings

    // All decendents work on the same basis...
    void SetSelector(TGoodTrackSelector &s) { fSelector = &s; }
    TGoodTrackSelector& GetSelector() { return *fSelector; }

    void SetPrimaryVertex(Double_t d, Double_t zMin, Double_t zMax)
    { 
	TEventSelector::SetPrimaryVertex(d,zMin,zMax);
	fSelector->SetPrimaryVertex(d,zMin,zMax);
    }

    void SetMomentum(Double_t p1, Double_t p2) 
    { 
	TEventSelector::SetMomentum(p1,p2);
	fSelector->SetMomentum(p1,p2);
    }

    void SetPt(Double_t p1, Double_t p2) 
    { 
	TEventSelector::SetPt(p1,p2);
	fSelector->SetPt(p1,p2);
    }

    void SetShowerEnergy(Double_t e1, Double_t e2) 
    { 
	TEventSelector::SetShowerEnergy(e1,e2);
	fSelector->SetShowerEnergy(e1,e2);
    }

    void SetTheta(Double_t t1, Double_t t2) 
    {
	TEventSelector::SetTheta(t1, t2);
	fSelector->SetTheta(t1, t2);
    }

    void SetPhi(Double_t p1, Double_t p2) 
    {
	TEventSelector::SetPhi(p1, p2);
	fSelector->SetPhi(p1, p2);
    }
    
protected:
    TGoodTrackSelector *fSelector; //! Do not stream
    TGoodTrackSelector *fOwnSelector; //! Do not stream

public:
    ClassDef(TAodBaseSelector,1)  // MicroDST Selector base class
};

// standalone print
std::ostream&  operator <<(std::ostream& o, const TAodBaseSelector&);

// TTagSelector is used to define a cut based on a TTreeFormula

class TTagSelector : public TTagBaseSelector {
    
public:
    //Constructor
    TTagSelector(const char *name="TTagSelector",const char* f="");

    //Destructor
    virtual ~TTagSelector() {}
    
    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;

private:

public:
    ClassDef(TTagSelector,1)  // TTreeFormula event selector
};      
  
// standalone print
std::ostream&  operator <<(std::ostream& o, const TTagSelector&);

// Define rough cuts to be applied at the TAG level
// see: http://babar-hn.slac.stanford.edu:5090/HyperNews/get/physAnal/223.html
// Note: The selector wakes up with Yury's cuts

class TTwoprongTagSelector : public TTagBaseSelector {
    
public:
    //Constructor
    TTwoprongTagSelector(const char *name = "TTwoprongTagSelector");

    //Destructor
    virtual ~TTwoprongTagSelector() {}
    
    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;
    void SetDeltaPhi(Double_t p) { fDPhi = p; }
    void SetDeltaCosTheta(Double_t c) { fDCosTh = c; }
    
private:
    Double_t fDPhi, fDCosTh;

public:
    ClassDef(TTwoprongTagSelector,1)  // Twoprong event selector
};      
  
// standalone print
std::ostream&  operator <<(std::ostream& o, const TTwoprongTagSelector&);

// Define rough cuts for dimuon selection to be applied at the TAG level
// See http://www.slac.stanford.edu/BFROOT/~rfaccini/phys/Y4Sscan/selections.html

class TDimuonTagSelector : public TTagBaseSelector
{
    
public:
    //Constructor
    TDimuonTagSelector(const char *name = "TDimuonTagSelector");

    //Destructor
    virtual ~TDimuonTagSelector() {}
    
    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;

public:
    ClassDef(TDimuonTagSelector,1)  // Dimuon event selector
};      
  
// standalone print
std::ostream&  operator <<(std::ostream& o, const TDimuonTagSelector&);

// Define rough cuts for dimuon selection to be applied at the TAG level
// for a two photon selector

class TGammaGammaTagSelector : public TTagBaseSelector
{
    
public:
    //Constructor
    TGammaGammaTagSelector(const char *name = "TGammaGammaTagSelector");

    //Destructor
    virtual ~TGammaGammaTagSelector() {}
    
    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;

public:
    ClassDef(TGammaGammaTagSelector,1)  // Gamma Gamma event selector
};      
  
// standalone print
std::ostream&  operator <<(std::ostream& o, const TGammaGammaTagSelector&);

// Bhabha selector

class TBhabhaTagSelector : public TTagBaseSelector {
    
public:
    //Constructor
    TBhabhaTagSelector(const char *name = "TBhabhaTagSelector");

    //Destructor
    virtual ~TBhabhaTagSelector() {}
    
    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;

public:
    ClassDef(TBhabhaTagSelector,1)  // Bhabha event selector
};      

// standalone print
std::ostream&  operator <<(std::ostream& o, const TBhabhaTagSelector&);


// Define rough cuts to be applied at the TAG level
// see: http://babar-hn.slac.stanford.edu:5090/HyperNews/get/physAnal/223.html

class TBhabhaRatioTagSelector : public TTwoprongTagSelector {
    
public:
    //Constructor
    TBhabhaRatioTagSelector(const char *name = "TBhabhaRatioTagSelector");

    //Destructor
    virtual ~TBhabhaRatioTagSelector() {}
    
    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;
    void SetAngularAcceptance(Double_t c) { _costh = c; }
    void SetEnergyMomentumRatio(Double_t r) { _epratio = r; }
    
private:
    Double_t _costh,_epratio;

public:
    ClassDef(TBhabhaRatioTagSelector,1)  // Another Bhabha event selector
};      

// standalone print
std::ostream&  operator <<(std::ostream& o, const TBhabhaRatioTagSelector&);


// Define rough cuts to be applied at the TAG level
// see: http://babar-hn.slac.stanford.edu:5090/HyperNews/get/physAnal/223.html

class TRadiativeBhabhaTagSelector : public TTwoprongTagSelector {
    
public:
    //Constructor
    TRadiativeBhabhaTagSelector(const char *name = "TRadiativeBhabhaTagSelector")
    { SetName(name); }
    //Destructor
    virtual ~TRadiativeBhabhaTagSelector() {}
    
    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;

public:
    ClassDef(TRadiativeBhabhaTagSelector,1)  // Radiative Bhabha event selector
};      

// standalone print
std::ostream&  operator <<(std::ostream& o, const TRadiativeBhabhaTagSelector&);

// Define rough cuts to be applied at the TAG level
// see: http://babar-hn.slac.stanford.edu:5090/HyperNews/get/physAnal/223.html
// see: http://babar-hn.slac.stanford.edu:5090/HyperNews/get/physAnal/275.html
// Note: The selector wakes up with Yury's cuts
  
class THadronTagSelector : public TTagBaseSelector {
    
public:
    //Constructor
    THadronTagSelector(const char *name = "THadronTagSelector");

    //Destructor
    virtual ~THadronTagSelector() {}
    
    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;

public:
    ClassDef(THadronTagSelector,1)  // Hadronic event selector
};      
  
// standalone print
std::ostream&  operator <<(std::ostream& o, const THadronTagSelector&);

// Perform the "Rome" selection for hadronic events:
// Define more refined cuts to be applied at the Micro level ("Rome selection")
// see http://babar-hn.slac.stanford.edu:5090/HyperNews/get/physAnal/216.html

class THadronAodSelector : public TAodBaseSelector {
    
public:
    //Constructor
    THadronAodSelector(const char *name = "THadronAodSelector");

    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;
    void SetHighPt(Double_t p) { fHighPt = p; }

private:
    Double_t fHighPt;
    
public:
    ClassDef(THadronAodSelector,1)  // Hadronic event selector
};

// standalone print
std::ostream&  operator <<(std::ostream& o, const THadronAodSelector&);


// There is as well a more refined dimuon selector based on the AOD
// Idea: Look for two tracks with energy deposit in the EMC
// and require the tracks back to back
// In order to not accept cosmics, dcos(theta) should be set to kPI.

class TDimuonAodSelector : public TAodBaseSelector {
    
public:
    //Constructor
    TDimuonAodSelector(const char *name = "TDimuonAodSelector");

    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;
    void SetDeltaPhi(Double_t p) { fDPhi = p; }
    void SetDeltaCosTheta(Double_t c) { fDCosTh = c; }
    
private:
    TCandList tracklist;
    Double_t fDPhi, fDCosTh;
    
public:
    ClassDef(TDimuonAodSelector,1)  // Dimuon event selector
};

// standalone print
std::ostream&  operator <<(std::ostream& o, const TDimuonAodSelector&);

// Bhabha selector based on the AOD (two leg analysis)

class TBhabhaAodSelector : public TAodBaseSelector {
    
public:
    //Constructor
    TBhabhaAodSelector(const char *name = "TBhabhaAodSelector");

    //operations
    virtual Bool_t Accept(TEventManager&);
    virtual void PrintOn( std::ostream& o=std::cout ) const;

public:
    ClassDef(TBhabhaAodSelector,1)  // Bhabha event selector
};

// standalone print
std::ostream&  operator <<(std::ostream& o, const TBhabhaAodSelector&);

#endif
