//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdIntKey.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    Implementation of IfdIntKey.  See .h for details.
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      19 Jan 00         Creation of first version by moving
//                                    old code from .h to here.
//
// Copyright Information:
//      Copyright (C) 1997
//
// Bugs:
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

#include "IfdIntKey.h"


IfdIntKey::IfdIntKey( const int i ) 
  : IfdKey( intKey )
{
   intVal   =  i; 
   _hashVal = i % _nHashBuckets;
}

