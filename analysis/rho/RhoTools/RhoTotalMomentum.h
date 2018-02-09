//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//  Class RhoTotalMomentum - a simple object to calculate the total
//                               4 momentum in an event
//
// Environment:
//  Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Paul Harrison                         Original author
//
// Copyright Information:
//  Copyright (C) 1997                Hedgehog Concepts
//
// ROOT Version by Marcel Kunze, RUB
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              
//------------------------------------------------------------------------

#ifndef RHOTOTALMOMENTUM_H
#define RHOTOTALMOMENTUM_H

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
class RhoEventInfo;
class RhoCandidate;
#include "RhoBase/RhoCandList.h"
#include "TVector3.h"
#include "TLorentzVector.h"

//    ---------------------
//    -- Class Interface --
//    ---------------------

class RhoTotalMomentum : public TObject
{

//--------------------
// Instance Members --
//--------------------

  public:

    // Constructors
    RhoTotalMomentum ( RhoCandList&, const RhoEventInfo& );

    // Destructor
    virtual ~RhoTotalMomentum();

    // accessors to get at results

    inline double TotalEnergy() const { return _total4Momentum.T(); }
    inline double TotalMomentum() const { return TVector3 ( _total4Momentum.X(),_total4Momentum.Y(),_total4Momentum.Z() ).Mag(); }
    inline TVector3 Total3Momentum() const { return TVector3 ( _total4Momentum.X(),_total4Momentum.Y(),_total4Momentum.Z() ); }
    inline TLorentzVector Total4Momentum() const { return _total4Momentum; }

    inline double TotalEnergyCms() const { return _total4MomentumCms.T(); }
    inline double TotalMomentumCms() const { return TVector3 ( _total4MomentumCms.X(),_total4MomentumCms.Y(),_total4MomentumCms.Z() ).Mag(); }
    inline TVector3 Total3MomentumCms() const { return TVector3 ( _total4MomentumCms.X(),_total4MomentumCms.Y(),_total4MomentumCms.Z() ); }
    inline TLorentzVector Total4MomentumCms() const { return _total4MomentumCms; }

    inline double TotalMass() const { return _total4Momentum.Mag(); }

    inline double MissingEnergy() const { return _missing4Momentum.T(); }
    inline double MissingMomentum() const { return TVector3 ( _missing4Momentum.X(),_missing4Momentum.Y(),_missing4Momentum.Z() ).Mag(); }
    inline TVector3 Missing3Momentum() const { return TVector3 ( _missing4Momentum.X(),_missing4Momentum.Y(),_missing4Momentum.Z() ); }
    inline TLorentzVector Missing4Momentum() const { return _missing4Momentum; }

    inline double MissingEnergyCms() const { return _missing4MomentumCms.T(); }
    inline double MissingMomentumCms() const { return TVector3 ( _missing4MomentumCms.X(),_missing4MomentumCms.Y(),_missing4MomentumCms.Z() ).Mag(); }
    inline TVector3 Missing3MomentumCms() const { return TVector3 ( _missing4MomentumCms.X(),_missing4MomentumCms.Y(),_missing4MomentumCms.Z() ); }
    inline TLorentzVector Missing4MomentumCms() const { return _missing4MomentumCms; }

    inline double MissingMass() const { return _missing4Momentum.Mag(); }

  protected:

    // Member data
    TLorentzVector _total4Momentum; //!
    TLorentzVector _total4MomentumCms; //!

    TLorentzVector _missing4Momentum; //!
    TLorentzVector _missing4MomentumCms; //!

  public:
    ClassDef ( RhoTotalMomentum,1 ) // A simple object to calculate the total 4 momentum in an event
};

#endif
