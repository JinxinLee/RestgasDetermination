#ifndef TEVENTINFO_H
#define TEVENTINFO_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TEventInfo								//
//                                                                      //
// Contains and provides access to summary information in the event	//
// useful for analysis							//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Dec. 2k						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

//----------------------
// Base Class Headers --
//----------------------
#include "TObject.h"
#include "TVector3.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsVertex.h"
#include "RhoMath/TPointErr.h"

class TEventShape;

//		---------------------
// 		-- Class Interface --
//		---------------------

class TEventInfo : public TObject {
    
    //--------------------
    // Instance Members --
    //--------------------
    
public:
    
    // Constructors
    TEventInfo();
    // there should eventually be a constructor that takes all the
    // various data items
    
    // Destructor
    virtual ~TEventInfo( );
    
    // Operations
   virtual void PrintOn( std::ostream& o=std::cout ) const;

    // accessors to contained information
    const TCandidate& PositronBeam() const   {return fPositronBeam;}
    const TCandidate& ElectronBeam() const   {return fElectronBeam;}
    
    const TLorentzVector& CmFrame() const   {return fCmFrame;}
    
    const TPointErr& BeamSpot() const        {return fBeamSpot;}
    const TPointErr& BeamSpotBFlight() const        {return fBeamSpotBFlight;}
    const TPointErr PrimaryVertex() const;
    const VAbsVertex* PrimaryVtx() const { return fPrimaryVertex;}
    const TVector3* MCImpactPoint() const{ return fMCIP;}
    
    const TEventShape* EventShape() const    {return fEventShape;}
    
    // Function to set fPrimaryVertex
    void SetPrimaryVertex(VAbsVertex* inVtx) ;
    void SetIPTruth(const TLorentzVector& inVtx) ;
    void SetIPTruth(const TVector3& inVtx) ;
    void SetBeamSpot(const TPointErr& inVtx) {fBeamSpot=inVtx;}
    void SetBeamSpotBFlight(const TPointErr& inVtx) {fBeamSpotBFlight=inVtx;}
    void SetPepBeams(const TCandidate& ele,const TCandidate& pos) {fElectronBeam=ele;fPositronBeam=pos;}
    void SetCmFrame(const TLorentzVector& cmf){fCmFrame =cmf;}
    
    void SetEventShape(TEventShape* eventShape);
    
private:
    TCandidate fPositronBeam; //!The positron beam
    TCandidate fElectronBeam; //!The electron beam
    TLorentzVector fCmFrame; //!The CMS frame
    TVector3* fMCIP; //!The MC interaction point
    TPointErr fBeamSpot; //!The beam spot w/ error matrix
    TPointErr fBeamSpotBFlight; //!
    VAbsVertex* fPrimaryVertex; //!The primary vertex
    
    TEventShape* fEventShape; //!The event shape
    
public:
    ClassDef(TEventInfo,1) // Summary information in the event
};

// standalone print
std::ostream&  operator << (std::ostream& o, const TEventInfo&);

#endif




