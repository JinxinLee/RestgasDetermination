#ifndef IFDCOUNTER_HH
#define IFDCOUNTER_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdCounter.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//
// Bugs:
//    The class allows a defalt copy constructor and assignemtn op.
//    I'm not sure if this is right.  Since counters are supposed to
//    count the number of instances of the beasts exist, it doesn't
//    seem fair to assing one to another.  On the other hand, counters
//    may find use embedded inside other objects and I'm not sure
//    if we should allow an impact there by denying copies.
//    So, for now the user is on his own.  If he does a copy of one
//    of these, it will s.cxxeed...and he'll probably regret it.
//------------------------------------------------------------------------


// This class just counts up forever as instnaces are created.
// It does *not* count number of existing instances, i.e., there's
// no decrement in the destructor.
//


class IfdCounter {
public:
    IfdCounter();
    virtual ~IfdCounter();
    long value() const;
private:
    long& count(void) const;
    long  _myCount;
};
#endif /* IFDCOUNTER_HH */
