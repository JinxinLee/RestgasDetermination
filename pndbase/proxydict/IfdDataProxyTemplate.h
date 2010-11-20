#ifndef IFDDATAPROXYTEMPLATE_HH
#define IFDDATAPROXYTEMPLATE_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdDataProxyTemplate.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    The term "Template" in this class name refers to the Template
//    pattern.  See, for exmaple, Design Patterns,  Gamma et al.
//    A Template pattern means that we implement an algorithm at the
//    top of an inheritance tree in terms of virtual functions provided
//    further down in the inheritance.   Here, we implement the rules
//    for doing a get() that guarantee type safty in terms of faultHandler(),
//    and other, methods provided by derivatives of IfdDataProxyTemplate.
//    We then setup the frontend, i.e., Ifd<T>, so that the put() method
//    only .cxxepts derivatives of IfdDataProxyTemplate.
//
//    If you need to write a proxy class, you normally only need to 
//    inherit from IfdDataProxyTemplate and then "fill in the blanks,"
//    providing the following functions:
//       0.  Memory management
//              The algothrighm implemented here does caching.
//              The T* it caches into is NOT OWNED by this
//              class.  You must handle deleting it, if appropriate
//              in your derivative.
//       1.  faultHandler()
//              Called during a get() whenever the cache is
//              invalid.  Cache validity is determined by the
//              boolean isValid() and defaults to false.  Your last
//              operation in faultHandler should be to call
//              markCacheValid() *if* you want caching behavior.
//              Otherwise, every call to get() will cause a call to the
//              faultHandler().
//       2.  storeHandler()
//
//
//    Note that implementation is in this
//    file not because I want to inline it, but because our compilers
//    were having trouble finding the implementation when instantiating
//    templates in client code (esp. DEC cxx)
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      27 May 96         Creation of first version
//    Ed Frank      28 Aug 97         Replace IfdData* with void*
//
// BUGS:
//   1.  The corresponding .cxx file is completely comments and all methods
//       are defined inline here (although not declared inline).  This
//       is forced on us by the DEC cxx compiler:  I simply can not get
//       cxx to instantiate these methods and emit a vtab otherwise.
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

// Please see the comments in IfdDataProxyIFace.h
//

#include <stdlib.h>
#include "IfdDataProxyIFace.h"


class IfdKey;
class AbsArg;
class IfdProxyDict;

template <class T>
class IfdDataProxyTemplate : public IfdDataProxyIFace {
public:

  //  Note: The workaround to a VxWorks template instantiation bug 
  // requires that the destructor be the first function defined in a
  // templated class.  Please do not move this or put functions above it!

  virtual ~IfdDataProxyTemplate() {}

protected:
  IfdDataProxyTemplate();

public:

  virtual void* get(IfdProxyDict* d,  const IfdKey& k, AbsArg& a );

  virtual void  store( IfdProxyDict*d, const IfdKey& k, AbsArg& a);

  virtual T*   faultHandler( IfdProxyDict*, const IfdKey&, AbsArg&    )=0;
  virtual void storeHandler( IfdProxyDict*, const IfdKey&, AbsArg&,T* )=0;
  virtual void testCache( void ) = 0;

  bool    cacheIsValid()    { return _cacheIsValid; }
  void    setCacheIsValid() { _cacheIsValid = true; }
  void    setCacheInvalid() { _cacheIsValid = false; }

protected:
  T* _myT;		// derivative class must delete, if appropriate.

private:
  bool _cacheIsValid; 

  // Copy ctor and assignment op:  not allowed.
  //   Note:  we must define thise (not just declare them) or
  //   DEC CXX has trouble instantiating templates.  hence the
  //   aborts, eventhough I'd rather just get a link error
  //
  IfdDataProxyTemplate( const IfdDataProxyTemplate<T>&  ) { ::abort(); }
  IfdDataProxyTemplate<T>& operator=(const IfdDataProxyTemplate<T>& )
     { ::abort();  return *this; }
};

#include  "IfdDataProxyTemplate.cxx"

#endif /*  IFDDATAPROXYTEMPLATE_HH */
