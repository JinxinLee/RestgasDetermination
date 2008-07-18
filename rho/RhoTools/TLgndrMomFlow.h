//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//      
//	Class TLgndrMomFlow - a simple object to calculate the 
//      Legandre expansion of the track-vs-thrast angle weighted 
//      by the track/cluster momentum 
//        
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Vasia Shelkov
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TLGNDRMOMFLOW_H
#define TLGNDRMOMFLOW_H

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
class TCandidate;
class TBooster;
class TCandList;

#include "TVector3.h"
#include "TVectorD.h"

//		---------------------
// 		-- Class Interface --
//		---------------------

class TLgndrMomFlow  : public TObject {
    
public:
    
    // Constructor
    TLgndrMomFlow( Int_t nb_terms = 6);
    
    // Destructor
    virtual ~TLgndrMomFlow();
    
    // member functions
    void Compute( const TCandList& list, const TVector3& axis );
    void BoostAndCompute( const TCandList& list,
	const TVector3& axis, TBooster* booster );
    void Reset();
    
    // default legandre term weighted by momentum
    const TVectorD& LgndrTerms()    const  { return _p1array; }   
    // terms weighted by momentum^0, momentum^1, momentum^2
    const TVectorD& LgndrTermsP0()  const  { return _p0array; }   
    const TVectorD& LgndrTermsP1()  const  { return _p1array; }   
    const TVectorD& LgndrTermsP2()  const  { return _p2array; }
    
    // default legandre term weighted by momentum
    const Double_t& Term(Int_t order) const  { return _p1array(order); }
    // terms weighted by momentum^0, momentum^1, momentum^2
    const Double_t& TermP0(Int_t order) const  { return _p0array(order); }
    const Double_t& TermP1(Int_t order) const  { return _p1array(order); }
    const Double_t& TermP2(Int_t order) const  { return _p2array(order); }
    
    
private:
    
    // Member data 
    const Int_t _nterms; //!
    TVectorD _p0array; //!
    TVectorD _p1array; //!
    TVectorD _p2array; //!
    TVectorD _trackarray; //!
    // recursive Legandre terms
    Double_t TrackTerms(Int_t n, Double_t X);
    
    ClassDef(TLgndrMomFlow,1) // Calculate the Legandre expansion of the track-vs-thrast angle
	
};

#endif
