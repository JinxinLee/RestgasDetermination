//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOperatorBase.h,v 1.3 2002-02-01 03:54:57 marcel Exp $
//
// Description:
//	Class TOperatorBase
//	
//  Base for classes that are used to "combine" TCandidates
//  to make one or more new ones.
//
//  This class defines the interface for operators which _must_ return
//  a TCandidate of some polymorphic type in order to properly
//  function.  It's subclasses are used as
//
//      If b is a TOperatorBase* initialized from some subclass, then
//
//      To return a new'd object:
//         b->create(const TCandidate& in1, ...);
//
//  In the more general TOperator subclass, this interface is 
//  extended to add two more forms.  These can only be provided by
//  TOperator subclasses who's output type is TCandidate itself.
//
//      If b is a TOperatorBase* initialized from some subclass, then
//
//      To return a TCandidate by value:  (May be removed; see docs)
//         b->combine(const TCandidate& in1, ...);
//
//      To fill an existing object:  (May be removed; see docs)
//         b->fill(TCandidate& out, TCandidate& in1 ... );
//
//  Note that which to use can require some thought; see the documentation
//  for the specific subclasses.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bob Jacobsen		        Original Author
//      Gautier Hamel de Monchenault
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TOPERATORBASE_H
#define TOPERATORBASE_H

//----------------------
// Base Class Headers --
//----------------------
#include "RhoBase/TCandidate.h"

class VAbsVertex;

//		---------------------
// 		-- Class Interface --
//		---------------------

class TOperatorBase  
{

public:

  // Constructors

  // Destructor
  virtual ~TOperatorBase( );

  // Operations
  virtual void PrintOn( std::ostream& o=std::cout ) const;

  // public virtual functions
  virtual void SetP4Err(TCandidate* temp, TError err) const;

protected:

  // service members so subclasses 
  // can access the inside of an output object

  inline void AddDaughterLink( TCandidate& mother, 
			       const TCandidate* daughter ) const;
  
  inline void SetDecayVtx( TCandidate& inComing, 
			   VAbsVertex* vtx ) const;

  inline void AddToVertexingList( TCandidate& cand,
				  TCandList& list );

  inline void SetTypeLeaveMass(TCandidate * cand, 
			       const TParticlePDG * pdt) const;

  //TCandBase* contentOf(TCandidate& b) const 
  //{ return b.content(); }

  //const TCandBase* contentOf(const TCandidate& b) const 
  //{ return b.content(); }

  // access to the fit parameters
  //TFitParams& fitParams( TCandidate& b )
  //  { return b.content()->_params; }

  // sets the mother link of a TCandidate
  // to the mother of its local candidate.
  // (this works only for candidates with no mother)
  inline void SetMotherLinkToLocal( TCandidate& daughter ) const;

    ClassDef(TOperatorBase,1) // Base for classes that are used to "combine" TCandidates
};

// standalone print
std::ostream&  operator << (std::ostream& o, const TOperatorBase&);

void 
TOperatorBase::AddDaughterLink( TCandidate& mother, 
				const TCandidate* daughter ) const
{ 
  mother.AddDaughterLink( daughter ); 
}

void 
TOperatorBase::SetDecayVtx( TCandidate& inComing, 
			    VAbsVertex* vtx ) const
{ 
  inComing.SetDecayVtx( vtx );
}

void 
TOperatorBase::AddToVertexingList( TCandidate& cand,
				   TCandList& list )
{
  cand.AddToVertexingList( list );
}

// sets the mother link of a TCandidate
// to the mother of its local candidate.
// (this works only for candidates with no mother)
void 
TOperatorBase::SetMotherLinkToLocal( TCandidate& daughter ) const
{
  if( daughter.fTheMother==0 )
    {
      //daughter.fTheMother = daughter.thisCandidate()->fTheMother;
    }
}

void 
TOperatorBase::SetTypeLeaveMass(TCandidate * cand, 
				  const TParticlePDG * pdt) const {
  if(pdt == cand->fPdtEntry || pdt == 0) return;
  cand->fPdtEntry = pdt;
}


#endif


