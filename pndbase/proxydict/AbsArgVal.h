#ifndef ABSARGVAL_HH
#define ABSARGVAL_HH
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AbsArgVal.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//      AbsArgVal.h is forced to implement all the meothds because of DEC cxx.
//      However, HP-UX complains about no .cxx file.  So here we have
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

// Notes:
//   1.  AbsArgVal do NOT OWN the T* that you build them with.  AbsArgs should
//        be thought of a a wrapper around the T* they  are built from.
//
//   2.   I expect common usage to look like
//           T myT;
//           void f( AbsArg );         // typical client signature.
//
//           f( AbsArgVal<T>( T ) );   //  call semantics.
//
//        Notice thta the signature is in terms of the abstract base class, but
//        we build a wrapper (concrete derivative) to do the call.  In
//        order to speed  this up, AbsArgVal<T> has _myKey as a class 
//        variable (static).
//
//    3.  forget about const-ness.  It's impossible to do (AbsArg is really
//        just a smart pointer, at heart).  Be happy you've got type saftey 
//        rather than void*.



#include "AbsArg.h"

class IfdTypeKeyIFace;

template <class T> class AbsArgCast;

template <class T>
class AbsArgVal : public AbsArg {
public:
  //  Note: The workaround to a VxWorks template instantiation bug requires
  // that  the destructor be the first function defined in a templated 
  // class.  Please do not move this or put functions above it!

  virtual ~AbsArgVal();
  AbsArgVal( T* t);
  AbsArgVal( T& t);

  virtual const IfdTypeKeyIFace&  getTypeKey() const;
  virtual AbsArg* clone() const;

private:
  T* _myT;
  friend class AbsArgCast<T>;
};

#include  "AbsArgVal.cxx"

#endif //ABSARGVAL_HH
