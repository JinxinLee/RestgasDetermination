//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdCounter.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         -Creation of first version
//    Ed Frank      07 Jan 98         -Remove static data from class by
//                                     moving it into count(void).
//                                    -Move everything from inline (oops) to
//                                     out of line.
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//
//------------------------------------------------------------------------

#include "IfdCounter.h"


long&
IfdCounter::count( void ) const {
  // Acts as class data but without having
  // the static live in the global namespace.

  static long _counter = 0;
  return _counter;
}

IfdCounter::IfdCounter() {
  _myCount = count();
  count() = count() + 1;
}

IfdCounter::~IfdCounter()
{}

long
IfdCounter::value() const {
  return _myCount;
}

