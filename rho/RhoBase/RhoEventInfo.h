#ifndef RHOEVENTINFO_H
#define RHOEVENTINFO_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoEventInfo                                                         //
//                                                                      //
// Contains and provides access to summary information in the event     //
// useful for analysis                                                  //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Dec. 2k                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//----------------------
// Base Class Headers --
//----------------------
#include "TObject.h"
#include "TVector3.h"
#include "RhoBase/RhoCandidate.h"
#include "RhoMath/RhoVector3Err.h"

#include "RhoBase/RhoEventShape.h"

//      ---------------------
//      -- Class Interface --
//      ---------------------

class RhoEventInfo : public TObject
{

    //--------------------
    // Instance Members --
    //--------------------

  public:

    // Constructors
    RhoEventInfo();
    // there should eventually be a constructor that takes all the
    // various data items

    // Destructor
    virtual ~RhoEventInfo( );

    // Operations
    virtual void PrintOn ( std::ostream& o=std::cout ) const;

    // accessors to contained information

    const TLorentzVector& CmFrame() const   {
      return fCmFrame;
    }

    const RhoVector3Err& BeamSpot() const        {
      return fBeamSpot;
    }
    const RhoVector3Err PrimaryVertex() const;
    const RhoVector3Err* PrimaryVtx() const {
      return fPrimaryVertex;
    }
    const TVector3* MCImpactPoint() const {
      return fMCIP;
    }

    const RhoEventShape* EventShape() const    {
      return fEventShape;
    }

    // Function to set fPrimaryVertex
    void SetPrimaryVertex ( RhoVector3Err* inVtx ) ;
    void SetIPTruth ( const TLorentzVector& inVtx ) ;
    void SetIPTruth ( const TVector3& inVtx ) ;
    void SetBeamSpot ( const RhoVector3Err& inVtx ) {
      fBeamSpot=inVtx;
    }
    void SetCmFrame ( const TLorentzVector& cmf ) {
      fCmFrame =cmf;
    }

    void SetEventShape ( RhoEventShape* eventShape );

  private:
    TLorentzVector fCmFrame; //!The CMS frame
    TVector3* fMCIP; //!The MC interaction point
    RhoVector3Err fBeamSpot; //!The beam spot w/ error matrix
    RhoVector3Err* fPrimaryVertex; //!The primary vertex

    RhoEventShape* fEventShape; //!The event shape

  public:
    ClassDef ( RhoEventInfo,1 ) // Summary information in the event
};

// standalone print
std::ostream&  operator << ( std::ostream& o, const RhoEventInfo& );

#endif
