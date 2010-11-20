#ifndef IFDTYPEKEY_HH
#define IFDTYPEKEY_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdTypeKey.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    An IfdKey that defines op==() in terms of "same class."  No consideration
//    for inheritance hierarchies is made, i.e., if IfdTypeKey<T1> t1, and
//    IfdTypeKey<T2> t2, then t1 == t2 iff T1 and T2 are identical class
//    names.
//
//    Trickiness:  we need static members (class data) to implement this,
//    but not all compilers support static data in templated classes. We
//    move the static data from class scope to function scope by creating
//    a method to manipulate the static data, and then putting the static
//    data into the method (setMyId()).  This makes all compilers tested
//    so far happy.
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//    Ed Frank      22 Nov 96         Put definition of class statics into
//                                    this file rather than a .cxx file
//                                    for compilation w/ xlC. I've removed
//                                    the #ifdef and we'll see if all systems
//                                    are happy with it.
//    Ed Frank      02 Feb 97         Support compilers that lack static data
//                                    in templated functions, e.g., g++. via
//                                    explicit instantiation (specialization).
//    Ed Frank      18 Sep 97        -Support compilers that lack static data
//                                    by moving the class static into setMyId()
//                                    Now specialization is not needed.
//                                   -Rename NO_STATICS_IN_TEMPLATES to
//                                    be IFDTYPEKEY_SPECIALIZE.
//    Ed Frank      10 Jul 03        -Rip out the IFDTYPKEY_SPECIALIZE
//                                    
//  
// Bugs:
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

#include "IfdTypeKeyIFace.h"
#include "IfdKey.h"
#include "IfdCounter.h"

#include <string>
#include <stdlib.h>
#include <iostream>
#include <ostream>
#include <cstring>

using namespace std;
class IfdKey;

template <class T>
class IfdTypeKey : public IfdTypeKeyIFace {
// class IfdTypeKey : public IfdKey{
public:
//  Note: The workaround to a VxWorks template instantiation bug requires that
//  the destructor be the first function defined in a templated class.  Please
//  do not move this or put functions above it!
    virtual ~IfdTypeKey() {}

    IfdTypeKey();



    virtual IfdKey*  clone( void ) const { return new IfdTypeKey<T>(); }

       // All IfdTypeKey<T>'s (for a given T) are really identical,
       // so just new one off and return it.

  static char* name( const char* newName );
  // Our name as a convenient string.
  
  virtual void print( ostream &o ) const;


protected:

private:

    void setMyId(void);
      
//     Copy ctor and assignemnt op are not allowed.  This keeps
//     the class behavior like its base, IfdKey.  See that class for
//     more info.
    IfdTypeKey( const IfdTypeKey<T>& ) { ::abort(); }
    IfdTypeKey<T>& operator= ( const IfdTypeKey<T>& )
       { ::abort(); return *this; }
};


#include  "IfdTypeKey.cxx"

#endif /*  IFDTYPEKEY_HH */
