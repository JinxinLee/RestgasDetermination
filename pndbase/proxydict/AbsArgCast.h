#ifndef ABSARGCAST_HH
#define ABSARGCAST_HH
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AbsArgCast.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//      AbsArgCast.h is forced to implement all the meothds because of DEC 
//      cxx.  However, HP-UX complains about no .cxx file.  So here we have
//      an empty one.
//      
//      EXCLUDE THIS FILE FROM THE LIST OF .CC FILE TO COMPILE!
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Ed Frank		University of Pennsylvania
//
// Copyright Information:
//	Copyright (C) 1997
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

class AbsArg;

template <class T>
class AbsArgCast {

public:
  static T* value( AbsArg& a );
  static T* value( AbsArg* a );

};

 
#include  "AbsArgCast.cxx"


#endif //ABSARGCAST_HH
