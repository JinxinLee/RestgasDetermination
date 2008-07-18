//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EmcSharedDigi.hh,v 1.3 2005/11/15 01:20:20 steinke Exp $
//
// Description:
//	Class Template for EmcClBump
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Stephen J. Gowdy         University of Edinburgh
//
// History (add to end):
//      S.J. Gowdy    19June96 - Created to inherit from EmcCluster
//
//------------------------------------------------------------------------

#ifndef PNDEMCSHAREDDIGI_HH
#define PNDEMCSHAREDDIGI_HH

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PndEmcDigi.h"


//		---------------------
// 		-- Class Interface --
//		---------------------

class PndEmcSharedDigi : public PndEmcDigi
{

public:

  // Constructors
  PndEmcSharedDigi( const PndEmcDigi &theDigi, const Double_t weight );
  PndEmcSharedDigi( const PndEmcSharedDigi &theDigi );

  // Destructor

  virtual ~PndEmcSharedDigi();

  // Unowned clone
  virtual PndEmcDigi* clone() const;
  
  // Operators
  virtual void weight( Double_t newWeight );

  // Selectors (const)
  virtual Double_t GetEnergy() const {return _digi->GetEnergy()*weight();}
  virtual Double_t weight() const {return _weight;}

    // Dynamic Cast methods
  virtual PndEmcSharedDigi* dynamic_cast_PndEmcSharedDigi();
  virtual const PndEmcSharedDigi* dynamic_cast_PndEmcSharedDigi() const;


private:

  Double_t _weight;
  PndEmcDigi* _digi;

  // not implemented
  PndEmcSharedDigi();
  PndEmcSharedDigi& operator=(const PndEmcSharedDigi&);

};

#endif // PNDEMCSHAREDDIGI_HH
