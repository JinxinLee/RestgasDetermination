//-----------------------------------------------------------
// File and Version Information:
// $Id: IfdStdVectorProxy.h,v 1.2 2005/07/22 14:41:47 sneubert Exp $
//
// Description:
//      A proxy for std::vector.
//      Difference to IfdHepAListProxy: Note that this Proxy will
//      _NOT_ delete all the items in the vector when it is destructed.
//      If you put ordinary pointers into the vector you can create
//      a memory leak!
//
//      Note, that this proxy does not override testCache(void) which is
//      provided by the base class IfdDataProxy<T>. Therefore this Proxy 
//      will never think its cache may be invalid once it has been set.
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//      Bertram Kopf (RUB) modified and migrated to PandaRoot
//-----------------------------------------------------------

#ifndef IFDSTDVECTORPROXY_HH
#define IFDSTDVECTORPROXY_HH

// Base Class Headers ----------------
#include "IfdDataProxy.h"

// Collaborating Class Headers -------
#include <vector>
using std::vector;
// Collaborating Class Declarations --


template <class T>
class IfdStdVectorProxy: public IfdDataProxy< vector<T> > {
public:

  // Constructors/Destructors ---------
  virtual ~IfdStdVectorProxy();

  IfdStdVectorProxy(vector<T>* v);
  IfdStdVectorProxy(vector<T>* (*f)(const IfdKey& k));
  
private:

  // Private Methods -----------------
  // Copy Constructor and assignment operator are forbidden
  IfdStdVectorProxy( const IfdStdVectorProxy<T>& ) { ::abort(); }
  IfdStdVectorProxy<T>& operator= ( const IfdStdVectorProxy<T>& )
  { ::abort(); return *this;}

};

#include "IfdStdVectorProxy.cxx"

#endif
