//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOpAdd4.h,v 1.3 2002-02-01 03:54:56 marcel Exp $
//
// Description:
//	Class TOpAdd4
//	
//  Child class of TOperator that works by adding 4-monemta 
//  at the origin.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bob Jacobsen		        Original Author
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TOPADD4_H
#define TOPADD4_H

//----------------------
// Base Class Headers --
//----------------------
class TCandidate;
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoTools/TOperator.h"

//		---------------------
// 		-- Class Interface --
//		---------------------

class TOpAdd4  : public TOperator {

//--------------------
// Instance Members --
//--------------------
public:

    // Constructors

    // Destructor
    virtual ~TOpAdd4( );

    // Operations
    virtual void PrintOn( std::ostream& o=std::cout ) const;

    // the following _must_ be overriden
    virtual TCandidate* CreateFromList( 
			 TCandListIterator &input );

    //virtual TCandidate* createFromList( const RWTPtrVector<TCandidate> &input );

    ClassDef(TOpAdd4,1) // TOperator that works by adding 4-monemta
};

// standalone print
std::ostream&  operator << (std::ostream& o, const TOpAdd4&);

#endif




