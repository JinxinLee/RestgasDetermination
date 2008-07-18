//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class TSphericity - a simple object to calculate sphericity
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Paul Harrison                    Original author
//      Bob Jacobsen                     Update 
//      Sebong Chun
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------
#ifndef TSPHERICITY_H
#define TSPHERICITY_H

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
class TEventInfo;
class TCandidate;
#include "RhoBase/TCandList.h"
#include "TVector3.h"
#include "TVector.h"

//		---------------------
// 		-- Class Interface --
//		---------------------
 
class TSphericity  : public TObject {

//--------------------
// Instance Members --
//--------------------

public:

    // Constructors
    TSphericity(TCandList&, const TEventInfo&,
		  Bool_t chargedTracksOnly = kTRUE);

    // Destructor
    virtual ~TSphericity();

    // accessors to get at results

    double Sph() const { return _sph; }
    double Apl() const { return _apl; }
    double Pla() const { return _pla; }
    TVector3 SphAx() const { return _sphAx; }
    TVector3 SecAx() const { return _secAx; }
    TVector3 ThrAx() const { return _thrAx; }
    TVector EValues() const {return _E; }

protected:

    // Member data 
    double _sph; //!
    double _apl; //!
    double _pla; //!
    TVector3 _sphAx; //!
    TVector3 _secAx; //!
    TVector3 _thrAx; //!
    TVector _E;  //!
    
public:
    ClassDef(TSphericity,1) // A simple object to calculate sphericity
};

#endif
