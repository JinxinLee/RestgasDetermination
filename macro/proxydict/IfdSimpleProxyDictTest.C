//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdSimpleProxyDictTest.cc,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//    Ed Frank      27 Jan 97         Move to dictFE<T> interface.
//    Ed Frank      28 Jan 97         Test no-2ndary key iface.
//    Bertram Kopf (RUB)              Migrated to PandaRoot
// Bugs:
//    This code is uninteligible.
//
//------------------------------------------------------------------------
//
// See if the dictionary mechanism works.
//
// Tests:
//    0.  (implicit in the following:  DictDataProxy<T> works)
//    1.  Can get/put new data types w/o recompiling dictionary code.
//    2.  a Failed get *sets* its ptr to NULL.
//    3.  a failed put returns false.
//    4.  Can get/put multiple instances of a type but 
//           a) only if done w/ distinct keys
//           b) if attempted twice w/ same key, failure happens
//    5.  Cross talk between Type keys and secondary keys
//    6.  Default secondary keys work
//    7.  Extensibility w.r.t. proxy mechanism
//
//    8.  Can put a dictionary inside of a dictionary
//        NB: can't put a dict inside itself (since it takes ownership of
//        its data.)  how do we trap this?
//
//    z.  Memory management.
//

#include <iostream>

#include "IfdSimpleProxyDict.h"
#include "IfdDataProxy.h"
#include "IfdStrKey.h"
#include "IfdTypeKey.h"
#include "IfdTestClasses.h"
#include "IfdAliasProxy.h"
#include "IfdDataProxyTemplate.h"

#ifdef ONLINE_TEMPLATES
class B;
class C;
class D;
class NoIfd;
#include "Ifd.cc"
#include "IfdDataProxy.cc"
#include "IfdTypeKey.cc"
#include "AbsArgVal.cc"
#include "IfdDataProxyTemplate.cc"
#include "IfdAliasProxy.cc"
template class Ifd<B>;
template class Ifd<C>;
template class Ifd<D>;
template class Ifd<NoIfd>;
template class IfdDataProxy<B>;
template class IfdDataProxy<C>;
template class IfdDataProxy<D>;
template class IfdDataProxy<NoIfd>;
template class IfdAliasProxy<B>;
template class IfdAliasProxy<C>;
template class IfdAliasProxy<D>;
template class IfdAliasProxy<NoIfd>;
template class IfdTypeKey<B>;
template class IfdTypeKey<C>;
template class IfdTypeKey<D>;
template class IfdTypeKey<NoIfd>;
template class IfdTypeKey< Ifd<B> >;
template class IfdTypeKey< Ifd<C> >;
template class IfdTypeKey< Ifd<D> >;
template class IfdTypeKey< Ifd<NoIfd> >;
template class AbsArgVal<B>;
template class AbsArgVal<C>;
template class AbsArgVal<D>;
template class AbsArgVal<NoIfd>;
template class IfdDataProxyTemplate<B>;
template class IfdDataProxyTemplate<C>;
template class IfdDataProxyTemplate<D>;
template class IfdDataProxyTemplate<NoIfd>;
#endif

using std::cout;
using std::endl;
using std::ostream;

#ifdef IFDTYPEKEY_SPECIALIZE
   IfdTypeKeySpecialize( B, 0 )
   IfdTypeKeySpecialize( C, 1 )
   IfdTypeKeySpecialize( D, 2 )
#endif


//**
// A faultHandler for a B
//**
B*
b3FaultHandler( const IfdKey& ) {
  B* myB= new B(3);
  cout << " (** b3FaultHandler called **) ";
  return myB;
}


//**
// A Proxy that will test its cache- lets us test
// testProxyCaches()
//**
template <class T>
class cacheTestProxy : public IfdDataProxy<T> {
public:
  cacheTestProxy(T* (*f)(const IfdKey& )) : IfdDataProxy<T>( f ) {}
  virtual void testCache( void ) {
    cout << "cacheTestProxy::testCache:  invalidating cache" << endl;
    delete this->_myT;
    this->_myT=0;
    this->setCacheInvalid();
  }
};

//**
// Test IfdDAtaProxyTemplate
//
class  bProxy : public IfdDataProxyTemplate<B> {
public:
  virtual ~bProxy();
  virtual B*   faultHandler( IfdProxyDict*, const IfdKey&, AbsArg&    );
  virtual void storeHandler( IfdProxyDict*, const IfdKey&, AbsArg&, B* );
  virtual void testCache( void );
};

bProxy::~bProxy() { delete _myT; }


B*
bProxy::faultHandler( IfdProxyDict*, const IfdKey&, AbsArg& ) {
  B* aB = new B(3);
  cout << "(bProxy::faultHandler called.  Bptr of " << aB << ")" << endl;
  setCacheIsValid();
  return aB;
}

void
bProxy::storeHandler( IfdProxyDict*, const IfdKey&, AbsArg&, B* b ) {
  cout << "bProxy::storeHandler called.  Bptr of " << b << endl;
}

void
bProxy::testCache( void ) {
  cout << "bProxy::testCache called." << endl;
  setCacheInvalid();
  delete _myT;
}


//
// Some I/O garbage
//**
ostream& operator <<( ostream& o, bool b ) {
  if ( b )
    o << "(true )";
  else
    o << "(false)";

  return o;
}

ostream& operator <<( ostream& o, const B& a) {
  a.print();
  return o;
}
ostream& operator <<( ostream& o, const C& a) {
  a.print();
  return o;
}
ostream& operator <<( ostream& o, const D& a) {
  a.print();
  return o;
}

void
boolPrint( ostream& o, bool b ) {
  if ( b ) {
    o << "(true) ";
  } else {
    o << "(false) ";
  }
  return;
}


//**
// Testing procedures- hold your nose.  This is going to be
//    gross.
//**



void IfdSimpleProxyDictTest(){
  
  // Output a sed string to allow us to do regression tests.
  // IfdTypeKeys pick up their values from statics, and different
  // systems init them differently.  Here we make a xlation table
  // for this particular run.


  IfdTypeKey< Ifd<B> > tkb;
  IfdTypeKey< Ifd<C> > tkc;
  IfdTypeKey< Ifd<D> > tkd;

  cout << "sed "
       << " -e 's/" << tkb <<"/IfdTypeKey[Ifd<B>]/g' "
       << " -e 's/" << tkc <<"/IfdTypeKey[Ifd<C>]/g' "
       << " -e 's/" << tkd <<"/IfdTypeKey[Ifd<D>]/g' "
       << " -e 's/0[xX][0-9,a-f,A-F]*/0xbogus/g' "
       << " << ENDofTEST"
       << endl;



  // Make the dictionary to test
  //
  cout << "Maing an empty dictionary to test..." << endl;
  IfdSimpleProxyDict *dict = new IfdSimpleProxyDict;
  dict->print(cout, IfdSimpleProxyDict::noAddresses);
  cout << endl;


  // Cannon fodder
  //
  cout << "Making 3 objects each of classes B, C, D." << endl;
  B* b  = 0;
  B* b1 = new B(1);  IfdStrKey kb1( "B[1]" );
  B* b2 = new B(2);  IfdStrKey kb2( "B[2]" );
  B* b3 = new B(3);  // no 2ndary key
  B* b4 = new B(4);  // will use explicit string key, below
  C* c  = 0;
  C* c1 = new C(1);  IfdStrKey kc1( "C[1]" );
  C* c2 = new C(2);  IfdStrKey kc2( "C[2]" );
  C* c3 = new C(3);  // no 2ndary key
  D* d  = 0;
  D* d1 = new D(1);  IfdStrKey kd1( "D[1]" );
  D* d2 = new D(2);  IfdStrKey kd2( "D[2]" );
  D* d3 = new D(3);  // no 2ndary key

  cout << "Making IfdDataProxy for each..."<< endl;
  IfdDataProxy<B>*  pb1 = new IfdDataProxy<B>( b1 );
  IfdDataProxy<B>*  pb2 = new IfdDataProxy<B>( b2 );
  IfdDataProxy<B>*  pb3 = new IfdDataProxy<B>( b3 );
  IfdDataProxy<B>*  pb4 = new IfdDataProxy<B>( b4 );
  IfdDataProxy<C>*  pc1 = new IfdDataProxy<C>( c1 );
  IfdDataProxy<C>*  pc2 = new IfdDataProxy<C>( c2 );
  IfdDataProxy<C>*  pc3 = new IfdDataProxy<C>( c3 );
  IfdDataProxy<D>*  pd1 = new IfdDataProxy<D>( d1 );
  IfdDataProxy<D>*  pd2 = new IfdDataProxy<D>( d2 );
  IfdDataProxy<D>*  pd3 = new IfdDataProxy<D>( d3 );

  cout << "Here each object prints its name.  For convenience we" << endl;
  cout << "will use thse as the secondary keys too." << endl;
  b1->print();  b2->print(); b3->print();
  c1->print();  c2->print(); c3->print();
  d1->print();  d2->print(); d3->print();
  cout << endl;

//    0.  (implicit in the following:  IfdDataProxy works)
//    1.  Can get/put new data types w/o recompiling dictionary code.
//    2''. a failed put returns false.
//    1'. a Failed get *sets* its ptr to NULL.
//    2.  Can get/put multiple instances of a type but 
//           a) only if done w/ distinct keys
//           b) if attempted twice w/ same key, failure happens


  cout << "Tests:" << endl;
  cout << "  -Can get/put new data types w/o recompiling dictionary." << endl;
  cout << "  -A failed put returns false, and a good put returns true."<<endl;
  cout << "  -Can not put same key in twice." << endl;
  cout << "  -Can put same type in multiple times, if w/ differnt keys."<<endl;
  cout << endl;

  cout << "Storing the objects willy nilly.  All of these puts" << endl;
  cout << "should succeed,causing (true) to print 9 times."  << endl;

  boolPrint(cout, Ifd<D>::put( dict, pd3      ));
  boolPrint(cout, Ifd<C>::put( dict, pc2, kc2 ));
  boolPrint(cout, Ifd<B>::put( dict, pb1, kb1 ) );
  boolPrint(cout, Ifd<D>::put( dict, pd2, kd2 ) );
  boolPrint(cout, Ifd<C>::put( dict, pc3      ) );
  boolPrint(cout, Ifd<B>::put( dict, pb3      ) );
  boolPrint(cout, Ifd<C>::put( dict, pc1, kc1 ) );
  boolPrint(cout, Ifd<D>::put( dict, pd1, kd1 ) );
  boolPrint(cout, Ifd<B>::put( dict, pb2, kb2 ) );
  cout << endl;
  cout << "Attempting to put one in again.  This should print (false)" <<endl;
  boolPrint(cout, Ifd<B>::put( dict, pb1, kb1 ) );
  cout << endl << endl;
  cout << "Printing the dictionary again.  Look for 9 entries." << endl;
  dict->print(cout, IfdSimpleProxyDict::noAddresses);
  cout << endl;

  cout << "Pulling out the objects.  Will call each ones print()" << endl;
  cout << "which should match its key." << endl;
  cout << endl;
  cout << kb1 << " fetches ";
  if ( (b=Ifd<B>::get(dict, kb1))!=0 ) 
    { cout << *b;} else { cout << "NULL";} cout<<endl;
  cout << kb2 << " fetches ";
  if ( (b=Ifd<B>::get(dict, kb2))!=0 )
    { cout << *b;} else { cout << "NULL";} cout<<endl;
  cout << "[default B]  fetches ";
  if ( (b=Ifd<B>::get(dict     ))!=0 )
    { cout << *b;} else { cout << "NULL";} cout<<endl;
  cout << kc1 << " fetches ";
  if ( (c=Ifd<C>::get(dict, kc1))!=0 )
    { cout << *c;} else { cout << "NULL";} cout<<endl;
  cout << kc2 << " fetches ";
  if ( (c=Ifd<C>::get(dict, kc2))!=0 )
    { cout << *c;} else { cout << "NULL";} cout<<endl;
  cout << "[default C]  fetches ";
  if ( (c=Ifd<C>::get(dict     ))!=0 )
    { cout << *c;} else { cout << "NULL";} cout<<endl;
  cout << kd1 << " fetches ";
  if ( (d=Ifd<D>::get(dict, kd1))!=0 )
    { cout << *d;} else { cout << "NULL";} cout<<endl;
  cout << kd2 << " fetches ";
  if ( (d=Ifd<D>::get(dict, kd2))!=0 )
    { cout << *d;} else { cout << "NULL";} cout<<endl;
  cout << "[default D]  fetches ";
  if ( (d=Ifd<D>::get(dict     ))!=0 )
    { cout << *d;} else { cout << "NULL";} cout<<endl;
  cout <<endl;

  cout << "Test to make sure that a failed get returns false.  You" << endl;
  cout << "should see B[1] printed, and then NULL printed." << endl;
  // get good value into b
  if ( (b=Ifd<B>::get(dict, kb1))!=0 ) {
    cout << *b;
  } else { 
    cout << "NULL";
  }
  cout<<endl;
  if ( !b ) { cout << "test code hosed."<< endl; ::exit(1); }
  // now do a fetch that *should* fail: use a Key for which there is no B*.
  if ( (b=Ifd<B>::get(dict, kc1))!=0 ){
    cout << *b;
  } else {
    cout << "NULL";}
  cout<<endl;
  cout << endl;

  // Try a faulting proxy
  //
  cout << "Adding a proxy that actually uses a fault handler.  Look for"
       << " (true) " << endl;
      
  IfdStrKey kb3( "B[3]");
  bProxy *ctp = new bProxy();
  boolPrint(cout,  Ifd<B>::put(dict, ctp, kb3 ) );
  cout << endl;
  cout << "Now pulling B[3] back  out twice.  First time should tickle"
       << " the fault handler" << endl;
  cout << kb3 << " fetches ";
  if ( (b=Ifd<B>::get(dict, kb3))!=0 ) {
    cout << *b;
  }  else {
    cout << "NULL";
  }
  cout<<endl;
  cout << "This time should not cause a fault:" << endl;
  cout << kb3 << " fetches ";
  if ( (b=Ifd<B>::get(dict, kb3))!=0 ){
    cout << *b;
  } else { 
    cout << "NULL";
  }
  cout<<endl;
  cout << "Now request tests of the proxy caches via "
       << "ProxyDict::testProxyCache."
       << endl;
  dict->testProxyCaches();

  cout << "Sneak in a keyUsed().  This should print (true) but not "
       << "cause a fault." << endl;
  if ( Ifd<B>::keyUsed(dict, kb3) ) 
    cout << "(true)" << endl; 
  else
    cout << "(false)"<< endl;


  cout << "This time should cause a fault again:" << endl;
  cout << kb3 << " fetches ";
  if ( (b=Ifd<B>::get(dict, kb3))!=0 ){
    cout << *b;
  } else { 
    cout << "NULL";
  }
  cout<<endl;

  cout << "Now testing markForStore().  Look for a T in 'Store' column"<< endl;
  Ifd<B>::markForStore( dict, kb3 );
  dict->print(cout, IfdSimpleProxyDict::noAddresses);
  cout << endl;

  cout << "Now testing storeAllMarked(). T in 'Store' column should clear."
       << endl;
  dict->storeAllMarked();
  dict->print(cout, IfdSimpleProxyDict::noAddresses);
  cout << endl;

  cout << "Testing markForStore() w/ control of flag clearing." << endl;
  cout << "Marking item for store again. Look for a T in 'Store' column"<<endl;
  Ifd<B>::markForStore( dict, kb3 );
  dict->print(cout, IfdSimpleProxyDict::noAddresses);
  cout << endl;

  cout <<"Now storeAllMarked(keepStoreFlags). 'Store' column should stay same."
       << endl;
  dict->storeAllMarked( IfdProxyDict::keepStoreFlags );
  dict->print(cout, IfdSimpleProxyDict::noAddresses);
  cout << endl;

  cout <<"Now storeAllMarked(clearStoreFlags).'Store' column should go to F."
       << endl;
  dict->storeAllMarked( IfdProxyDict::clearStoreFlags );
  dict->print(cout, IfdSimpleProxyDict::noAddresses);
  cout << endl;

  // See if strinigs as seconday keys work.
  //
  cout << "Try the explicit string-as-key interface.  Look for (true)" << endl;
  boolPrint(cout, Ifd<B>::put( dict, pb4, "strTest" )  );
  cout << endl;

  cout << "Now pull it back out via the string, (strTest) :";
  if ( (b=Ifd<B>::get(dict, "strTest"))!=0 ) {
    cout << *b;
  }  else {
    cout << "NULL";
  }  
  cout << endl;
  cout << "Now pull it back out via and IfdStrKey (crosswire test): ";
  IfdStrKey xwireKey( "strTest" );
  if ( (b=Ifd<B>::get(dict, xwireKey))!=0 ) {
    cout << *b;
  }  else {
    cout << "NULL";
  }  
  cout << endl;

  cout << "Testing alias proxy..."<< endl;
  dict->testProxyCaches();
  if ( Ifd<B>::put( dict, new IfdAliasProxy<B>( kb3 ), "alias" ) ) {
    cout << "Put in an alias to the faulting proxy.  Expect a " << endl;
    cout << "fault here: " ;

    if ( (b=Ifd<B>::get(dict, "alias"))!=0 ) {
      cout << *b << endl;
    }  else {
      cout << "NULL" << endl;
    }  
  } else {
    cout << "Failed to create alias proxy." << endl;
  }
  cout << "Now call faulting proxy directly.  Do not expect a";
  cout << "fault this time: ";
  if ( (b=Ifd<B>::get(dict, kb3))!=0 ) {
    cout << *b << endl;
  }  else {
    cout << "NULL" << endl;
  }
  cout << endl;

  cout << "Inserting a proxy for non-IfdData data." << endl;
  NoIfd* nifd = new NoIfd( 0 );
  IfdDataProxy<NoIfd>*  pNoIfd = new IfdDataProxy<NoIfd>( nifd );
  nifd = 0;   // proxy owns it now.  acquire amnesia...

  if ( Ifd<NoIfd>::put( dict, pNoIfd) ) {
    cout << "Fetching NoIfd data returns: ";
    if ( (nifd=Ifd<NoIfd>::get(dict))!=0 ) {
      cout << *nifd << endl;
    }  else {
      cout << "NULL" << endl;
    }  
  } else {
    cout << "Failed to insert proxy for test." << endl;
  }
  
  //*
  // Test interface that creates a simple proxy for the user.
  //*


  cout << endl << "Testing 'proxy-free' put..." << endl;

  nifd = new NoIfd( 1 );
  if ( Ifd<NoIfd>::put( dict, nifd, "proxyLess" ) ) {
    nifd = 0;
    cout << "Fetching NoIfd data returns: ";
    if ( (nifd=Ifd<NoIfd>::get(dict, "proxyLess" ))!=0 ) {
      cout << *nifd << endl;
    }  else {
      cout << "NULL" << endl;
    }  
  } else {
    cout << "Failed to do proxy-less insert." << endl;
  }
  

  cout << endl;
  cout << "final dump of dict." << endl;
  dict->print(cout, IfdSimpleProxyDict::noAddresses);
  cout << endl;


  //************
  // Test dtor
  //************
  cout << "Testing the deletion of an IfdSimpleProxyDict" << endl;
  delete dict;
  cout << "Done w/ deletion" << endl;

  cout << "ENDofTEST" << endl;

  exit(0);    
}


