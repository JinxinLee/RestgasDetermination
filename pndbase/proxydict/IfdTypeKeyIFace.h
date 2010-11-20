#ifndef IFDTYPEKEYIFACE_HH
#define IFDTYPEKEYIFACE_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdTypeKeyIFace.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//    Ed Frank      02 Dec 97         -Make abstract via pure virt dtor
//                                    -Add dumpName()
//                                    -Move print impl. into derivatives
//                                    -Move stuff to .cxx (no longer inline)
//
// Bugs:
//    I'd like to have print(ostream&) pure virtual, but then the Sun debugger
//    seems unable to function.
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

#include <iostream>
#include <stdlib.h>

#include "IfdKey.h"

using namespace std;
// A class that represents a generic TypeKey<T>.  This lets you
// express the idea "not just a Key, but a TypeKey (although i don't
// care which TypeKey<T> it is)"  See DictKey.
//

class IfdTypeKeyIFace : public IfdKey {
public:
    virtual ~IfdTypeKeyIFace();

    // Implement the required members from Key
    //
    virtual int operator==( const IfdKey &k ) const
       { return  ( typeKey == k.getKeyKind()) && ( intVal == k.intVal ); }

    virtual void print( ostream &o ) const;


       // Print ourselves to an ostream. See IfdTypeKey.h.cxx.  Operator
       // << in ostream uses this, so no terminating endl;

    void dumpName( ostream& o ) const;

       // just print() with a newline to force a flush.  Intended for
       // use in a debugger.

protected:
    IfdTypeKeyIFace();  //IfdTypeKeyIFaces are only to be created as TypeKeys
    // virtual keyKind getKeyKind( void ) const { return typeKey; }
private:
    // Copy ctor and assignemnt op are not allowed.  This keeps
    // the class behavior like its base, IfdKey.  See that class for
    // more info.
    IfdTypeKeyIFace( const IfdTypeKeyIFace &);
    IfdTypeKeyIFace& operator=( IfdTypeKeyIFace &);
};







#endif /*  IFDTYPEKEYIFACE_HH */
