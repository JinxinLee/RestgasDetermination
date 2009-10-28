//-----------------------------------------------------------
// File and Version Information:
// $Id: IfdStdVectorProxy.cxx,v 1.2 2005/07/22 14:41:47 sneubert Exp $
//
// Description:
//      Implementation of class IfdStdVectorProxy
//      see IfdStdVectorProxy.h for details
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


// This Class' Header ------------------
#include "IfdStdVectorProxy.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------

template <class T>
IfdStdVectorProxy<T>::IfdStdVectorProxy( vector<T>* v )
  : IfdDataProxy< vector<T> >(v)
{}

template <class T>
IfdStdVectorProxy<T>::IfdStdVectorProxy( vector<T>* (*f)(const IfdKey& k))
  : IfdDataProxy< vector<T> >(f)
{}

template <class T>
IfdStdVectorProxy<T>::~IfdStdVectorProxy() {
  if (0== this->_myT) return;
  int n=this->_myT->size();
  for(int i=0;i<n;++i) delete (*this->_myT)[i];
  this->_myT->clear();
  delete this->_myT;
  this->_myT=0;
}

