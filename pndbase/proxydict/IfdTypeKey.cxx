//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdTypeKey.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//    Ed Frank      22 Nov 96         Make this an empty file, effectively.
//                                    See the .h file for explanation.
//    SJ Gowdy       5 May 97         Added BaBar.h for HP templates
//
// Bugs:
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

#include "IfdTypeKey.h"
#include "IfdCounter.h"
#include <cstring>

template <class T>
IfdTypeKey<T>::IfdTypeKey() 
{
  setMyId(); 
  _hashVal = intVal % _nHashBuckets;
}

template <class T>
void
IfdTypeKey<T>::setMyId(void) {
  // this is not a mem. leak.  Do not delete in the destructor.
  //
   static IfdCounter *_myId = 0;
   if (!_myId) _myId= new IfdCounter;
   intVal = _myId->value();
   return;
}

template <class T>
char*
IfdTypeKey<T>::name(const char* newName) {
  // this is not a mem. leak.  Do not delete in the destructor.
  // A non-null newName resets the name.
  // Bug:  a) once set yu may change it, but never fully get rid of it.
  //       b) unprotected strcpy (not strncpy).
  //
   static char* _myName = 0;

   if ( 0 != newName ) {
     delete[] _myName;   // invariant: always 0 or pting to owned mem.
     _myName = new char[ strlen( newName )+1 ];
     strcpy( _myName, newName);
   }

   return _myName;
}

template <class T>
void
IfdTypeKey<T>::print( ostream &o ) const { 
   if ( IfdTypeKey<T>::name(0) ) {
       o << "IfdTypeKey(" << IfdTypeKey<T>::name(0) << "/" << intVal << ")";
   } else {
       o << "IfdTypeKey(" << intVal << ")";
   }
   return;
}



