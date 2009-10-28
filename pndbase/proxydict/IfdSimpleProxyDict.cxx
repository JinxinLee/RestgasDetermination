//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdSimpleProxyDict.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//    Ed Frank      24 Feb 97         Add invalidateProxyCaches().
//                  30 May 97         change name of above to testProxyCaches()
//    Ed Frank      18 Sep 97         Reimplement w/o CLHEP package.
//
// Bugs:
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <string.h>     // for memcpy

#include "IfdSimpleProxyDict.h"
#include "IfdSimpleProxyDictEntry.h"
#include "IfdDataProxyIFace.h"
#include "IfdDictKey.h"
#include "AbsArg.h"

using std::endl;
using std::ostream;
using std::setw;


IfdSimpleProxyDict::IfdSimpleProxyDict( )
   : _myData(0)
   , _nDictEntries(0)   
   , _deletionHead(0)
   , _deletionTail(0)
{

  // allocate space for the hash table and zero it.

  _dictSize = IfdKey::nHashBuckets();
  _myData = new IfdSimpleProxyDictEntry* [_dictSize];

  memset((void*)_myData, 0, (sizeof _myData[0]) *_dictSize);
}

IfdSimpleProxyDict::~IfdSimpleProxyDict() {
  IfdSimpleProxyDictEntry* e;   // present location in deletion list
  IfdSimpleProxyDictEntry* d;   // guy to delete

  e=_deletionHead;
  while (e !=0 ) {
    d=e;
    e=e->nextDelete;
    delete d;
  }

  delete [] _myData;
  _myData=0;
}

IfdDataProxyIFace*
IfdSimpleProxyDict::find( const IfdDictKey& k ) const {
  IfdSimpleProxyDictEntry *e = findEntry( k );
  if ( 0 != e ) {
    return e->proxy;
  } else {
    return 0;
  }
}

IfdSimpleProxyDictEntry*
IfdSimpleProxyDict::findEntry( const IfdKey& k ) const {
  IfdSimpleProxyDictEntry* e;

  e = _myData[k._hashVal];

  if ( 0==e) {
    return 0;
  } else if ( *e->key == k ) {
    return e;
  } else while ( 0 != (e=e->next ) ) {
    if (*e->key == k) { return e;}
  }

  return 0;

}

void
IfdSimpleProxyDict::add( const IfdDictKey& k, IfdDataProxyIFace* p ) {
  IfdSimpleProxyDictEntry *e_new=new IfdSimpleProxyDictEntry( k, p );
  IfdSimpleProxyDictEntry *e;

  e=_myData[k._hashVal];	// look at the slot we hashed to
  if (0 != e) {			// if a hashing collision...
    e_new->next=e;		// make new entry the new head of the collision
  }				// list
  _myData[k._hashVal]=e_new;	// collision or not, the new guy is 1st in slot

  _nDictEntries++;

  // Keep track of the insertion order so that we can delete in the
  // same order.  Order of deletion is something I never wanted to
  // guarantee, but tracking needs it so we provide it for now.  We
  // for a linked list that tells us how to delete.  See also the dtor.

  if (0 == _deletionHead ) {
    assert( _deletionTail == 0 );
    _deletionTail = e_new;
    _deletionTail->nextDelete=0;
    _deletionHead = _deletionTail;
  } else {
    _deletionTail->nextDelete = e_new;   // append node to list
    _deletionTail = e_new;	         // make it the tail.
    _deletionTail->nextDelete=0;         // terminate the list.
  }
}

void
IfdSimpleProxyDict::testProxyCaches( void ) {
  IfdSimpleProxyDictEntry* e;
  for (size_t i=0; i<_dictSize; i++) {
    e=_myData[i];
    while ( 0 != e ) {
      e->proxy->testCache();
      e=e->next;
    }
  }
}

void
IfdSimpleProxyDict::print( ostream& o, const PrintType prtCtl ) const {
  o << "IfdSimpleProxyDict with " << _nDictEntries << " entries";
  if ( 0 == _nDictEntries ) {
    o << "." << endl;
    return;
  } else {
    o << ":" << endl;
  }

  if ( prtCtl == noAddresses ) {
    o << "  " << setw(4) << "Mrk" << " Key" << endl;
  } else if ( prtCtl == showAddresses ) {
    o << "  "   << setw(10) << "&Proxy" << setw(11) << "&arg" 
      << setw(4) << "Mrk" << " Key" << endl;
  }

  IfdSimpleProxyDictEntry* e;
  for( size_t i=0; i<_dictSize; i++) {
    e = _myData[i];
    while ( 0 != e ) {
      o << "  ";
      if (prtCtl == showAddresses ) {
	o << setw(10) << e->proxy;
	o << setw(11) << e->arg;
      }
      if (e->storeFlag)
	o << setw(4) << "T ";
      else
	o << setw(4) << "F ";
      o << " " << *e->key;
      o << endl;
      e=e->next;
    }
  }
}


bool
IfdSimpleProxyDict::markForStore( const IfdDictKey& k, AbsArg& a) {
  IfdSimpleProxyDictEntry* e = findEntry( k );
  if ( 0 != e ) {
    e->storeFlag = true;
    e->arg       = a.clone();
    return true;
  } else {
    return false;
  }
}


void
IfdSimpleProxyDict::storeAllMarked( const StoreFlagControl c ) {
  IfdSimpleProxyDictEntry* e;

  for( size_t i=0; i<_dictSize; i++) {
    e=_myData[i];
    while ( 0 != e ) {
      if ( e->storeFlag ) {
	e->proxy->store(this, *e->key, *e->arg);
	e->storeFlag = false || ( c == keepStoreFlags ) ;
	if (c==clearStoreFlags) {delete e->arg;}
	e->arg = 0;
      }
      e=e->next;
    }
  }

}
