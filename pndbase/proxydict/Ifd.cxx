//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: Ifd.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    See the .h file.
//
// Implementation notes.
//
// 2.  Some compilers do not support default args in
//     templated member functions.  That's why we
//     have pairs of member functions, one w/ a 2ndary
//     key and one without.
// 
// 3.  Ideally, the pairs of functions above would call
//     into a common implementation helper function.  Too
//     bad.
//
//
//
//      EXCLUDE THIS FILE FROM THE LIST OF .CC FILE TO COMPILE!
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Ed Frank		University of Pennsylvania
//
// Copyright Information:
//	Copyright (C) 1997
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "Ifd.h"

#include "IfdProxyDict.h"
#include "IfdDictKey.h"
#include "IfdKey.h"
#include "IfdTypeKey.h"
#include "IfdStrKey.h"

#include "IfdDataProxyTemplate.h"
#include "IfdDataProxy.h"
//#include "AbsArgVal.h"

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------

//--------------
// Destructor --
//--------------

//-------------
// Methods   --
//-------------


//**********************************************************************
//template <class T>
//Ifd<T>::~Ifd() 
//**********************************************************************
//{}

//**********************************************************************
template <class T>
AbsArg&
Ifd<T>::nullArg()  {
//**********************************************************************
  static AbsArgVal<T> _nullArg( (T*) 0 );  
  return _nullArg;
}

//**********************************************************************
template <class T>
T* Ifd<T>::get( IfdProxyDict *d ) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> > tK;   // The static keeps us from creating one on
				    // each call.
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );
  return (T*) d->get( *dictKey, nullArg() );
}

//**********************************************************************
template <class T>
T* Ifd<T>::get( IfdProxyDict *d, AbsArg& a ) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> >   tK;
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

  return (T*) d->get( *dictKey, a );
}

//**********************************************************************
template <class T>
T* Ifd<T>::get( IfdProxyDict *d, const IfdKey &k) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> >   tK;  
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

  dictKey->reset();  
  dictKey->add( k ); 
  return (T*) d->get( *dictKey, nullArg() );
}

//**********************************************************************
template <class T>
T* Ifd<T>::get( IfdProxyDict *d, const IfdKey &k, AbsArg& a) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> >   tK;  
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

  dictKey->reset();
  dictKey->add( k );
  return (T*) d->get( *dictKey, a);
}

//**********************************************************************
template <class T>
T* Ifd<T>::get( IfdProxyDict *d, const char *s) {
//**********************************************************************

  IfdStrKey k( s );
  return Ifd<T>::get(d, k);
}

//**********************************************************************
template <class T>
T* Ifd<T>::get( IfdProxyDict *d, const char *s, AbsArg& a) {
//**********************************************************************

  IfdStrKey k( s );
  return Ifd<T>::get(d, k, a);
}

//**********************************************************************
template <class T>
bool Ifd<T>::put( IfdProxyDict *d, IfdDataProxyTemplate<T> *p ) {
//**********************************************************************

    static IfdTypeKey< Ifd<T> >   tK;    
    static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

    return d->put(*dictKey, p);
}


//**********************************************************************
template <class T>
bool Ifd<T>::put(IfdProxyDict *d, IfdDataProxyTemplate<T> *p, const IfdKey &k){
//**********************************************************************
    static IfdTypeKey< Ifd<T> >   tK;    
    static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

    dictKey->reset();
    dictKey->add( k );
    return d->put(*dictKey, p);
}

//**********************************************************************
template <class T>
bool Ifd<T>::put(IfdProxyDict *d, IfdDataProxyTemplate<T> *p, const char *s ) {
//**********************************************************************
  
  IfdStrKey k( s );
  return Ifd<T>::put( d, p, k );
}


//**********************************************************************
template <class T>
bool Ifd<T>::put(IfdProxyDict *d, T* t) {
//**********************************************************************

    static IfdTypeKey< Ifd<T> >   tK;    
    static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

    // Make a proxy for the (lazy rotten) user.
    IfdDataProxy<T> *p = new IfdDataProxy<T>( t );

    bool result = d->put(*dictKey, p);

    return  result;
}


//**********************************************************************
template <class T>
bool Ifd<T>::put(IfdProxyDict *d, T* t, const IfdKey &k) {
//**********************************************************************
    static IfdTypeKey< Ifd<T> >   tK;    
    static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

    // Make a proxy for the (lazy rotten) user.
    IfdDataProxy<T> *p = new IfdDataProxy<T>( t );

    dictKey->reset();
    dictKey->add( k );
    return d->put(*dictKey, p);
}

//**********************************************************************
template <class T>
bool Ifd<T>::put(IfdProxyDict *d, T* t, const char *s) {
//**********************************************************************
  
  IfdStrKey k( s );
  return Ifd<T>::put( d, t, k );
}

//**********************************************************************
template <class T>
bool Ifd<T>::markForStore(IfdProxyDict *d ) {
//**********************************************************************
    static IfdTypeKey< Ifd<T> >   tK;    
    static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

    return d->markForStore(*dictKey, nullArg());
}

//**********************************************************************
template <class T>
bool Ifd<T>::markForStore(IfdProxyDict *d, AbsArg& a ) {
//**********************************************************************
    static IfdTypeKey< Ifd<T> >   tK;    
    static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

    return d->markForStore(*dictKey, a);
}

//**********************************************************************
template <class T>
bool Ifd<T>::markForStore(IfdProxyDict *d, const IfdKey& k ) {
//**********************************************************************
    static IfdTypeKey< Ifd<T> >   tK;    
    static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

    dictKey->reset();
    dictKey->add( k );
    return d->markForStore(*dictKey, nullArg());
}

//**********************************************************************
template <class T>
bool Ifd<T>::markForStore(IfdProxyDict *d, const IfdKey& k, AbsArg& a ) {
//**********************************************************************
    static IfdTypeKey< Ifd<T> >   tK;    
    static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

    dictKey->reset();
    dictKey->add( k );
    return  d->markForStore(*dictKey, a);
}

//**********************************************************************
template <class T>
bool Ifd<T>::markForStore(IfdProxyDict *d, const char *s) {
//**********************************************************************
  IfdStrKey k( s );
  return Ifd<T>::markForStore(d, k);
}

//**********************************************************************
template <class T>
bool Ifd<T>::markForStore(IfdProxyDict *d, const char *s, AbsArg& a) {
//**********************************************************************
  IfdStrKey k( s );
  return Ifd<T>::markForStore(d, k, a);
}


//**********************************************************************
template <class T>
void Ifd<T>::storeItem(IfdProxyDict *d ) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> >   tK;  
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

  d->storeItem(*dictKey, nullArg());
  return;
}

//**********************************************************************
template <class T>
void Ifd<T>::storeItem(IfdProxyDict *d, AbsArg& a ) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> >   tK;  
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

  d->storeItem(*dictKey, a);
  return;
}

//**********************************************************************
template <class T>
void Ifd<T>::storeItem(IfdProxyDict *d, const IfdKey& k ) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> >   tK;  
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

  dictKey->add( k );
  d->storeItem(*dictKey, nullArg());
  dictKey->reset();
  return;

}

//**********************************************************************
template <class T>
void Ifd<T>::storeItem(IfdProxyDict *d, const IfdKey& k, AbsArg& a ) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> >   tK;  
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

  dictKey->add( k );
  d->storeItem(*dictKey, a);
  dictKey->reset();
  return;
}

//**********************************************************************
template <class T>
void Ifd<T>::storeItem(IfdProxyDict *d, const char* s) {
//**********************************************************************
  IfdStrKey k(s);
  Ifd<T>::storeItem(d, k);
  return; 
}

//**********************************************************************
template <class T>
void Ifd<T>::storeItem(IfdProxyDict *d, const char* s, AbsArg& a) {
//**********************************************************************
  IfdStrKey k(s);
  Ifd<T>::storeItem(d, k, a);
  return;
}

//**********************************************************************
template <class T>
bool Ifd<T>::keyUsed(IfdProxyDict *d) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> >   tK;  
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK);

  return ( 0 != d->find( *dictKey ) );
}

//**********************************************************************
template <class T>
bool Ifd<T>::keyUsed(IfdProxyDict *d, const IfdKey& k) {
//**********************************************************************
  static IfdTypeKey< Ifd<T> >   tK;  
  static IfdDictKey*  dictKey=IfdProxyDictFrontEnd::newDictKey( tK );

  dictKey->reset();
  dictKey->add( k );
  return ( 0 != d->find( *dictKey ) );
}

//-------------
// Operators --
//-------------
    
//-------------
// Selectors --
//-------------
    
//-------------
// Modifiers --
//-------------

//		-----------------------------------------------
// 		-- Static Data & Function Member Definitions --
//		-----------------------------------------------

//		-------------------------------------------
// 		-- Protected Function Member Definitions --
//		-------------------------------------------

//		-----------------------------------------
// 		-- Private Function Member Definitions --
//		-----------------------------------------

//		-----------------------------------
// 		-- Internal Function Definitions --
//		-----------------------------------
