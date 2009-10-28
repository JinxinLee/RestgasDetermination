#ifndef IFDDATAPROXY_HH
#define IFDDATAPROXY_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdDataProxy.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
//    *** THIS CLASS IS DEPRECATED. ***
//   
//
//    WARNING:  this proxy just calls delete on it's T* when being
//    destructed.  This is _wrong_ for cases where T is a container
//    that owns it's data but does not delete its data in its constructor.
//    That is a contradiction in terms, but it is what we have in real
//    life with HepAList and the BaBar AbsEvent design.
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//    Ed Frank      24 Feb 97         add invalidateCache(void)
//                  30 May 97         change name of above to testCahce.
//
// Bugs:
//    Memory leak if the T* does not delete what it owns in its destructor.
//
//------------------------------------------------------------------------

// Please see the comments in IfdDataProxyIFace.h
//

#include "IfdDataProxyTemplate.h"


class IfdKey;
class IfdProxyDict;
class IfdAbsArg;

template <class T>
class IfdDataProxy : public IfdDataProxyTemplate<T> {
public:

  //  Note: The workaround to a VxWorks template instantiation bug 
  // requires that the destructor be the first function defined in a
  // templated class.  Please do not move this or put functions above it!

  virtual ~IfdDataProxy() { delete this->_myT;} // possible mem leak here. 
                                           // See notes below.

  IfdDataProxy(T* t);

  IfdDataProxy(T* (*f)(const IfdKey& ));
     // Makes a proxy that caches a T* and uses a T* handler(Key& k) to
     // set the T*, which starts off as NULL.

  virtual T*   faultHandler( IfdProxyDict*, const IfdKey&, AbsArg&     );
  virtual void storeHandler( IfdProxyDict*, const IfdKey&, AbsArg&, T* );
  virtual void testCache( void );
  

protected:
   IfdDataProxy();
      //This stupid ctor makes it easer to write a copy ctor in a
      //derived class that just calls ::abort().  There must be a
      //better way, but this is good enough for now.

   // These are protected rather than private because derivaties
   // for Proxy'ing container classes may need to do deletions
   // in the _myT.  See, for example, PDHepAListProxy.

   T* (*handler)( const IfdKey& k);	// ptr to a fn of Key& returning T*

private:
  // Copy ctor and assignment op:  not allowed.
  //   Note:  we must define thise (not just declare them) or
  //   DEC CXX has trouble instantiating templates.  hence the
  //   aborts, eventhough I'd rather just get a link error
  //
  IfdDataProxy( const IfdDataProxy<T>&  ) { ::abort(); }
  IfdDataProxy<T>& operator=(const IfdDataProxy<T>& )
     { ::abort();  return *this; }

};

 
#include  "IfdDataProxy.cxx"

#endif
