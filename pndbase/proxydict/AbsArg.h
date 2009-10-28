#ifndef ABSARG_HH
#define ABSARG_HH
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AbsArg.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//	Abstract arguments to use rather than void* because they are type
//      safe.  See also AbsArgVal.h and AbsArgCast.h.
//
//      Comments on usage:
//
//         1.  If "AbsArg a,b" then a.getTypeKey() == b.getTypeKey if and
//             only if the underlying AbsArgVal<T> for a and b use exactly
//             the same Key.  Normal users do not need to use getTypeKey().
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Ed Frank		University of Pennsylvania
//
// Copyright Information:
//	Copyright (C) 1994	
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

class IfdTypeKeyIFace;

class AbsArg {
public:
  virtual ~AbsArg();
  virtual const IfdTypeKeyIFace& getTypeKey() const =0;
  virtual AbsArg* clone() const = 0;

protected:
  AbsArg( );
};


#endif //ABSARG_HH
