//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdTypeKeyIFace.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    Implementation of IfdTypeKeyIFace.  See .h file.
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      02 Dec 97         Creation of first version
//
//
// Bugs:
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------


#include "IfdTypeKeyIFace.h"


IfdTypeKeyIFace::IfdTypeKeyIFace() 
   : IfdKey( typeKey )
{}


IfdTypeKeyIFace::~IfdTypeKeyIFace() {
}

void
IfdTypeKeyIFace::print( ostream &o ) const {
  o << "IfdTypeKeyIFace::print:  error:  use of base class print." << endl;
}

void
IfdTypeKeyIFace::dumpName( ostream& o ) const {
  // just print() with a newline to force a flush.  Intended for
  // use in a debugger.

  print( o );
  o<<endl;
}











