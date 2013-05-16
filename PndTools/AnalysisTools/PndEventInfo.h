#ifndef PNDEVENTINFO_H
#define PNDEVENTINFO_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndEventInfo                             //
//                                                                      //
// Contains and provides access to summary information in the event //
// useful for analysis                          //
//                                  //
//////////////////////////////////////////////////////////////////////////

//----------------------
// Base Class Headers --
//----------------------
#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "RhoMath/RhoVector3Err.h"

#include "RhoEventShape.h"

//      ---------------------
//      -- Class Interface --
//      ---------------------

class PndEventInfo : public TObject
{

    //--------------------
    // Instance Members --
    //--------------------
  public:

    // Constructors
    PndEventInfo();
    // there should eventually be a constructor that takes all the
    // various data items

    // Destructor
    virtual ~PndEventInfo( );

    // Operations
    virtual void PrintOn ( std::ostream& o=std::cout ) const;

    // accessors to contained information
    const TLorentzVector& AntiProtonBeam() const   {return fAntiProtonBeam;}
    const TLorentzVector& CmFrame() const   {return fCmFrame;}
    const RhoVector3Err& BeamSpot() const        {return fBeamSpot;}
    const RhoVector3Err& PrimaryVertex() const { return fPrimaryVertex;}
    const TVector3& IPTruth() const {return fIPTruth;}
//  const TEventShape& EventShape() const {return fEventShape;}

    int   GetCharged() {return fNCharged;}
    int   GetNeutrals() {return fNNeutrals;}

    // Function to set information
    void SetPrimaryVertex ( RhoVector3Err& inVtx ) { fPrimaryVertex=inVtx;}
    void SetIPTruth ( TVector3& inVtx ) {fIPTruth=inVtx;}
    void SetBeamSpot ( RhoVector3Err& inVtx ) { fBeamSpot=inVtx;}
    void SetCmFrame ( TLorentzVector& cmf );
    void SetAntiProtonBeam ( TLorentzVector& beam );

//    void SetEventShape(TEventShape &eventShape) {fEventShape=eventShape;}

    void SetCharged ( int n ) {fNCharged=n;}
    void SetNeutrals ( int n ) {fNNeutrals=n;}

  private:
    TLorentzVector  fAntiProtonBeam; // The antiproton beam
    TLorentzVector  fCmFrame;        // The CMS frame
    RhoVector3Err       fBeamSpot;       //! The beam spot w/ error matrix
    RhoVector3Err       fPrimaryVertex;  //! The primary vertex
    TVector3        fIPTruth;        // the MC truth IP
    Int_t           fNCharged;
    Int_t           fNNeutrals;
//  TEventShape     fEventShape;

  public:
    ClassDef ( PndEventInfo,1 ) // Summary information in the event
};

// standalone print
std::ostream&  operator << ( std::ostream& o, const PndEventInfo& );

#endif




